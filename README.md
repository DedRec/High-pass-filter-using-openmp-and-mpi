# High-pass-filter-using-openmp-and-mpi

## Description:
This project implements various high-pass filters using different parallelization techniques, including OpenMP and MPI. It applies high-pass filtering to input images using different kernel types and parallelization strategies.

## Contents:
1. **sequential_dynamicKernel.cpp**: Sequential implementation of high-pass filtering with a dynamically generated kernel.
2. **sequential_staticKernel.cpp**: Sequential implementation of high-pass filtering with a statically defined kernel.
3. **openmp_dynamicKernel.cpp**: OpenMP parallel implementation of high-pass filtering with a dynamically generated kernel.
4. **openmp_staticKernel.cpp**: OpenMP parallel implementation of high-pass filtering with a statically defined kernel.
5. **mpi_staticKernel.cpp**: MPI parallel implementation of high-pass filtering with a statically defined kernel.
6. **mpi_dynamicKernel.cpp**: MPI parallel implementation of high-pass filtering with a dynamically generated kernel.
7. **Samples**: Sample input images used for testing the filtering algorithms.

## Usage:
1. Compile each source code file using a C++ compiler.
2. Execute the compiled binaries.

## Dependencies:
- OpenCV: This project uses OpenCV for image input/output and processing.
- MPI: Required for MPI parallelization.
- OpenMP: Required for OpenMP parallelization.

## Sample Input and Output:
- **Input Image:**
<img src="Samples/lena.png" alt="Sample" width="50%">
- **Output Image:** [Name of Output Image]
