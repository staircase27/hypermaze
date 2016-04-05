get_cmake_property(_vars VARIABLES)
foreach(_var ${_vars})
  message("Create Version File ${_var} ${${_var}}")
endforeach(_var)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/VERSION "${CPACK_PACKAGE_VERSION}")
