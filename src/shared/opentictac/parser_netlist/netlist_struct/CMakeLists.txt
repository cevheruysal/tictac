target_sources(
        opentictac
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_ConditionedNetlist.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_Element.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_Function.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_Model.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_NetlistStructBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_NetlistStructInfo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_NetlistStructSequence.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_NetworkNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_Param.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_SubcirDef.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_SubcirSettings.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/IP_XCall.cpp
)

target_include_directories(
        opentictac
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}

)

add_subdirectory(xspice)
