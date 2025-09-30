# Python Parallelization Examples

import numpy as np
import time
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
from multiprocessing import Pool
import threading

def sequential_array_processing(data):
    """Simple array processing - perfect parallelization candidate"""
    result = np.zeros_like(data)
    for i in range(len(data)):
        result[i] = data[i] ** 2 + 2 * data[i] + 1
    return result

def parallel_reduction(data):
    """Reduction with conditional - requires careful parallelization"""
    total = 0.0
    count = 0
    for x in data:
        if x > 0.5:  # Conditional adds complexity
            total += np.sqrt(x)
            count += 1
    return total / count if count > 0 else 0.0

def stencil_computation(grid):
    """2D stencil operation - neighbor-based parallelization"""
    rows, cols = grid.shape
    result = np.zeros_like(grid)
    
    for i in range(1, rows - 1):
        for j in range(1, cols - 1):
            result[i, j] = (grid[i-1, j] + grid[i+1, j] + 
                           grid[i, j-1] + grid[i, j+1] + 
                           grid[i, j]) / 5.0
    return result

def matrix_vector_multiply(matrix, vector):
    """Matrix-vector multiplication - parallelizable outer loop"""
    rows, cols = matrix.shape
    result = np.zeros(rows)
    
    for i in range(rows):
        for j in range(cols):
            result[i] += matrix[i, j] * vector[j]
    return result

def independent_computation(data_chunk):
    """Embarrassingly parallel computation"""
    result = []
    for x in data_chunk:
        # Simulate heavy computation
        value = 0.0
        for k in range(100):
            value += np.sin(x + k * 0.01) * np.cos(x - k * 0.01)
        result.append(value)
    return result

def parallel_histogram(data, bins):
    """Histogram computation with potential race conditions"""
    hist = [0] * bins
    bin_width = (max(data) - min(data)) / bins
    min_val = min(data)
    
    for value in data:
        bin_idx = int((value - min_val) / bin_width)
        if bin_idx >= bins:
            bin_idx = bins - 1
        hist[bin_idx] += 1  # Potential race condition
    
    return hist

def nested_loop_dependencies(matrix):
    """Nested loops with loop-carried dependencies"""
    rows, cols = matrix.shape
    result = np.zeros_like(matrix)
    
    for i in range(1, rows):
        for j in range(1, cols):
            result[i, j] = matrix[i, j] + result[i-1, j] + result[i, j-1]  # Dependencies!
    
    return result

def vector_operations(a, b, c):
    """Vector operations - highly vectorizable"""
    result = np.zeros_like(a)
    
    for i in range(len(a)):
        result[i] = a[i] * b[i] + c[i]  # SAXPY operation
    
    return result

def main():
    """Performance testing with timing"""
    print("Python Parallelization Analysis Demo")
    print("=" * 40)
    
    # Test data
    large_array = np.random.random(1000000)
    grid_2d = np.random.random((1000, 1000))
    matrix = np.random.random((500, 500))
    vector = np.random.random(500)
    
    # Sequential array processing
    start = time.time()
    result1 = sequential_array_processing(large_array[:10000])
    end = time.time()
    print(f"Sequential Array Processing: {(end - start)*1000:.2f} ms")
    
    # Parallel reduction
    start = time.time()
    result2 = parallel_reduction(large_array[:10000])
    end = time.time()
    print(f"Parallel Reduction: {(end - start)*1000:.2f} ms (result: {result2:.4f})")
    
    # Stencil computation
    start = time.time()
    result3 = stencil_computation(grid_2d[:100, :100])
    end = time.time()
    print(f"Stencil Computation: {(end - start)*1000:.2f} ms")
    
    # Matrix-vector multiply
    start = time.time()
    result4 = matrix_vector_multiply(matrix[:100, :100], vector[:100])
    end = time.time()
    print(f"Matrix-Vector Multiply: {(end - start)*1000:.2f} ms")
    
    # Histogram
    start = time.time()
    result5 = parallel_histogram(large_array[:10000], 50)
    end = time.time()
    print(f"Histogram Computation: {(end - start)*1000:.2f} ms")
    
    print("\nAnalysis Complete!")
    print("Upload this file to the analyzer to see parallelization opportunities!")

if __name__ == "__main__":
    main()