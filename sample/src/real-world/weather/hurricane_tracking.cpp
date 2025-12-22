// Hurricane Tracking and Prediction
#include <vector>
#include <cmath>
#include <algorithm>

class HurricaneTracker {
public:
    struct Position {
        double lat, lon;  // Latitude, longitude in degrees
        double time;      // Time in hours
    };
    
    struct HurricaneState {
        Position pos;
        double windSpeed;      // Maximum sustained wind speed (m/s)
        double pressure;       // Central pressure (hPa)
        double radius;         // Radius of maximum wind (km)
        double direction;      // Direction of motion (degrees)
        double translationSpeed; // Forward speed (m/s)
    };
    
    struct AtmosphericData {
        double sst;           // Sea surface temperature (C)
        double shear;         // Wind shear (m/s)
        double humidity;      // Relative humidity (%)
        double vorticity;     // Vorticity (1/s)
        double divergence;    // Divergence (1/s)
    };
    
    // Wind-pressure relationship (Dvorak technique)
    double estimateWindSpeed(double pressure) {
        // Empirical formula
        return 6.3 * std::sqrt(1013 - pressure);
    }
    
    double estimatePressure(double windSpeed) {
        return 1013 - (windSpeed / 6.3) * (windSpeed / 6.3);
    }
    
    // Saffir-Simpson Hurricane Scale
    int getCategory(double windSpeed) {
        double windKmh = windSpeed * 3.6;
        
        if (windKmh < 119) return 0;
        else if (windKmh < 154) return 1;
        else if (windKmh < 178) return 2;
        else if (windKmh < 209) return 3;
        else if (windKmh < 252) return 4;
        else return 5;
    }
    
    // Maximum Potential Intensity (MPI)
    double computeMPI(double sst, double outflowTemp) {
        double Ts = sst + 273.15;  // Convert to Kelvin
        double To = outflowTemp + 273.15;
        
        double Ck = 0.0015;  // Exchange coefficient for enthalpy
        double Cd = 0.001;   // Drag coefficient
        
        // Emanuel's MPI formula
        double mpi = std::sqrt((Ck / Cd) * (Ts - To) / To * 
                              (3000 + 2.5e6));  // Simplified
        
        return mpi;
    }
    
    // Track prediction using persistence model
    std::vector<Position> persistenceModel(const std::vector<Position>& history,
                                          int forecastHours) {
        if (history.size() < 2) return {};
        
        std::vector<Position> forecast;
        
        // Compute average direction and speed
        double avgDlat = 0, avgDlon = 0;
        for (size_t i = 1; i < history.size(); i++) {
            double dt = history[i].time - history[i-1].time;
            avgDlat += (history[i].lat - history[i-1].lat) / dt;
            avgDlon += (history[i].lon - history[i-1].lon) / dt;
        }
        
        avgDlat /= (history.size() - 1);
        avgDlon /= (history.size() - 1);
        
        // Generate forecast
        Position current = history.back();
        
        for (int h = 6; h <= forecastHours; h += 6) {
            Position next;
            next.time = current.time + h;
            next.lat = current.lat + avgDlat * h;
            next.lon = current.lon + avgDlon * h;
            forecast.push_back(next);
        }
        
        return forecast;
    }
    
    // Track prediction using climatology and persistence (CLIPER)
    std::vector<Position> cliperModel(const std::vector<Position>& history,
                                     const HurricaneState& state,
                                     int forecastHours) {
        
        std::vector<Position> forecast;
        
        // Climatological steering flow
        double baseDirection = 270;  // Westerly (degrees)
        double baseSpeed = 5.0;      // m/s
        
        // Adjust for latitude
        double lat = state.pos.lat;
        if (lat > 25) {
            baseDirection = 45;  // Northeast
            baseSpeed = 8.0;
        }
        
        // Persistence component
        double persistDir = state.direction;
        double persistSpeed = state.translationSpeed;
        
        // Blend climatology and persistence
        double weight = 0.7;  // Weight for persistence
        double finalDir = weight * persistDir + (1 - weight) * baseDirection;
        double finalSpeed = weight * persistSpeed + (1 - weight) * baseSpeed;
        
        // Generate forecast
        Position current = state.pos;
        
        for (int h = 6; h <= forecastHours; h += 6) {
            double dt = h * 3600;  // Convert to seconds
            
            // Convert to Cartesian
            double dx = finalSpeed * std::cos(finalDir * M_PI / 180) * dt;
            double dy = finalSpeed * std::sin(finalDir * M_PI / 180) * dt;
            
            // Convert back to lat/lon (simplified)
            double dlat = dy / 111000;  // 1 degree lat = ~111 km
            double dlon = dx / (111000 * std::cos(current.lat * M_PI / 180));
            
            Position next;
            next.time = current.time + h;
            next.lat = current.lat + dlat;
            next.lon = current.lon + dlon;
            
            forecast.push_back(next);
        }
        
        return forecast;
    }
    
