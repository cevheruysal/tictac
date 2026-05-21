add_library(opentictac OBJECT "")

add_subdirectory(analysis)
add_subdirectory(linear_solvers)
add_subdirectory(matrix_la)
add_subdirectory(options)
add_subdirectory(nonlinear_solvers)
add_subdirectory(parser_netlist)
add_subdirectory(utils)
add_subdirectory(netlist)
add_subdirectory(output)
add_subdirectory(computer)
#add_subdirectory(test)

target_link_libraries(opentictac PUBLIC "$<$<CXX_COMPILER_ID:GNU>:-static-libgcc -static-libstdc++>")

# We declare it is a public so that the executables have access to it immediately
# We might consider changing this if we switch to a relative/path/to/header approach
target_include_directories(opentictac SYSTEM BEFORE PUBLIC ${Boost_INCLUDE_DIR})
