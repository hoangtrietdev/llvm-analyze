cmake_minimum_required(VERSION 3.16)

# LLVM Pass library
add_library(ParallelCandidatePass MODULE ParallelCandidatePass.cpp)

# Link against LLVM libraries
llvm_map_components_to_libnames(llvm_libs support core irreader analysis passes)
target_link_libraries(ParallelCandidatePass ${llvm_libs})

# Set the output name for main pass
set_target_properties(ParallelCandidatePass PROPERTIES
    OUTPUT_NAME "ParallelCandidatePass"
    PREFIX "lib"
    SUFFIX ".dylib"
)