    // Intensity prediction using Statistical Hurricane Intensity Prediction Scheme
    std::vector<double> shipsModel(const HurricaneState& state,
                                  const std::vector<AtmosphericData>& envData,
                                  int forecastHours) {
        
        std::vector<double> intensityForecast;
        
        double currentIntensity = state.windSpeed;
        
        for (int h = 6; h <= forecastHours; h += 6) {
            int idx = std::min((size_t)(h / 6), envData.size() - 1);
            const AtmosphericData& env = envData[idx];
            
            // Favorable factors
            double sstContrib = (env.sst - 26.5) * 2.0;  // SST above threshold
            double humidityContrib = (env.humidity - 50) * 0.1;
            double vorticityContrib = env.vorticity * 1000;
            
            // Unfavorable factors
            double shearPenalty = -env.shear * 1.5;
            double divergencePenalty = -env.divergence * 500;
            
            // Total change
            double change = sstContrib + humidityContrib + vorticityContrib +
                          shearPenalty + divergencePenalty;
            
            // Apply damping for very intense storms
            if (currentIntensity > 50) {
                change *= 0.5;
            }
            
            currentIntensity += change;
            
            // Physical limits
            currentIntensity = std::max(10.0, std::min(85.0, currentIntensity));
            
            intensityForecast.push_back(currentIntensity);
        }
        
        return intensityForecast;
    }
    
    // Numerical weather prediction - simple barotropic model
    std::vector<Position> barotropicModel(const HurricaneState& state,
                                         const std::vector<std::vector<double>>& windFieldU,
                                         const std::vector<std::vector<double>>& windFieldV,
                                         int forecastHours) {
        
        std::vector<Position> forecast;
        
        int nx = windFieldU.size();
        int ny = windFieldU[0].size();
        
        double lat = state.pos.lat;
        double lon = state.pos.lon;
        
        for (int h = 1; h <= forecastHours; h++) {
            // Convert position to grid indices
            int i = (int)((lat + 90) / 180 * nx);
            int j = (int)((lon + 180) / 360 * ny);
            
            i = std::max(0, std::min(nx - 1, i));
            j = std::max(0, std::min(ny - 1, j));
            
            // Interpolate wind at current position
            double u = windFieldU[i][j];
            double v = windFieldV[i][j];
            
            // Beta effect (Coriolis parameter variation)
            double beta = 2e-11;  // 1/(m*s)
            double f = 2 * 7.29e-5 * std::sin(lat * M_PI / 180);
            
            // Beta drift
            double uBeta = -beta * state.radius * state.radius / f;
            
            u += uBeta;
            
            // Update position
            double dt = 3600;  // 1 hour timestep
            double dlat = v * dt / 111000;
            double dlon = u * dt / (111000 * std::cos(lat * M_PI / 180));
            
            lat += dlat;
            lon += dlon;
            
            if (h % 6 == 0) {
                Position next;
                next.time = state.pos.time + h;
                next.lat = lat;
                next.lon = lon;
                forecast.push_back(next);
            }
        }
        
        return forecast;
    }
    
    // Ensemble prediction
    struct EnsembleForecast {
        std::vector<std::vector<Position>> tracks;
        std::vector<double> weights;
    };
    
    EnsembleForecast ensembleForecast(const HurricaneState& state,
                                     const std::vector<Position>& history,
                                     const std::vector<AtmosphericData>& envData,
                                     int forecastHours) {
        
        EnsembleForecast ensemble;
        
        // Model 1: Persistence
        auto track1 = persistenceModel(history, forecastHours);
        ensemble.tracks.push_back(track1);
        ensemble.weights.push_back(0.2);
        
        // Model 2: CLIPER
        auto track2 = cliperModel(history, state, forecastHours);
        ensemble.tracks.push_back(track2);
        ensemble.weights.push_back(0.3);
        
        // Model 3-7: Perturbed initial conditions
        for (int i = 0; i < 5; i++) {
            HurricaneState perturbed = state;
            perturbed.pos.lat += (i - 2) * 0.5;
            perturbed.pos.lon += (i - 2) * 0.5;
            
            auto track = cliperModel(history, perturbed, forecastHours);
            ensemble.tracks.push_back(track);
            ensemble.weights.push_back(0.1);
        }
        
        return ensemble;
    }
    
