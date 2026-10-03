function(showMessage)
  message("------------------- build info ----------------------")
  message("-- Build type:" ${CMAKE_BUILD_TYPE})
  message("-- C Standard: " ${CMAKE_C_STANDARD})
  message("-- C++ Standard: " ${CMAKE_CXX_STANDARD})
  message("-- c compiler: " ${CMAKE_C_COMPILER_VERSION})
  message("-- c++ compiler: " ${CMAKE_CXX_COMPILER_VERSION})
  message("-- robot version: " ${GIT_VERSION})
  message("-----------------------------------------------------")
  message(
    "Welcome To PIONEER E-Control Team !                  \n"
    "    ____   _               ______        __                _         __\n"
    "   / __ \\ (_)____   __  __/_  __/__  __ / /_ ____   _____ (_)____ _ / /\n"
    "  / /_/ // // __ \\ / / / / / /  / / / // __// __ \\ / ___// // __ `// /\n" 
    " / ____// // / / // /_/ / / /  / /_/ // /_ / /_/ // /   / // /_/ // /\n"  
    "/_/    /_//_/ /_/ \\__, / /_/   \\__,_/ \\__/ \\____//_/   /_/ \\__,_//_/\n"   
    "                 /____/\n")                                                
endfunction()
