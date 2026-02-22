// Credit Risk Modeling with Default Probability
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

class CreditRiskModel {
public:
    struct Loan {
        int id;
        double principal;
        double interestRate;
        int termMonths;
        double ltv;  // Loan-to-value
        int creditScore;
        double dti;  // Debt-to-income
        std::string purpose;
        bool defaulted;
    };
    
    struct Borrower {
        int id;
        int creditScore;
        double annualIncome;
        double totalDebt;
        int employmentYears;
        int delinquencies;
        double utilization;  // Credit utilization
    };
    
    std::vector<Loan> portfolio;
    std::vector<Borrower> borrowers;
    
    CreditRiskModel() {}
    
    // Merton model for credit risk
    struct MertonModel {
        double assetValue;
        double debt;
        double volatility;
        double riskFreeRate;
        double timeToMaturity;
        double distanceToDefault;
        double defaultProbability;
    };
    
    MertonModel computeMerton(double V, double D, double sigma,
                             double r, double T) {
        MertonModel model;
        model.assetValue = V;
        model.debt = D;
        model.volatility = sigma;
        model.riskFreeRate = r;
        model.timeToMaturity = T;
        
        // Distance to default
        double d1 = (std::log(V / D) + (r + 0.5 * sigma * sigma) * T) /
                   (sigma * std::sqrt(T));
        
        model.distanceToDefault = d1;
        
        // Default probability (simplified)
        model.defaultProbability = normalCDF(-d1);
        
        return model;
    }
    
    double normalCDF(double x) {
        return 0.5 * (1 + std::erf(x / std::sqrt(2)));
    }
    
    // Logistic regression for default prediction
    struct LogisticModel {
        std::vector<double> coefficients;
        double intercept;
    };
    
    LogisticModel trainLogistic(const std::vector<Loan>& trainLoans,
                                const std::vector<Borrower>& trainBorrowers) {
        LogisticModel model;
        int numFeatures = 6;  // Credit score, DTI, LTV, income, etc.
        model.coefficients.resize(numFeatures, 0);
        model.intercept = 0;
        
        double learningRate = 0.01;
        int epochs = 100;
        
        for (int epoch = 0; epoch < epochs; epoch++) {
            for (size_t i = 0; i < trainLoans.size(); i++) {
                // Extract features
                std::vector<double> features = extractFeatures(
                    trainLoans[i], trainBorrowers[i]
                );
                
                // Compute prediction
                double z = model.intercept;
                for (int j = 0; j < numFeatures; j++) {
                    z += model.coefficients[j] * features[j];
                }
                
                double pred = 1.0 / (1.0 + std::exp(-z));  // Sigmoid
                
                // Gradient descent update
                double error = trainLoans[i].defaulted ? 1.0 - pred : -pred;
                
                model.intercept += learningRate * error;
                for (int j = 0; j < numFeatures; j++) {
                    model.coefficients[j] += learningRate * error * features[j];
                }
            }
        }
        
        return model;
    }
    
    std::vector<double> extractFeatures(const Loan& loan,
                                       const Borrower& borrower) {
        std::vector<double> features;
        
        features.push_back(borrower.creditScore / 850.0);  // Normalize
        features.push_back(borrower.totalDebt / borrower.annualIncome);  // DTI ratio
        features.push_back(loan.ltv);
        features.push_back(std::log(borrower.annualIncome + 1) / 15);
        features.push_back(borrower.employmentYears / 40.0);
        features.push_back(borrower.utilization);
        
        return features;
    }
    
    double predictDefault(const LogisticModel& model, const Loan& loan,
                         const Borrower& borrower) {
        auto features = extractFeatures(loan, borrower);
        
        double z = model.intercept;
        for (size_t i = 0; i < features.size(); i++) {
            z += model.coefficients[i] * features[i];
        }
        
        return 1.0 / (1.0 + std::exp(-z));
    }
    
    // Credit scoring (FICO-style)
    int computeCreditScore(const Borrower& borrower) {
        double score = 300;  // Base score
        
        // Payment history (35%)
        score += (1.0 - borrower.delinquencies / 10.0) * 250;
        
        // Credit utilization (30%)
        score += (1.0 - borrower.utilization) * 210;
        
        // Length of credit history (15%)
        score += std::min(borrower.employmentYears / 20.0, 1.0) * 105;
        
        // New credit (10%)
        score += 70;  // Simplified
        
        // Credit mix (10%)
        score += 70;  // Simplified
        
        return std::min(850, std::max(300, static_cast<int>(score)));
    }
    
    // Expected Loss calculation
    struct ExpectedLoss {
        double probabilityOfDefault;
        double lossGivenDefault;
        double exposureAtDefault;
        double expectedLoss;
    };
    
