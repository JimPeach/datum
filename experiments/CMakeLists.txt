cmake_minimum_required (VERSION 3.8)

# No point in running experiments in debug mode.
set(CMAKE_BUILD_TYPE Release)

add_executable (exp_memcpy_vs_loop "memcpy_vs_loop.cpp")
target_compile_options (exp_memcpy_vs_loop PUBLIC "-std=c++14")
target_link_libraries (exp_memcpy_vs_loop benchmark pthread)

add_executable (exp_realloc_vs_copy "realloc_vs_copy.cpp")
target_compile_options (exp_realloc_vs_copy PUBLIC "-std=c++14")
target_link_libraries (exp_realloc_vs_copy benchmark pthread)

add_executable (exp_cache_miss "cache_miss.cpp")
target_compile_options (exp_cache_miss PUBLIC "-std=c++14")
target_link_libraries (exp_cache_miss benchmark pthread)
