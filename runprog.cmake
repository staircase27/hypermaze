foreach (_var ${addtoenv})
  set(ENV{${_var}} ${${_var}})
endforeach()

execute_process(COMMAND env)
execute_process(COMMAND ${prog} ${args})
