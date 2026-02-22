// Dense Optical Flow - Variational method
#include <vector>
#include <cmath>

void variationalOpticalFlow(double* I1, double* I2, double* u, double* v,
                           int width, int height, int iterations, double alpha, double omega) {
    std::vector<double> u_new(width*height), v_new(width*height);
    
    for (int iter = 0; iter < iterations; iter++) {
        for (int y = 1; y < height-1; y++) {
            for (int x = 1; x < width-1; x++) {
                int idx = y*width + x;
                
                // Image gradients
                double Ix = (I1[idx+1] - I1[idx-1]) / 2.0;
                double Iy = (I1[idx+width] - I1[idx-width]) / 2.0;
                double It = I2[idx] - I1[idx];
                
                // Average flow in neighborhood
                double u_avg = (u[idx-1] + u[idx+1] + u[idx-width] + u[idx+width]) / 4.0;
                double v_avg = (v[idx-1] + v[idx+1] + v[idx-width] + v[idx+width]) / 4.0;
                
                // Data term
                double P = Ix*u_avg + Iy*v_avg + It;
                double D = alpha*alpha + Ix*Ix + Iy*Iy;
                
                // Update with relaxation
                u_new[idx] = u_avg - Ix * P / D;
                v_new[idx] = v_avg - Iy * P / D;
                
                // Apply relaxation
                u_new[idx] = (1-omega) * u[idx] + omega * u_new[idx];
                v_new[idx] = (1-omega) * v[idx] + omega * v_new[idx];
            }
        }
        
        // Copy data back from vectors to arrays
        for (int i = 0; i < width*height; i++) {
            u[i] = u_new[i];
            v[i] = v_new[i];
        }
    }
}

void computeFlowDivergence(double* u, double* v, double* divergence, int width, int height) {
    for (int y = 1; y < height-1; y++) {
        for (int x = 1; x < width-1; x++) {
            int idx = y*width + x;
            
            double du_dx = (u[idx+1] - u[idx-1]) / 2.0;
            double dv_dy = (v[idx+width] - v[idx-width]) / 2.0;
            
            divergence[idx] = du_dx + dv_dy;
        }
    }
}

int main() {
    const int width = 640, height = 480;
    std::vector<double> I1(width*height, 100.0);
    std::vector<double> I2(width*height, 105.0);
    std::vector<double> u(width*height, 0.0);
    std::vector<double> v(width*height, 0.0);
    std::vector<double> divergence(width*height);
    
    variationalOpticalFlow(I1.data(), I2.data(), u.data(), v.data(),
                          width, height, 100, 0.1, 1.8);
    computeFlowDivergence(u.data(), v.data(), divergence.data(), width, height);
    
    return 0;
}
