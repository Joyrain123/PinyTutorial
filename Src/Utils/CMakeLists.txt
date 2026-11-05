# Add the library
add_library(utils STATIC)

if(USE_HARDFAULT)
  target_sources(utils
                 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Hardfault/Hardfault.cpp)
  target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Hardfault)
endif()

if(USE_STM_LOG)
  target_sources(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Log/StmLog.cpp)
  if(LOG_OUTPUT_RTT)
    target_sources(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Log/Output/RttOutput.cpp)
  endif()
  if(LOG_OUTPUT_UART)
    target_sources(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Log/Output/UartOutput.cpp)
  endif()
  target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Log)
endif()

if(USE_SINGLETON)
  target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Singleton)
endif()

# link
target_link_libraries(
  utils
  PRIVATE Soc segger Bsp freertos_kernel)
