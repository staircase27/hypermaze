project(Hypermaze)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

########### next target ###############

set(hypermaze_SRCS
    src/shared/irrhypioimp.cc
    src/shared/irrhypioimp.hh
    src/shared/bufhypio.cc
    src/shared/bufhypio.hh
    src/core/string.hh
    src/core/maze.hh
    src/core/script.hh
    src/core/dirns.hh
    src/core/maze.cc
    src/core/mazegen.hh
    src/core/SmartPointer.hh
    src/core/hypio.cc
    src/core/hypio.hh
    src/core/script.cc
    src/core/vector.hh
    src/core/scriptimpl.hh
    src/irrshared/GUIFormattedText.cc
    src/irrshared/GUIFormattedText.hh
    src/irrshared/fonts.hh
    src/irrshared/fonts.cc
    src/irrshared/irrcurl.cc
    src/irrshared/irrcurl.hh
    src/irrshared/iMyCamera.cpp
    src/irrshared/iMyCamera.hh
    src/hypermaze/gui.cc
    src/hypermaze/helpgui.hh
    src/hypermaze/controller.hh
    src/hypermaze/keymap.hh
    src/shared/sound.hh
    src/shared/sound.cc
    src/hypermaze/hypermaze.cc
    src/hypermaze/irrdisp.hh
    src/hypermaze/irrdisp.cc
    src/hypermaze/keymapgui.hh
    src/hypermaze/gui.hh
    src/hypermaze/controller.cc
    src/hypermaze/keymap.cc)

add_executable(hypermaze ${hypermaze_SRCS})

if(WIN32)
   message(STATUS "On windows so using all in one directory setup")
   
   set(BINDIR ".")
   
   set(DATADIR ".")
   
   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS WIN32)
   
elseif(APPLE)
  message(FATAL_ERROR "I don't have a mac and so haven't setup the code locations\
 for macs. If you have a mac and know where things are normally installed please contact me.")

else(WIN32) # on unix like as not windows or apple

   message(STATUS "On linux so using linux setup")
   
   set(BINDIR "bin")
   
   set(DATADIR "share/hypermaze")

   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:IOSTREAM>)

   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS UNIX DATADIR="${CMAKE_INSTALL_PREFIX}/bin")

endif(WIN32)

set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:IOSTREAM> $<$<CONFIG:Debug>:DEBUG>)

install(TARGETS hypermaze RUNTIME DESTINATION ${BINDIR})

install(DIRECTORY levels irrlicht DESTINATION ${DATADIR})

add_custom_target(run ${CMAKE_COMMAND} -Daddtoenv=HYPERMAZE_DATA_DIR -DHYPERMAZE_DATA_DIR=${CMAKE_SOURCE_DIR}/ -Dprog=$<TARGET_FILE:hypermaze> -P ${CMAKE_SOURCE_DIR}/runprog.cmake DEPENDS hypermaze VERBATIM)

if(WIN32)

  add_custom_target(pre-run ${CMAKE_COMMAND} -E echo "Copying Libraries")

  add_dependencies(run pre-run)

endif(WIN32)

function(hypermaze_find_package package foundvar libsvar includesvar defn)

  find_package(${package} ${ARGV5})

  if(${foundvar})

    target_link_libraries(hypermaze ${${libsvar}})

    target_include_directories(hypermaze PUBLIC ${${includesvar}})

    set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS ${defn})

    if(WIN32)

      install(FILES ${${libsvar}} DESTINATION ${BINDIR})

      foreach(lib ${${libsvar}})
      
        add_custom_command(TARGET pre-run PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${lib} $<TARGET_FILE_DIR:hypermaze> VERBATIM)
        
      endforeach()

    endif(WIN32)

  endif(${foundvar})

endfunction()

hypermaze_find_package(Irrlicht IRRLICHT_FOUND IRRLICHT_LIBRARIES IRRLICHT_INCLUDE_DIRS IRRLICHT REQUIRED)

hypermaze_find_package(ALUT ALUT_FOUND ALUT_LIBRARIES ALUT_INCLUDE_DIR ALUT)

hypermaze_find_package(OpenAL OPENAL_FOUND OPENAL_LIBRARY OPENAL_INCLUDE_DIR OPENAL)

hypermaze_find_package(CURL CURL_FOUND CURL_LIBRARIES CURL_INCLUDE_DIR USE_CURL)

########### next target ###############

set(scriptedit_SRCS
    src/scriptedit/scriptedit.cc
    src/core/string.hh
    src/core/maze.hh
    src/core/script.hh
    src/core/dirns.hh
    src/core/maze.cc
    src/core/mazegen.hh
    src/core/SmartPointer.hh
    src/core/hypio.cc
    src/core/hypio.hh
    src/core/script.cc
    src/core/vector.hh
    src/core/scriptimpl.hh
    src/shared/cpphypioimp.cc
    src/shared/cpphypioimp.hh)

add_executable(scriptedit EXCLUDE_FROM_ALL ${scriptedit_SRCS})

set_property(TARGET scriptedit APPEND PROPERTY COMPILE_DEFINITIONS IOSTREAM)

add_custom_target(run-scriptedit $<TARGET_FILE:scriptedit> DEPENDS scriptedit VERBATIM)

########### next target ###############

set(levelgen_SRCS
    src/test/test.cc
    src/core/string.hh
    src/core/maze.hh
    src/core/script.hh
    src/core/dirns.hh
    src/core/maze.cc
    src/core/mazegen.hh
    src/core/SmartPointer.hh
    src/core/hypio.cc
    src/core/hypio.hh
    src/core/script.cc
    src/core/vector.hh
    src/core/scriptimpl.hh
    src/shared/cpphypioimp.cc
    src/shared/cpphypioimp.hh)

add_executable(levelgen EXCLUDE_FROM_ALL ${levelgen_SRCS})

set_property(TARGET levelgen APPEND PROPERTY COMPILE_DEFINITIONS IOSTREAM)

add_custom_target(run-levelgen $<TARGET_FILE:levelgen> DEPENDS levelgen VERBATIM)
########### really compile all ###############

add_custom_target(all-full DEPENDS hypermaze scriptedit levelgen)
