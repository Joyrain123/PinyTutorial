set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Locate the ARM GNU toolchain. It may be available through PATH, or through
# ARM_GNU_TOOLCHAIN_DIR when it is installed in a user-local directory.
find_program(ARM_NONE_EABI_GCC
  NAMES arm-none-eabi-gcc
  HINTS
    "$ENV{ARM_GNU_TOOLCHAIN_DIR}/bin"
    "/home/gdfish/usr/arm-gnu-toolchain/bin")

if(NOT ARM_NONE_EABI_GCC)
  message(FATAL_ERROR
    "arm-none-eabi-gcc was not found. Add the ARM GNU toolchain to PATH or "
    "set ARM_GNU_TOOLCHAIN_DIR to its installation directory.")
endif()

get_filename_component(ARM_TOOLCHAIN_BIN_DIR "${ARM_NONE_EABI_GCC}" DIRECTORY)
set(ARM_NONE_EABI_GXX     "${ARM_TOOLCHAIN_BIN_DIR}/arm-none-eabi-g++")
set(ARM_NONE_EABI_OBJCOPY "${ARM_TOOLCHAIN_BIN_DIR}/arm-none-eabi-objcopy")
set(ARM_NONE_EABI_SIZE    "${ARM_TOOLCHAIN_BIN_DIR}/arm-none-eabi-size")

set(CMAKE_C_COMPILER   "${ARM_NONE_EABI_GCC}")
set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_GCC}")
set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_GXX}")
set(CMAKE_LINKER       "${ARM_NONE_EABI_GXX}")
set(CMAKE_OBJCOPY      "${ARM_NONE_EABI_OBJCOPY}")
set(CMAKE_SIZE         "${ARM_NONE_EABI_SIZE}")

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-O3 -g3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g3")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --specs=nano.specs")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--print-memory-usage")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--no-warn-rwx-segments")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -u _printf_float")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lstdc++ -lsupc++ -Wl,--end-group")