    ExpectedLoss computeExpectedLoss(const Loan& loan,
                                     const Borrower& borrower,
                                     const LogisticModel& model) {
        ExpectedLoss el;
        
        // Probability of Default (PD)
        el.probabilityOfDefault = predictDefault(model, loan, borrower);
        
        // Loss Given Default (LGD) - recovery rate
        double recoveryRate = 0.4;  // 40% recovery for unsecured
        if (loan.purpose == "home") {
            recoveryRate = 0.7;  // 70% recovery for secured
        }
        el.lossGivenDefault = 1.0 - recoveryRate;
        
        // Exposure at Default (EAD)
        double monthsPaid = 0;  // Simplified
        double remainingBalance = loan.principal;
        el.exposureAtDefault = remainingBalance;
        
        // Expected Loss
        el.expectedLoss = el.probabilityOfDefault * 
                         el.lossGivenDefault * 
                         el.exposureAtDefault;
        
        return el;
    }
    
    // Credit VaR (Value at Risk)
    struct CreditVaR {
        double var95;
        double var99;
        double expectedShortfall95;
        std::vector<double> lossDistribution;
    };
    
    CreditVaR computeCreditVaR(int numSimulations) {
        CreditVaR var;
        var.lossDistribution.resize(numSimulations);
        
        std::default_random_engine generator;
        std::uniform_real_distribution<double> uniform(0.0, 1.0);
        
        // Monte Carlo simulation
        for (int sim = 0; sim < numSimulations; sim++) {
            double totalLoss = 0;
            
            for (const auto& loan : portfolio) {
                // Simulate default
                double pd = 0.05;  // Simplified - should use model
                double randomDraw = uniform(generator);
                
                if (randomDraw < pd) {
                    // Default occurred
                    double lgd = 0.6;
                    totalLoss += loan.principal * lgd;
                }
            }
            
            var.lossDistribution[sim] = totalLoss;
        }
        
        // Sort losses
        std::sort(var.lossDistribution.begin(), var.lossDistribution.end());
        
        // Compute VaR
        int var95Idx = numSimulations * 0.95;
        int var99Idx = numSimulations * 0.99;
        
        var.var95 = var.lossDistribution[var95Idx];
        var.var99 = var.lossDistribution[var99Idx];
        
        // Expected Shortfall (CVaR)
        double sum = 0;
        for (int i = var95Idx; i < numSimulations; i++) {
            sum += var.lossDistribution[i];
        }
        var.expectedShortfall95 = sum / (numSimulations - var95Idx);
        
        return var;
    }
    
    // Copula for default correlation
    struct CopulaModel {
        double correlationMatrix[10][10];
        int numBorrowers;
    };
    
