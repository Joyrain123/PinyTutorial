include(${CMAKE_SOURCE_DIR}/tools/cmake/FindInclude.cmake)

file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS "*.cpp")

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCES})

find_include_dir(HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${HEADERS})
