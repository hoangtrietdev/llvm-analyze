// Sparse matrix operations
#include <vector>
#include <map>

const int MATRIX_SIZE = 100000;

class SparseMatrix {
private:
    std::map<std::pair<int,int>, double> data;
    int rows, cols;
    
public:
    SparseMatrix(int r, int c) : rows(r), cols(c) {}
    
    void set(int i, int j, double val) {
        if (val != 0.0) data[{i, j}] = val;
    }
    
    double get(int i, int j) const {
        auto it = data.find({i, j});
        return (it != data.end()) ? it->second : 0.0;
    }
    
    std::vector<double> multiply(const std::vector<double>& vec) {
        std::vector<double> result(rows, 0.0);
        
        for (const auto& entry : data) {
            int i = entry.first.first;
            int j = entry.first.second;
            result[i] += entry.second * vec[j];
        }
        
        return result;
    }
};

int main() {
    SparseMatrix mat(MATRIX_SIZE, MATRIX_SIZE);
    std::vector<double> vec(MATRIX_SIZE, 1.0);
    
    for (int i = 0; i < MATRIX_SIZE; i++) {
        mat.set(i, i, 2.0);
        if (i > 0) mat.set(i, i-1, -1.0);
        if (i < MATRIX_SIZE-1) mat.set(i, i+1, -1.0);
    }
    
    std::vector<double> result = mat.multiply(vec);
    
    return 0;
}
