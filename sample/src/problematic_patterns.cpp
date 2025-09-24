// Problematic Patterns - Difficult or Unsafe to Parallelize
// These examples show patterns that require special handling

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>  // for printf

// Forward declaration for helper function
int expensive_function(int x);

void function_calls_with_side_effects() {
    // ❌ NOT SAFE - Function calls with side effects
    const int n = 1000;
    int array[n], result[n];
    
    for (int i = 0; i < n; i++) {
        result[i] = expensive_function(array[i]);  // Unknown side effects
    }
    
    // Problem: Function might have side effects, global state, I/O
}

void prefix_sum_sequential_dependency() {
    // ❌ NOT EASILY PARALLELIZABLE - Sequential dependency
    const int n = 1000;
    int array[n];
    
    for (int i = 1; i < n; i++) {
        array[i] += array[i-1];  // Each iteration depends on previous
    }
    
    // Problem: True data dependency - needs parallel scan algorithms
    // Suggested: Use specialized parallel scan algorithms
}

void indirect_memory_access() {
    // ⚠️ DIFFICULT - Indirect access patterns
    const int n = 1000;
    int array[n], index[n], result[n];
    
    for (int i = 0; i < n; i++) {
        result[i] = array[index[i]];  // Random memory access
    }
    
    // Problem: Cache performance issues, potential memory conflicts
    // Suggested: May parallelize but with poor performance
}

void loop_with_io() {
    // ❌ NOT SAFE - I/O operations
    const int n = 100;
    int data[n];
    
    for (int i = 0; i < n; i++) {
        printf("Processing item %d: %d\n", i, data[i]);  // I/O side effect
    }
    
    // Problem: I/O operations are not thread-safe by default
}

void filter_with_output_dependency() {
    // ⚠️ COMPLEX - Filter pattern with output dependency
    const int n = 1000;
    int input[n], output[n];
    int count = 0;
    
    for (int i = 0; i < n; i++) {
        if (input[i] > 100) {
            output[count++] = input[i];  // Output index depends on previous iterations
        }
    }
    
    // Problem: Output index depends on how many elements passed filter
    // Suggested: Use parallel scan or gather operations
}

void random_updates() {
    // ❌ NOT SAFE - Race conditions
    const int n = 1000;
    int array[n], updates[n], indices[n];
    
    for (int i = 0; i < n; i++) {
        array[indices[i]] += updates[i];  // Potential race condition
    }
    
    // Problem: Multiple threads might update same array element
}

void cross_iteration_dependency() {
    // ❌ NOT PARALLELIZABLE - Cross-iteration dependency
    const int n = 1000;
    int array[n];
    
    for (int i = 2; i < n; i++) {
        array[i] = array[i-1] + array[i-2];  // Fibonacci-like dependency
    }
    
    // Problem: Each iteration depends on multiple previous iterations
}

void complex_control_flow() {
    // ⚠️ COMPLEX - Complex control flow
    const int n = 1000;
    int array[n], result[n];
    
    for (int i = 0; i < n; i++) {
        if (array[i] > 0) {
            if (array[i] % 2 == 0) {
                result[i] = array[i] / 2;
            } else {
                result[i] = array[i] * 3 + 1;
                if (result[i] > 1000) {
                    result[i] = 1000;  // Clamp value
                }
            }
        } else {
            result[i] = 0;
        }
    }
    
    // May be parallelizable but complex control flow affects performance
}

// Helper function for demonstration
int expensive_function(int x) {
    // Simulated expensive computation
    static int global_counter = 0;
    global_counter++;  // Side effect!
    return x * x + global_counter;
}