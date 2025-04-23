add_executable(eccpp-sim sim.cpp)
target_link_libraries(eccpp-sim PRIVATE eccpp)

add_executable(eccpp-fb frozen-bits.cpp)
target_link_libraries(eccpp-fb PRIVATE eccpp)
