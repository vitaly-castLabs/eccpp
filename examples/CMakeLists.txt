add_executable(eccpp_sim sim.cpp)
target_link_libraries(eccpp_sim PRIVATE eccpp)

install(TARGETS eccpp_sim
    RUNTIME DESTINATION bin
)
