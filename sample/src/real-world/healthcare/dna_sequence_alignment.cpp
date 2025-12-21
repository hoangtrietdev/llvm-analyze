// DNA sequence alignment using dynamic programming
#include <vector>
#include <string>
#include <algorithm>

const int MAX_SEQUENCE_LENGTH = 10000;

class DNASequenceAligner {
private:
    std::vector<std::vector<int>> score_matrix;
    std::vector<std::vector<char>> traceback_matrix;
    
public:
    int align_sequences(const std::string& seq1, const std::string& seq2) {
        int m = seq1.length();
        int n = seq2.length();
        
        score_matrix.resize(m + 1, std::vector<int>(n + 1, 0));
        traceback_matrix.resize(m + 1, std::vector<char>(n + 1, 'X'));
        
        // Initialize first row and column
        for (int i = 0; i <= m; i++) {
            score_matrix[i][0] = i * -2;
            traceback_matrix[i][0] = 'U';
        }
        for (int j = 0; j <= n; j++) {
            score_matrix[0][j] = j * -2;
            traceback_matrix[0][j] = 'L';
        }
        
        // Fill the matrix
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                int match = score_matrix[i-1][j-1] + (seq1[i-1] == seq2[j-1] ? 1 : -1);
                int delete_gap = score_matrix[i-1][j] - 2;
                int insert_gap = score_matrix[i][j-1] - 2;
                
                score_matrix[i][j] = std::max({match, delete_gap, insert_gap});
                
                if (score_matrix[i][j] == match) traceback_matrix[i][j] = 'D';
                else if (score_matrix[i][j] == delete_gap) traceback_matrix[i][j] = 'U';
                else traceback_matrix[i][j] = 'L';
            }
        }
        
        return score_matrix[m][n];
    }
    
    void align_multiple_sequences(std::vector<std::string>& sequences) {
        int num_seq = sequences.size();
        std::vector<std::vector<int>> pairwise_scores(num_seq, std::vector<int>(num_seq, 0));
        
        // Compute all pairwise alignments
        for (int i = 0; i < num_seq; i++) {
            for (int j = i + 1; j < num_seq; j++) {
                pairwise_scores[i][j] = align_sequences(sequences[i], sequences[j]);
                pairwise_scores[j][i] = pairwise_scores[i][j];
            }
        }
    }
};

void find_motifs(const std::string& sequence, int motif_length) {
    std::vector<int> motif_counts(1 << (2 * motif_length), 0);
    
    for (size_t i = 0; i <= sequence.length() - motif_length; i++) {
        int hash = 0;
        for (int j = 0; j < motif_length; j++) {
            char c = sequence[i + j];
            int code = (c == 'A') ? 0 : (c == 'C') ? 1 : (c == 'G') ? 2 : 3;
            hash = (hash << 2) | code;
        }
        motif_counts[hash]++;
    }
}

int main() {
    DNASequenceAligner aligner;
    
    std::string seq1 = "ACGTACGTACGTACGT";
    std::string seq2 = "ACGTACGTACGTACGT";
    
    for (int i = 0; i < 1000; i++) {
        seq1 += "ACGT";
        seq2 += "ACGT";
    }
    
    int score = aligner.align_sequences(seq1, seq2);
    find_motifs(seq1, 8);
    
    return 0;
}
