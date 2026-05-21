target_sources(
        opentictac
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sim_utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XMLReader.cpp
)

target_include_directories(
        opentictac
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}

)
