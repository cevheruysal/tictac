get_target_property(TICTAC_SIMETRIX_SOURCES tictac_simetrix SOURCES)
get_target_property(TICTAC_SIMETRIX_INCLUDE_DIR tictac_simetrix INCLUDE_DIRECTORIES)


list(REMOVE_ITEM TICTAC_SIMETRIX_SOURCES ${CMAKE_SOURCE_DIR}/src/main_simetrix/main_simetrix.cpp)
message("${TICTAC_SIMETRIX_SOURCES}")

add_executable(test_tictac_simetrix
        test_main.cpp
        test_graph_filter.cpp
        ${TICTAC_SIMETRIX_SOURCES}
        )

target_include_directories(
        test_tictac_simetrix
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${TICTAC_SIMETRIX_INCLUDE_DIR}
)

target_include_directories(
        test_tictac_simetrix
        SYSTEM
        BEFORE
        PRIVATE
        /home/titan_sw/BOOST_dir/boost_1_57_0
)
