add_executable(tictac_simulator main_simulator.cpp)
target_link_libraries(tictac_simulator PRIVATE opentictac)

install(TARGETS tictac_simulator
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin)