    void initializeCopula(CopulaModel& copula, int n) {
        copula.numBorrowers = n;
        
        // Initialize correlation matrix
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j) {
                    copula.correlationMatrix[i][j] = 1.0;
                } else {
                    // Industry/sector correlation
                    copula.correlationMatrix[i][j] = 0.3;
                }
            }
        }
    }
    
    std::vector<bool> simulateCorrelatedDefaults(const CopulaModel& copula,
                                                 const std::vector<double>& pds) {
        std::vector<bool> defaults(copula.numBorrowers);
        
        std::default_random_engine generator;
        std::normal_distribution<double> normal(0, 1);
        
        // Generate correlated normals using Cholesky
        std::vector<double> z(copula.numBorrowers);
        for (int i = 0; i < copula.numBorrowers; i++) {
            z[i] = normal(generator);
        }
        
        // Apply correlation
        std::vector<double> correlatedZ(copula.numBorrowers);
        for (int i = 0; i < copula.numBorrowers; i++) {
            correlatedZ[i] = 0;
            for (int j = 0; j <= i; j++) {
                correlatedZ[i] += copula.correlationMatrix[i][j] * z[j];
            }
        }
        
        // Convert to uniform and compare with PD
        for (int i = 0; i < copula.numBorrowers; i++) {
            double u = normalCDF(correlatedZ[i]);
            defaults[i] = (u < pds[i]);
        }
        
        return defaults;
    }
    
    // Credit migration matrix
    struct MigrationMatrix {
        double matrix[8][8];  // AAA, AA, A, BBB, BB, B, CCC, Default
    };
    
    void initializeMigrationMatrix(MigrationMatrix& mm) {
        // Simplified one-year migration probabilities
        // Rows: From rating, Columns: To rating
        
        // AAA transitions
        mm.matrix[0][0] = 0.9081;  // Stay AAA
        mm.matrix[0][1] = 0.0833;  // AAA to AA
        mm.matrix[0][2] = 0.0068;  // AAA to A
        mm.matrix[0][7] = 0.0;     // AAA to Default
        
        // Fill in other ratings (simplified)
        for (int i = 1; i < 7; i++) {
            for (int j = 0; j < 8; j++) {
                if (i == j) {
                    mm.matrix[i][j] = 0.85;  // Stay in rating
                } else if (j == i + 1) {
                    mm.matrix[i][j] = 0.05;  // Downgrade
                } else if (j == i - 1) {
                    mm.matrix[i][j] = 0.05;  // Upgrade
                } else if (j == 7) {
                    mm.matrix[i][j] = 0.01 * (7 - i);  // Default
                } else {
                    mm.matrix[i][j] = 0.01;
                }
            }
        }
        
        // CCC transitions (higher default)
        mm.matrix[6][6] = 0.70;
        mm.matrix[6][7] = 0.20;  // High default probability
    }
    
    int simulateMigration(const MigrationMatrix& mm, int currentRating) {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> uniform(0, 1);
        
        double draw = uniform(generator);
        double cumProb = 0;
        
        for (int newRating = 0; newRating < 8; newRating++) {
            cumProb += mm.matrix[currentRating][newRating];
            if (draw < cumProb) {
                return newRating;
            }
        }
        
        return currentRating;
    }
    
    // Credit spread modeling
    double computeCreditSpread(double pd, double lgd, double riskFreeRate) {
        // Simplified credit spread
        double hazardRate = -std::log(1 - pd);
        double spread = hazardRate * lgd;
        
        return spread;
    }
    
    // Portfolio optimization with credit constraints
    struct PortfolioAllocation {
        std::vector<double> weights;
        double expectedReturn;
        double creditRisk;
        double concentration;
    };
    
    PortfolioAllocation optimizePortfolio(
        const std::vector<double>& returns,
        const std::vector<double>& creditRisks,
        double maxCreditRisk) {
        
        PortfolioAllocation allocation;
        int n = returns.size();
        allocation.weights.resize(n);
        
        // Simple greedy allocation
        std::vector<int> indices(n);
        for (int i = 0; i < n; i++) indices[i] = i;
        
        // Sort by return/risk ratio
        std::sort(indices.begin(), indices.end(),
            [&](int a, int b) {
                return returns[a] / creditRisks[a] > 
                       returns[b] / creditRisks[b];
            });
        
        double totalWeight = 0;
        allocation.expectedReturn = 0;
        allocation.creditRisk = 0;
        
        for (int idx : indices) {
            if (allocation.creditRisk + creditRisks[idx] <= maxCreditRisk &&
                totalWeight < 1.0) {
                
                double weight = std::min(0.15, 1.0 - totalWeight);  // Max 15% per position
                allocation.weights[idx] = weight;
                totalWeight += weight;
                allocation.expectedReturn += returns[idx] * weight;
                allocation.creditRisk += creditRisks[idx] * weight;
            }
        }
        
        // Normalize
        for (double& w : allocation.weights) {
            w /= totalWeight;
        }
        
        return allocation;
    }
};

int main() {
    CreditRiskModel crm;
    
    // Create sample portfolio
    for (int i = 0; i < 1000; i++) {
        CreditRiskModel::Loan loan;
        loan.id = i;
        loan.principal = 10000 + rand() % 90000;
        loan.interestRate = 0.05 + (rand() % 10) / 100.0;
        loan.termMonths = 36 + rand() % 324;  // 3-30 years
        loan.ltv = 0.5 + (rand() % 40) / 100.0;
        loan.creditScore = 600 + rand() % 250;
        loan.dti = 0.2 + (rand() % 30) / 100.0;
        loan.defaulted = (rand() % 100) < 5;  // 5% default rate
        
        crm.portfolio.push_back(loan);
        
        CreditRiskModel::Borrower borrower;
        borrower.id = i;
        borrower.creditScore = loan.creditScore;
        borrower.annualIncome = 40000 + rand() % 160000;
        borrower.totalDebt = loan.principal * 1.5;
        borrower.employmentYears = rand() % 30;
        borrower.delinquencies = rand() % 5;
        borrower.utilization = 0.1 + (rand() % 70) / 100.0;
        
        crm.borrowers.push_back(borrower);
    }
    
    // Train model
    auto logisticModel = crm.trainLogistic(crm.portfolio, crm.borrowers);
    
    // Compute credit VaR
    auto creditVaR = crm.computeCreditVaR(10000);
    
    // Optimize portfolio
    std::vector<double> returns(100), risks(100);
    for (int i = 0; i < 100; i++) {
        returns[i] = 0.05 + (rand() % 10) / 100.0;
        risks[i] = 0.01 + (rand() % 5) / 1000.0;
    }
    
    auto allocation = crm.optimizePortfolio(returns, risks, 0.10);
    
    return 0;
}
