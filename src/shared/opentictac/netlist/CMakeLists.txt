add_subdirectory(devices)

target_sources(
        opentictac
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Netlist.cpp
)
target_include_directories(
        opentictac
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
