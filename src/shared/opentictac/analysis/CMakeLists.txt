add_subdirectory(analyfunctions)
add_subdirectory(controllers)

target_sources(
        opentictac
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/AN_Analysis.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AN_DCAnalysis.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AN_TranAnalysis.cpp
)

target_include_directories(
        opentictac
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
