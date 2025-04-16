add_executable(eccpp_sim sim.cpp)
target_link_libraries(eccpp_sim PRIVATE eccpp)

set_target_properties(eccpp_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS eccpp_sim
    RUNTIME DESTINATION bin
)
