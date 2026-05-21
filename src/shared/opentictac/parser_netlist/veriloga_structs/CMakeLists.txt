add_subdirectory(declarations)
add_subdirectory(expressions)
add_subdirectory(statements)
add_subdirectory(titan_specific)
add_subdirectory(utils)
add_subdirectory(visitor)

target_include_directories(
        opentictac
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(
        opentictac
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