    // Compute consensus forecast
    std::vector<Position> consensusForecast(const EnsembleForecast& ensemble) {
        if (ensemble.tracks.empty()) return {};
        
        size_t numTimeSteps = ensemble.tracks[0].size();
        std::vector<Position> consensus(numTimeSteps);
        
        for (size_t t = 0; t < numTimeSteps; t++) {
            double lat = 0, lon = 0;
            double sumWeights = 0;
            
            for (size_t m = 0; m < ensemble.tracks.size(); m++) {
                if (t < ensemble.tracks[m].size()) {
                    lat += ensemble.tracks[m][t].lat * ensemble.weights[m];
                    lon += ensemble.tracks[m][t].lon * ensemble.weights[m];
                    sumWeights += ensemble.weights[m];
                }
            }
            
            consensus[t].lat = lat / sumWeights;
            consensus[t].lon = lon / sumWeights;
            consensus[t].time = ensemble.tracks[0][t].time;
        }
        
        return consensus;
    }
    
    // Compute track error
    double computeTrackError(const Position& forecast, const Position& actual) {
        // Great circle distance
        double lat1 = forecast.lat * M_PI / 180;
        double lon1 = forecast.lon * M_PI / 180;
        double lat2 = actual.lat * M_PI / 180;
        double lon2 = actual.lon * M_PI / 180;
        
        double dlat = lat2 - lat1;
        double dlon = lon2 - lon1;
        
        double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
                  std::cos(lat1) * std::cos(lat2) *
                  std::sin(dlon / 2) * std::sin(dlon / 2);
        
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
        
        double R = 6371;  // Earth radius in km
        return R * c;
    }
    
    // Rapid intensification probability
    double rapidIntensificationProb(const HurricaneState& state,
                                   const AtmosphericData& env) {
        // Logistic regression model
        double logit = -5.0;
        
        // Favorable conditions
        logit += 0.2 * (env.sst - 26);           // Warm SST
        logit += 0.01 * env.humidity;            // High humidity
        logit -= 0.3 * env.shear;                // Low shear
        logit += 100 * env.vorticity;            // Strong vorticity
        logit += 0.01 * (1013 - state.pressure); // Low pressure
        
        // Current intensity factor
        if (state.windSpeed < 30) {
            logit += 1.0;  // Easier to intensify from weak state
        }
        
        return 1.0 / (1.0 + std::exp(-logit));
    }
    
    // Storm surge estimation
    double estimateStormSurge(const HurricaneState& state,
                             double bathymetry,
                             double shoreAngle) {
        
        // Simplified surge model
        double windStress = 1.2e-3 * state.windSpeed * state.windSpeed;
        
        // Pressure contribution
        double pressureSurge = 0.01 * (1013 - state.pressure);
        
        // Wind setup
        double fetch = 50000;  // 50 km
        double windSetup = windStress * fetch / (9.81 * bathymetry);
        
        // Angle factor
        double angleFactor = std::sin(shoreAngle * M_PI / 180);
        
        double totalSurge = (pressureSurge + windSetup) * angleFactor;
        
        return totalSurge;
    }
};

int main() {
    HurricaneTracker tracker;
    
    // Historical track data
    std::vector<HurricaneTracker::Position> history = {
        {15.5, -45.0, 0},
        {15.8, -46.2, 6},
        {16.2, -47.5, 12},
        {16.8, -48.9, 18}
    };
    
    // Current state
    HurricaneTracker::HurricaneState state;
    state.pos = {17.5, -50.5, 24};
    state.windSpeed = 45.0;  // m/s (~100 mph)
    state.pressure = 970;
    state.radius = 30;
    state.direction = 280;
    state.translationSpeed = 6.0;
    
    // Environmental data
    std::vector<HurricaneTracker::AtmosphericData> envData;
    for (int i = 0; i < 20; i++) {
        HurricaneTracker::AtmosphericData env;
        env.sst = 28.5;
        env.shear = 5.0;
        env.humidity = 70;
        env.vorticity = 1e-4;
        env.divergence = -1e-5;
        envData.push_back(env);
    }
    
    // Track forecasts
    auto persistence = tracker.persistenceModel(history, 120);
    auto cliper = tracker.cliperModel(history, state, 120);
    
    // Intensity forecast
    auto intensity = tracker.shipsModel(state, envData, 120);
    
    // Ensemble forecast
    auto ensemble = tracker.ensembleForecast(state, history, envData, 120);
    auto consensus = tracker.consensusForecast(ensemble);
    
    // Rapid intensification probability
    double riProb = tracker.rapidIntensificationProb(state, envData[0]);
    
    // Storm surge
    double surge = tracker.estimateStormSurge(state, 10.0, 90);
    
    // Category
    int category = tracker.getCategory(state.windSpeed);
    
    return 0;
}
