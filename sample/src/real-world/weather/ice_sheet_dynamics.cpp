// Ice Sheet Dynamics - Glacial flow simulation
#include <vector>
#include <cmath>

void simulateIceFlow(double* thickness, double* velocity_x, double* velocity_y,
                    double* bedrock, int nx, int ny, int years) {
    const double ice_density = 917.0;
    const double gravity = 9.81;
    const double glen_coefficient = 2.4e-24;
    
    for (int year = 0; year < years; year++) {
        for (int i = 1; i < nx-1; i++) {
            for (int j = 1; j < ny-1; j++) {
                int idx = i * ny + j;
                
                // Surface elevation
                double surface = bedrock[idx] + thickness[idx];
                
                // Surface gradient
                double grad_x = (bedrock[(i+1)*ny+j] + thickness[(i+1)*ny+j] - 
                               bedrock[(i-1)*ny+j] - thickness[(i-1)*ny+j]) / 2000.0;
                double grad_y = (bedrock[i*ny+(j+1)] + thickness[i*ny+(j+1)] - 
                               bedrock[i*ny+(j-1)] - thickness[i*ny+(j-1)]) / 2000.0;
                
                // Driving stress
                double tau_x = ice_density * gravity * thickness[idx] * grad_x;
                double tau_y = ice_density * gravity * thickness[idx] * grad_y;
                
                // Glen's flow law
                double tau = sqrt(tau_x*tau_x + tau_y*tau_y);
                double deformation = glen_coefficient * pow(tau, 3);
                
                velocity_x[idx] = -deformation * tau_x;
                velocity_y[idx] = -deformation * tau_y;
                
                // Mass balance
                double accumulation = 0.3; // m/year
                double ablation = std::max(0.0, 0.01 * (surface - 2000.0));
                
                thickness[idx] += accumulation - ablation;
            }
        }
    }
}

int main() {
    const int nx = 200, ny = 200, years = 100;
    std::vector<double> thickness(nx * ny, 500.0);
    std::vector<double> velocity_x(nx * ny);
    std::vector<double> velocity_y(nx * ny);
    std::vector<double> bedrock(nx * ny, 1000.0);
    
    simulateIceFlow(thickness.data(), velocity_x.data(), velocity_y.data(),
                   bedrock.data(), nx, ny, years);
    return 0;
}
