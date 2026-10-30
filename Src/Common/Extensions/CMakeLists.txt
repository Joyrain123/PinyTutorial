# Add the sources
set(SOURCES Servo/ServoPwm.cpp)

if(EXTENSION_ET08A)
  list(APPEND SOURCES RemoteControl/ET08A/ET08A.cpp)
endif()

# Add the library
add_library(Extensions STATIC ${SOURCES})

# Include the header directories
find_include_dir(HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Extensions PUBLIC ${HEADERS})

# link
target_link_libraries(Extensions PRIVATE Bsp utils Soc kernel)
