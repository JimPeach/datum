cmake_minimum_required (VERSION 3.8)

set(CMAKE_BUILD_TYPE Debug)

file(GLOB TestFiles "*.cpp")
add_executable (datum_test ${TestFiles})
target_compile_options (datum_test PUBLIC "-std=c++14")
target_compile_options (datum_test PUBLIC "-g")

add_test(
    NAME all
    COMMAND datum_test
)
