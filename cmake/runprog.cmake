foreach (_var ${addtoenv})
  set(ENV{${_var}} ${${_var}})
endforeach()

message(STATUS "Running hypermaze: ${prog} ${args}")

execute_process(COMMAND ${prog} ${args} RESULT_VARIABLE result)

message(STATUS "Hypermaze finished: ${result}")
