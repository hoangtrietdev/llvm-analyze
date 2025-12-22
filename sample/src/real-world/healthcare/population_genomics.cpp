// Population Genomics: Variant Call Parallelization
// Parallel genotype calling and population genetics analysis
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <map>

class PopulationGenomics {
public:
    struct Variant {
        int position;
        char ref;
        char alt;
        double frequency;
        std::vector<int> genotypes;  // 0=ref/ref, 1=ref/alt, 2=alt/alt
    };
    
    struct Sample {
        std::string id;
        std::vector<char> sequence;
        std::string population;
    };
    
    std::vector<Sample> samples;
    std::vector<Variant> variants;
    int genomeLength;
    
    PopulationGenomics(int length) : genomeLength(length) {}
    
    // Call variants from multiple samples
    void callVariants(int minCoverage = 4) {
        variants.clear();
        
        // For each position in genome
        for (int pos = 0; pos < genomeLength; pos++) {
            std::map<char, int> alleleCounts;
            
            // Count alleles at this position
            for (const auto& sample : samples) {
                if (pos < static_cast<int>(sample.sequence.size())) {
                    char allele = sample.sequence[pos];
                    alleleCounts[allele]++;
                }
            }
            
            if (alleleCounts.size() < 2) continue;
            
            // Find major and minor alleles
            char ref = 'A', alt = 'A';
            int maxCount = 0, secondCount = 0;
            
            for (const auto& pair : alleleCounts) {
                if (pair.second > maxCount) {
                    secondCount = maxCount;
                    alt = ref;
                    maxCount = pair.second;
                    ref = pair.first;
                } else if (pair.second > secondCount) {
                    secondCount = pair.second;
                    alt = pair.first;
                }
            }
            
            if (secondCount < minCoverage) continue;
            
            // Create variant
            Variant var;
            var.position = pos;
            var.ref = ref;
            var.alt = alt;
            var.frequency = static_cast<double>(secondCount) / 
                          (maxCount + secondCount);
            
            // Call genotypes
            var.genotypes.resize(samples.size());
            for (size_t i = 0; i < samples.size(); i++) {
                char allele = samples[i].sequence[pos];
                if (allele == ref) {
                    var.genotypes[i] = 0;  // Homozygous ref
                } else if (allele == alt) {
                    var.genotypes[i] = 2;  // Homozygous alt
                } else {
                    var.genotypes[i] = 1;  // Heterozygous (simplified)
                }
            }
            
            variants.push_back(var);
        }
    }
    
    // Calculate linkage disequilibrium between variants
    double calculateLD(int var1Idx, int var2Idx) {
        if (var1Idx >= static_cast<int>(variants.size()) || 
            var2Idx >= static_cast<int>(variants.size())) {
            return 0.0;
        }
        
        const auto& v1 = variants[var1Idx];
        const auto& v2 = variants[var2Idx];
        
        // Count haplotypes
        int count00 = 0, count01 = 0, count10 = 0, count11 = 0;
        int n = samples.size();
        
        for (int i = 0; i < n; i++) {
            int g1 = v1.genotypes[i];
            int g2 = v2.genotypes[i];
            
            // Simplified: treat genotypes as haploid
            if (g1 == 0 && g2 == 0) count00++;
            else if (g1 == 0 && g2 > 0) count01++;
            else if (g1 > 0 && g2 == 0) count10++;
            else if (g1 > 0 && g2 > 0) count11++;
        }
        
        // Calculate D (linkage disequilibrium coefficient)
        double p1 = v1.frequency;
        double p2 = v2.frequency;
        double p11 = static_cast<double>(count11) / n;
        
        double D = p11 - p1 * p2;
        
        // R-squared
        if (p1 * (1 - p1) * p2 * (1 - p2) > 0) {
            double Dmax = std::min(p1 * p2, (1 - p1) * (1 - p2));
            double rSquared = (D * D) / (p1 * (1 - p1) * p2 * (1 - p2));
            return rSquared;
        }
        
        return 0.0;
    }
    
    // Calculate LD matrix (parallelizable)
    std::vector<std::vector<double>> calculateLDMatrix(int maxVariants) {
        int n = std::min(maxVariants, static_cast<int>(variants.size()));
        std::vector<std::vector<double>> ldMatrix(n, std::vector<double>(n));
        
        for (int i = 0; i < n; i++) {
            ldMatrix[i][i] = 1.0;
            for (int j = i + 1; j < n; j++) {
                double ld = calculateLD(i, j);
                ldMatrix[i][j] = ld;
                ldMatrix[j][i] = ld;
            }
        }
        
        return ldMatrix;
    }
    
