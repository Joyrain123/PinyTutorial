if(EXISTS "${SOURCE_DIR}/.config")
  return()
endif()

if(NOT KCONFIG_ALLDEFCONFIG)
  message(FATAL_ERROR "alldefconfig was not found; cannot create ${SOURCE_DIR}/.config")
endif()

execute_process(
  COMMAND "${KCONFIG_ALLDEFCONFIG}" "${KCONFIG_FILE}"
  WORKING_DIRECTORY "${SOURCE_DIR}"
  RESULT_VARIABLE CONFIG_RESULT
  OUTPUT_VARIABLE CONFIG_OUTPUT
  ERROR_VARIABLE CONFIG_ERROR)

if(NOT CONFIG_RESULT EQUAL 0)
  message(FATAL_ERROR
    "Failed to create ${SOURCE_DIR}/.config with alldefconfig:\n${CONFIG_ERROR}")
endif()
