#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

// Define constants for the data dimensions
const int NUM_RECORDS = 10000;  // The "10000 records"
const int NUM_FIELDS  = 10;     // Number of data fields/columns per record

// A simple structure to represent a single data record
struct DataRecord {
    // We'll use a fixed-size array for simplicity and speed here,
    // simulating 10 numeric fields (e.g., price, quantity, ID, etc.)
    double fields[NUM_FIELDS];
};

// Function to simulate reading/loading the data
std::vector<DataRecord> loadData() {
    std::cout << "-> Initializing " << NUM_RECORDS << " records with " 
              << NUM_FIELDS << " fields each..." << std::endl;
    
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(0)));

    std::vector<DataRecord> data;
    data.reserve(NUM_RECORDS); // Pre-allocate memory for efficiency

    // Populate the multi-dimensional array
    for (int i = 0; i < NUM_RECORDS; ++i) {
        DataRecord record;
        for (int j = 0; j < NUM_FIELDS; ++j) {
            // Generate a random double value between 0.0 and 100.0
            record.fields[j] = static_cast<double>(std::rand() % 10000) / 100.0;
        }
        data.push_back(record);
    }
    
    std::cout << "-> Data loading complete. Total elements: " 
              << data.size() * NUM_FIELDS << std::endl;
    return data;
}

// Function containing the "business logic"
void processData(const std::vector<DataRecord>& data) {
    if (data.empty()) {
        std::cout << "No data to process." << std::endl;
        return;
    }

    std::cout << "-> Starting business logic processing (Calculating total of Field 0)..." << std::endl;

    // --- Start Business Logic ---

    // We'll calculate the sum of the first field (Field 0) across all records
    double grandTotal = 0.0;
    
    // Loop through all 10,000 records
    for (const auto& record : data) {
        // Business Rule: Only include records where Field 1 is greater than 50.0
        if (record.fields[1] > 50.0) {
            // "Business Calculation": Add the value of Field 0 to the total
            grandTotal += record.fields[0];
        }
    }

    // --- End Business Logic ---

    std::cout << "-> Business logic complete." << std::endl;
    std::cout << "   Total number of records processed: " << data.size() << std::endl;
    std::cout << "   Calculated Grand Total (Field 0 where Field 1 > 50.0): " << grandTotal << std::endl;
}

int main() {
    // 1. Start timer for performance measurement
    auto start = std::chrono::high_resolution_clock::now();

    // 2. Load the data (Simulates reading from a file/database into a multi-dimensional structure)
    std::vector<DataRecord> businessData = loadData();

    // 3. Process the data (Executes the core business logic)
    processData(businessData);

    // 4. Stop timer and display elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "\n==========================================" << std::endl;
    std::cout << "Processing Time: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}