cmake_minimum_required(VERSION 3.20)
project(projekt)

set(CMAKE_CXX_STANDARD 20)

add_executable(projekt main.cpp DenseMatrix.h MatrixOperations.h Commons.h CSR.h SparseMatrix.h)
