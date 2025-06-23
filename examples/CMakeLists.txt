add_executable(hello-world hello-world.cpp)
target_link_libraries(hello-world PRIVATE eccpp)

add_executable(sim sim.cpp)
target_link_libraries(sim PRIVATE eccpp)

add_executable(frozen-bits frozen-bits.cpp)
target_link_libraries(frozen-bits PRIVATE eccpp)

add_executable(polar-dist polar-dist.cpp)
target_link_libraries(polar-dist PRIVATE eccpp)

add_executable(punct punct.cpp)
target_link_libraries(punct PRIVATE eccpp)
