include_directories(${Boost_INCLUDE_DIRS})
add_executable(merger main.cpp merger.cpp)
target_link_libraries(merger ${Boost_LIBRARIES})
target_compile_features(merger PRIVATE cxx_auto_type cxx_range_for cxx_uniform_initialization)
