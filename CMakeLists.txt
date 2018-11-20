# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("datum")

enable_testing()

include_directories("include")
include_directories("external/include")
link_directories("external/lib")

add_subdirectory ("test")
add_subdirectory ("test/benchmarks")
add_subdirectory ("experiments")