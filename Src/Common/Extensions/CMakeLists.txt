# Add the sources
set(SOURCES)

# Add the library
add_library(Extensions STATIC ${SOURCES})

# Include the header directories
find_include_dir(HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Extensions PUBLIC ${HEADERS})

# link
target_link_libraries(Extensions PRIVATE Bsp utils Soc kernel)
