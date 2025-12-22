// Atmospheric Radiation and Energy Balance
#include <vector>
#include <cmath>

class RadiativeTransfer {
public:
    struct Layer {
        double temperature;
        double pressure;
        double waterVapor;
        double co2;
        double ozone;
        double cloudCover;
        double cloudOpticalDepth;
    };
    
    std::vector<Layer> atmosphere;
    int nLayers;
    double surfaceTemperature;
    double surfaceAlbedo;
    
    RadiativeTransfer(int n) : nLayers(n), surfaceAlbedo(0.3) {
        atmosphere.resize(n);
    }
    
    // Planck function
    double planck(double wavelength, double T) {
        const double h = 6.626e-34;
        const double c = 3.0e8;
        const double k = 1.381e-23;
        
        double lambda = wavelength * 1e-6;  // Convert to meters
        
        double B = (2 * h * c * c) / std::pow(lambda, 5) /
                  (std::exp(h * c / (lambda * k * T)) - 1.0);
        
        return B;
    }
    
    // Two-stream radiative transfer
    struct RadiationFlux {
        std::vector<double> upward;
        std::vector<double> downward;
        std::vector<double> heating;
    };
    
    RadiationFlux solveTwoStream(const std::vector<double>& wavelengths) {
        RadiationFlux flux;
        flux.upward.resize(nLayers + 1);
        flux.downward.resize(nLayers + 1);
        flux.heating.resize(nLayers);
        
        for (double wl : wavelengths) {
            // Shortwave (solar) radiation
            if (wl < 4.0) {
                solveShortwaveFlux(wl, flux);
            }
            // Longwave (thermal) radiation
            else {
                solveLongwaveFlux(wl, flux);
            }
        }
        
        // Compute heating rates
        for (int k = 0; k < nLayers; k++) {
            double netFlux = (flux.upward[k] - flux.upward[k+1]) +
                           (flux.downward[k+1] - flux.downward[k]);
            
            double dp = (k == 0) ? atmosphere[k].pressure :
                       atmosphere[k].pressure - atmosphere[k-1].pressure;
            
            flux.heating[k] = netFlux / (dp * 100);  // K/day
        }
        
        return flux;
    }
    
    // Shortwave radiation
    void solveShortwaveFlux(double wavelength, RadiationFlux& flux) {
        double solarFlux = 1361.0;  // Solar constant
        
        // Top of atmosphere
        flux.downward[nLayers] = solarFlux;
        flux.upward[nLayers] = 0.0;
        
        // Propagate downward
        for (int k = nLayers - 1; k >= 0; k--) {
            double tau = computeOpticalDepth(k, wavelength);
            double transmission = std::exp(-tau);
            double reflection = computeReflection(k);
            
            flux.downward[k] = flux.downward[k+1] * transmission;
            flux.upward[k+1] += flux.downward[k+1] * reflection;
        }
        
        // Surface reflection
        flux.upward[0] = flux.downward[0] * surfaceAlbedo;
        
        // Propagate upward
        for (int k = 1; k <= nLayers; k++) {
            double tau = computeOpticalDepth(k-1, wavelength);
            double transmission = std::exp(-tau);
            
            flux.upward[k] += flux.upward[k-1] * transmission;
        }
    }
    
    // Longwave radiation
    void solveLongwaveFlux(double wavelength, RadiationFlux& flux) {
        // Upward from surface
        flux.upward[0] = planck(wavelength, surfaceTemperature);
        
        // Upward propagation
        for (int k = 0; k < nLayers; k++) {
            double tau = computeOpticalDepth(k, wavelength);
            double transmission = std::exp(-tau);
            double emission = planck(wavelength, atmosphere[k].temperature);
            
            flux.upward[k+1] = flux.upward[k] * transmission + 
                              emission * (1 - transmission);
        }
        
        // Downward from TOA
        flux.downward[nLayers] = 0.0;
        
        // Downward propagation
        for (int k = nLayers - 1; k >= 0; k--) {
            double tau = computeOpticalDepth(k, wavelength);
            double transmission = std::exp(-tau);
            double emission = planck(wavelength, atmosphere[k].temperature);
            
            flux.downward[k] = flux.downward[k+1] * transmission + 
                              emission * (1 - transmission);
        }
    }
    
    // Optical depth calculation
    double computeOpticalDepth(int layer, double wavelength) {
        double tau = 0.0;
        
        // Water vapor absorption
        if (wavelength > 5.0 && wavelength < 8.0) {
            tau += atmosphere[layer].waterVapor * 0.1;
        }
        
        // CO2 absorption
        if (wavelength > 13.0 && wavelength < 17.0) {
            tau += atmosphere[layer].co2 * 0.05;
        }
        
        // Ozone absorption
        if (wavelength < 0.3) {
            tau += atmosphere[layer].ozone * 0.2;
        }
        
        // Cloud optical depth
        tau += atmosphere[layer].cloudCover * atmosphere[layer].cloudOpticalDepth;
        
        return tau;
    }
    
    // Cloud reflection
    double computeReflection(int layer) {
        return atmosphere[layer].cloudCover * 0.5;
    }
    
    // Compute net radiation balance
    double computeRadiativeBalance() {
        std::vector<double> wavelengths;
        for (double wl = 0.2; wl < 50.0; wl += 0.5) {
            wavelengths.push_back(wl);
        }
        
        auto flux = solveTwoStream(wavelengths);
        
        // Net at TOA
        double netTOA = flux.upward[nLayers] - flux.downward[nLayers];
        
        return netTOA;
    }
    
    // Adjust temperatures to achieve equilibrium
    void radiativeConvectiveEquilibrium(int maxIter) {
        for (int iter = 0; iter < maxIter; iter++) {
            std::vector<double> wavelengths;
            for (double wl = 0.2; wl < 50.0; wl += 1.0) {
                wavelengths.push_back(wl);
            }
            
            auto flux = solveTwoStream(wavelengths);
            
            // Update temperatures based on heating rates
            for (int k = 0; k < nLayers; k++) {
                atmosphere[k].temperature += 0.01 * flux.heating[k];
            }
            
            // Convective adjustment
            applyConvectiveAdjustment();
        }
    }
    
    // Moist convective adjustment
    void applyConvectiveAdjustment() {
        double lapseRate = 6.5;  // K/km
        
        for (int k = 0; k < nLayers - 1; k++) {
            double dT = atmosphere[k+1].temperature - atmosphere[k].temperature;
            double dz = 1.0;  // km (approximate)
            
            double actualLapse = -dT / dz;
            
            if (actualLapse > lapseRate) {
                // Adjust to critical lapse rate
                double adjustment = (actualLapse - lapseRate) * dz / 2;
                atmosphere[k].temperature += adjustment;
                atmosphere[k+1].temperature -= adjustment;
            }
        }
    }
};

int main() {
    RadiativeTransfer rt(50);
    
    // Initialize atmosphere
    for (int k = 0; k < 50; k++) {
        rt.atmosphere[k].temperature = 288 - 6.5 * k;
        rt.atmosphere[k].pressure = 1000 * std::exp(-k / 7.0);
        rt.atmosphere[k].waterVapor = 10 * std::exp(-k / 2.0);
        rt.atmosphere[k].co2 = 400;
        rt.atmosphere[k].ozone = (k > 15 && k < 30) ? 3.0 : 0.1;
        rt.atmosphere[k].cloudCover = (k > 5 && k < 10) ? 0.5 : 0.0;
        rt.atmosphere[k].cloudOpticalDepth = 10.0;
    }
    
    rt.surfaceTemperature = 288;
    rt.radiativeConvectiveEquilibrium(100);
    
    return 0;
}
