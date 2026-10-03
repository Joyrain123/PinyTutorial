include(${CMAKE_SOURCE_DIR}/tools/cmake/FindInclude.cmake)

# Cmd
file (GLOB_RECURSE CMD_SOURCES CMAKE_CONFIGURE_DEPENDS
    "Cmd/Cmd.cpp"
    "Cmd/Handler/Handler.cpp")
list(APPEND SOURCES ${CMD_SOURCES})

# RcMsgHandler
if(EXTENSION_RC)
file (GLOB_RECURSE RCMSGHANDLER_SOURCES CMAKE_CONFIGURE_DEPENDS
    "Cmd/Handler/RcMsgHandler/RcMsgHandler.cpp")
list(APPEND SOURCES ${RCMSGHANDLER_SOURCES})
endif()

# RttMsgHandler
file (GLOB_RECURSE RTTMSGHANDLER_SOURCES CMAKE_CONFIGURE_DEPENDS
    "Cmd/Handler/RttMsgHandler/RttMsgHandler.cpp")
list(APPEND SOURCES ${RTTMSGHANDLER_SOURCES})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCES})

find_include_dir(HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${HEADERS})

target_link_libraries(${CMAKE_PROJECT_NAME} segger)
