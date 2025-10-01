#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

// Function to normalize data into range [0,1]
vector<vector<double>> normalizeTo2D(const vector<vector<vector<vector<int>>>> &data4D) {
    int D1 = data4D.size();
    int D2 = data4D[0].size();
    int D3 = data4D[0][0].size();
    int D4 = data4D[0][0][0].size();

    // Flatten into 2D (D1*D2 rows, D3*D4 cols)
    int rows = D1 * D2;
    int cols = D3 * D4;

    vector<vector<double>> result(rows, vector<double>(cols, 0.0));

    // Find global min and max for normalization
    int globalMin = data4D[0][0][0][0];
    int globalMax = data4D[0][0][0][0];

    for (int i = 0; i < D1; i++) {
        for (int j = 0; j < D2; j++) {
            for (int k = 0; k < D3; k++) {
                for (int l = 0; l < D4; l++) {
                    globalMin = min(globalMin, data4D[i][j][k][l]);
                    globalMax = max(globalMax, data4D[i][j][k][l]);
                }
            }
        }
    }

    double range = (globalMax == globalMin) ? 1.0 : (globalMax - globalMin);

    // Fill normalized 2D array
    for (int i = 0; i < D1; i++) {
        for (int j = 0; j < D2; j++) {
            int rowIndex = i * D2 + j;
            for (int k = 0; k < D3; k++) {
                for (int l = 0; l < D4; l++) {
                    int colIndex = k * D4 + l;
                    result[rowIndex][colIndex] = 
                        (data4D[i][j][k][l] - globalMin) / range;
                }
            }
        }
    }

    return result;
}

int main() {
    srand((unsigned)time(0));

    // Example: 4D array of size [2][3][4][5]
    int D1 = 2, D2 = 3, D3 = 4, D4 = 5;
    vector<vector<vector<vector<int>>>> data4D(
        D1, vector<vector<vector<int>>>(D2, 
            vector<vector<int>>(D3, vector<int>(D4)))
    );

    // Fill with random integers [0..100]
    for (int i = 0; i < D1; i++) {
        for (int j = 0; j < D2; j++) {
            for (int k = 0; k < D3; k++) {
                for (int l = 0; l < D4; l++) {
                    data4D[i][j][k][l] = rand() % 101;
                }
            }
        }
    }

    // Normalize and flatten
    vector<vector<double>> normalized2D = normalizeTo2D(data4D);

    // Print normalized 2D matrix
    cout << "Normalized 2D Matrix (" << normalized2D.size() 
         << "x" << normalized2D[0].size() << "):\n";

    for (const auto &row : normalized2D) {
        for (double val : row) {
            cout << fixed << setprecision(2) << val << " ";
        }
        cout << "\n";
    }

    return 0;
}
