option(X_TOOLS_IMPORT_MODULE_MODBUS "Enable Modbus Studio module" ON)
if(X_TOOLS_IMPORT_MODULE_MODBUS)
  include_directories(${CMAKE_CURRENT_LIST_DIR})
  add_compile_definitions(X_TOOLS_IMPORT_MODULE_MODBUS)
  file(GLOB_RECURSE X_TOOLS_MODBUS_SOURCE "${CMAKE_CURRENT_LIST_DIR}/*")
  list(APPEND X_TOOLS_SOURCE ${X_TOOLS_MODBUS_SOURCE})
endif()
