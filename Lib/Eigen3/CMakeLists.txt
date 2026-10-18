include (${CMAKE_SOURCE_DIR}/tools/cmake/FindInclude.cmake)

# Add the library
add_library(Eigen3 INTERFACE)

# Include the header directories
FIND_INCLUDE_DIR(HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Eigen3 INTERFACE ${HEADERS})