    // Principal Component Analysis for population structure
    std::vector<std::vector<double>> calculatePCA(int numComponents) {
        int n = samples.size();
        int m = std::min(1000, static_cast<int>(variants.size()));  // Use subset
        
        // Create genotype matrix (standardized)
        std::vector<std::vector<double>> matrix(n, std::vector<double>(m));
        
        for (int j = 0; j < m; j++) {
            double mean = 0.0;
            for (int i = 0; i < n; i++) {
                matrix[i][j] = variants[j].genotypes[i];
                mean += matrix[i][j];
            }
            mean /= n;
            
            // Standardize
            double variance = 0.0;
            for (int i = 0; i < n; i++) {
                matrix[i][j] -= mean;
                variance += matrix[i][j] * matrix[i][j];
            }
            variance /= n;
            double std = std::sqrt(variance);
            
            if (std > 0) {
                for (int i = 0; i < n; i++) {
                    matrix[i][j] /= std;
                }
            }
        }
        
        // Compute covariance matrix
        std::vector<std::vector<double>> cov(n, std::vector<double>(n, 0.0));
        
        for (int i = 0; i < n; i++) {
            for (int j = i; j < n; j++) {
                double sum = 0.0;
                for (int k = 0; k < m; k++) {
                    sum += matrix[i][k] * matrix[j][k];
                }
                cov[i][j] = sum / m;
                cov[j][i] = cov[i][j];
            }
        }
        
        // Power iteration for top eigenvectors (simplified PCA)
        std::vector<std::vector<double>> pcs(n, std::vector<double>(numComponents));
        
        for (int comp = 0; comp < numComponents; comp++) {
            std::vector<double> v(n, 1.0 / std::sqrt(n));
            
            // Power iteration
            for (int iter = 0; iter < 100; iter++) {
                std::vector<double> Av(n, 0.0);
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        Av[i] += cov[i][j] * v[j];
                    }
                }
                
                // Normalize
                double norm = 0.0;
                for (double x : Av) norm += x * x;
                norm = std::sqrt(norm);
                
                for (int i = 0; i < n; i++) {
                    v[i] = Av[i] / norm;
                }
            }
            
            for (int i = 0; i < n; i++) {
                pcs[i][comp] = v[i];
            }
            
            // Deflate covariance matrix
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    cov[i][j] -= v[i] * v[j];
                }
            }
        }
        
        return pcs;
    }
    
    // Fst calculation between populations
    double calculateFst(const std::string& pop1, const std::string& pop2) {
        double totalHt = 0.0, totalHs = 0.0;
        int variantCount = 0;
        
        for (const auto& var : variants) {
            // Split samples by population
            std::vector<int> geno1, geno2;
            
            for (size_t i = 0; i < samples.size(); i++) {
                if (samples[i].population == pop1) {
                    geno1.push_back(var.genotypes[i]);
                } else if (samples[i].population == pop2) {
                    geno2.push_back(var.genotypes[i]);
                }
            }
            
            if (geno1.empty() || geno2.empty()) continue;
            
            // Calculate allele frequencies
            double p1 = 0.0, p2 = 0.0;
            for (int g : geno1) p1 += g;
            for (int g : geno2) p2 += g;
            p1 /= (2.0 * geno1.size());
            p2 /= (2.0 * geno2.size());
            
            // Total heterozygosity
            double pTotal = (p1 * geno1.size() + p2 * geno2.size()) / 
                          (geno1.size() + geno2.size());
            double Ht = 2.0 * pTotal * (1.0 - pTotal);
            
            // Within-population heterozygosity
            double Hs1 = 2.0 * p1 * (1.0 - p1);
            double Hs2 = 2.0 * p2 * (1.0 - p2);
            double Hs = (Hs1 * geno1.size() + Hs2 * geno2.size()) / 
                       (geno1.size() + geno2.size());
            
            totalHt += Ht;
            totalHs += Hs;
            variantCount++;
        }
        
        if (variantCount == 0 || totalHt == 0) return 0.0;
        
        return (totalHt - totalHs) / totalHt;
    }
    
    // Tajima's D for selection detection
    double calculateTajimasD(int windowStart, int windowSize) {
        std::vector<Variant> windowVars;
        
        for (const auto& var : variants) {
            if (var.position >= windowStart && 
                var.position < windowStart + windowSize) {
                windowVars.push_back(var);
            }
        }
        
        if (windowVars.empty()) return 0.0;
        
        int n = samples.size();
        int S = windowVars.size();  // Number of segregating sites
        
        // Calculate pairwise differences
        double pi = 0.0;
        for (const auto& var : windowVars) {
            double p = var.frequency;
            pi += 2.0 * p * (1.0 - p) * n / (n - 1);
        }
        
        // Watterson's theta
        double a1 = 0.0;
        for (int i = 1; i < n; i++) {
            a1 += 1.0 / i;
        }
        double thetaW = S / a1;
        
        // Tajima's D
        double a2 = 0.0;
        for (int i = 1; i < n; i++) {
            a2 += 1.0 / (i * i);
        }
        
        double e1 = (n + 1) / (3.0 * (n - 1) * a1) - 1.0 / (a1 * a1);
        double e2 = 2.0 * (n * n + n + 3) / (9.0 * n * (n - 1)) - 
                   (n + 2) / (n * a1) + a2 / (a1 * a1);
        
        double variance = std::sqrt(e1 * S + e2 * S * (S - 1));
        
        if (variance == 0) return 0.0;
        
        return (pi - thetaW) / variance;
    }
};

int main() {
    PopulationGenomics pg(1000000);
    
    // Add samples
    for (int i = 0; i < 100; i++) {
        PopulationGenomics::Sample sample;
        sample.id = "IND" + std::to_string(i);
        sample.sequence.resize(1000000, 'A');
        sample.population = (i < 50) ? "POP1" : "POP2";
        pg.samples.push_back(sample);
    }
    
    pg.callVariants(4);
    auto ldMatrix = pg.calculateLDMatrix(100);
    auto pcs = pg.calculatePCA(10);
    double fst = pg.calculateFst("POP1", "POP2");
    
    return 0;
}
