target_sources(
        opentictac
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/AN_AnalysisFunctionBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AN_DCFunction.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AN_TranFunction.cpp
)

target_include_directories(
        opentictac
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
