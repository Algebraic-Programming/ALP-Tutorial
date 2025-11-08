
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

    //------------------------------
    // 1) Create a 3x3 sparse matrix A
    //------------------------------
    std::printf("Step 1: Constructing a 3x3 sparse matrix A.\n");
    Matrix<double> A(3, 3);
    // Reserve space for 6 nonzeros
    resize(A, 6);
    // Populate A from (Iidx,Jidx,Avalues)
    buildMatrixUnique( A, &(Iidx[0]), &(Jidx[0]), Avalues, /* nvals = */ 6, SEQUENTIAL );

    //------------------------------
    // 2) Create a 3-element vector x and initialize x = [1, 2, 3]^T
    //------------------------------
    std::printf("Step 2: Creating vector x = [1, 2, 3]^T.\n");
    Vector<double> x(3);
    set<descriptors::no_operation>(x, 0.0);           // zero-out
    setElement<descriptors::no_operation>(x, 1.0, 0); // x(0) = 1.0
    setElement<descriptors::no_operation>(x, 2.0, 1); // x(1) = 2.0
    setElement<descriptors::no_operation>(x, 3.0, 2); // x(2) = 3.0
    // note: setElement() supports vectors only
    //       set() support for matrix is work in progress

    //------------------------------
    // 3) Create two result vectors y and z (dimension 3)
    //------------------------------
    Vector<double> y(3), z(3);
    set<descriptors::no_operation>(y, 0.0);
    set<descriptors::no_operation>(z, 0.0);

    //------------------------------
    // 4) Use the built-in “plusTimes” semiring alias
    //      (add = plus, multiply = times, id‐add = 0.0, id-mul = 1.0)
    //------------------------------
    auto plusTimes = grb::semirings::plusTimes<double>();

    //------------------------------
    // 5) Compute y = A·x  (matrix‐vector multiply under plus‐times)
    //------------------------------
    std::printf("Step 3: Computing y = A·x under plus‐times semiring.\n");
    {
        RC rc = mxv<descriptors::no_operation>(y, A, x, plusTimes);
        if(rc != SUCCESS) {
            std::cerr << "Error: mxv(y,A,x) failed with code " << toString(rc) << "\n";
            return (int)rc;
        }
    }

    //------------------------------
    // 6) Compute z = x ⊙ y  (element‐wise multiply) via eWiseMul with semiring
    //------------------------------
    std::printf("Step 4: Computing z = x ⊙ y (element‐wise multiply).\n");
    {
        RC rc = eWiseMul<descriptors::no_operation>(
            z, x, y, plusTimes
        );
        if(rc != SUCCESS) {
            std::cerr << "Error: eWiseMul(z,x,y,plusTimes) failed with code " << toString(rc) << "\n";
            return (int)rc;
        }
    }
    
    //------------------------------
    // 7) Compute dot_val = xᵀ·x  (dot‐product under plus‐times semiring)
    //------------------------------
    std::printf("Step 5: Computing dot_val = xᵀ·x under plus‐times semiring.\n");
    double dot_val = 0.0;
    {
        RC rc = dot<descriptors::no_operation>(dot_val, x, x, plusTimes);
        if(rc != SUCCESS) {
            std::cerr << "Error: dot(x,x) failed with code " << toString(rc) << "\n";
            return (int)rc;
        }
    }

    //------------------------------
    // 8) Print x, y, z, and dot_val
    //    We reconstruct each full 3 - vector by filling an std::array<3,double>
    //------------------------------
    auto printVector = [&](const Vector<double> &v, const std::string &name) {
        // Initialize all entries to zero
        std::array<double,3> arr = { 0.0, 0.0, 0.0 };
        // Overwrite stored (nonzero) entries
        for(const auto &pair : v) {
            // pair.first = index, pair.second = value
            arr[pair.first] = pair.second;
        }
        // Print
        std::printf("%s = [ ", name.c_str());
        for(size_t i = 0; i < 3; ++i) {
            std::printf("%g", arr[i]);
            if(i + 1 < 3) std::printf(", ");
        }
        std::printf(" ]\n");
    };

    std::printf("\n-- Results --\n");
    printVector(x, "x");
    printVector(y, "y = A·x");
    printVector(z, "z = x ⊙ y");
    std::printf("dot(x,x) = %g\n\n", dot_val);

    return EXIT_SUCCESS;
}