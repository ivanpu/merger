include_directories(${Boost_INCLUDE_DIRS})
add_executable(merger main.cpp merger.cpp)
target_link_libraries(merger ${Boost_LIBRARIES})
