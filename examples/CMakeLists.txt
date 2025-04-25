add_executable(sim sim.cpp)
target_link_libraries(sim PRIVATE eccpp)

add_executable(frozen-bits frozen-bits.cpp)
target_link_libraries(frozen-bits PRIVATE eccpp)
