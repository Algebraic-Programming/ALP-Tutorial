/*
 * minimal ALP (GraphBLAS) example.
 *
 * To compile (using the reference OpenMP backend):
 *    grbcxx -b reference_omp example.cpp -o example
 *
 * To run:
 *    grbrun ./example
 */

#include <cstdio>
#include <iostream>
#include <vector>
#include <utility>   // for std::pair
#include <array>

#include <graphblas.hpp>

using namespace grb;

// Indices and values for our sparse 3x3 matrix A:
//
//      A = [ 1   0   2 ]
//          [ 0   3   4 ]
//          [ 5   6   0 ]
//
// We store the nonzero entries via buildMatrixUnique.
static const size_t Iidx[6]    = { 0, 0, 1, 1, 2, 2 };  // row indices
static const size_t Jidx[6]    = { 0, 2, 1, 2, 0, 1 };  // column indices
static const double Avalues[6] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };

int main( int argc, char **argv ) {
    (void)argc;
    (void)argv;
    std::printf("example (ALP/GraphBLAS) corrected API usage\n\n");

    // 1) Create a 3x3 sparse matrix A
    std::printf("Step 1: Constructing a 3x3 sparse matrix A.\n");
    Matrix<double> A(3, 3);
    // TODO 1: Reserve memory for 6 non-zero entries and build A from (Iidx,Jidx,Avalues), 
    //         use resize and buildMatrixUnique

    // 2) Create a 3-element vector x and initialize x = [1, 2, 3]^T
    // TODO 2: Initialize x = [1, 2, 3]^T
    //         first clear with set, then setElement for indices 0..2

    // 3) Create two result vectors y and z (dimension 3) and set to zero
    // TODO 3: Create y and z with proper type

    // 4) Use the built-in “plusTimes” semiring alias
    //      (add = plus, multiply = times, id‐add = 0.0, id-mul = 1.0)
    auto plusTimes = grb::semirings::plusTimes<double>();

    // 5) Compute y = A·x  (matrix‐vector multiply under plus‐times semiring)
    // TODO 3: y = A·x  (matrix-vector multiply under plusTimes) using mxv()

    // 6) Compute z = x ⊙ y  (element‐wise multiply) via eWiseMul with semiring
    // TODO 4: z = x ⊙ y  (element-wise multiply) using eWiseMul()
    
    // 7) Compute dot_val = xᵀ·x  (dot‐product under plus‐times semiring)
    // TODO 5: dot_val = x^T x (dot-product under plusTimes) using dot() 

    // 8) Print x, y, z, and dot_val

    return EXIT_SUCCESS;
}