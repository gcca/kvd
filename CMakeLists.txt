cmake_minimum_required(VERSION 3.29)
project(kvd VERSION 0.1.0 LANGUAGES CXX)

option(KVD_DISABLE_TESTS "Disable tests" OFF)
option(KVD_ABSL_ROOT "Absl root path for include and lib" OFF)

add_subdirectory(src)
