target_sources(
        opentictac
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_FillExpansionStack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_NameSearchVisitor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_NetlistVisitBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_NetlistVisitorUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_SubcktCopyVisitor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_SubcktElabor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_VerboseVisitor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GraphStringFilter.cpp
)

target_include_directories(
        opentictac
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
