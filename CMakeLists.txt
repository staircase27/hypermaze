project(Hypermaze)
cmake_minimum_required(VERSION  2.8.12)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CheckCXXSourceCompiles)
include(CMakeDependentOption)

########### next target ###############

set(hypermaze_SRCS
    src/core/dirns.hh
    src/core/hypio.cc
    src/core/hypio.hh
    src/core/maze.cc
    src/core/maze.hh
    src/core/mazegen.hh
    src/core/script.cc
    src/core/script.hh
    src/core/scriptimpl.hh
    src/core/SmartPointer.hh
    src/core/string.cc
    src/core/string.hh
    src/core/vector.hh
    src/hypermaze/controller.cc
    src/hypermaze/controller.hh
    src/hypermaze/guis.cc
    src/hypermaze/guis.hh
    src/hypermaze/helpgui.cc
    src/hypermaze/helpgui.hh
    src/hypermaze/hypermaze.cc
    src/hypermaze/irrdisp.cc
    src/hypermaze/irrdisp.hh
    src/hypermaze/keymap.cc
    src/hypermaze/keymapgui.cc
    src/hypermaze/keymapgui.hh
    src/hypermaze/keymap.hh
    src/irrshared/fonts.cc
    src/irrshared/fonts.hh
    src/irrshared/gui.cc
    src/irrshared/gui.hh
    src/irrshared/GUIFormattedText.cc
    src/irrshared/GUIFormattedText.hh
    src/irrshared/iMyCamera.cpp
    src/irrshared/iMyCamera.hh
    src/irrshared/irrcurl.cc
    src/irrshared/irrcurl.hh
    src/irrshared/platformcompat.cc
    src/irrshared/platformcompat.hh
    src/shared/bufhypio.cc
    src/shared/bufhypio.hh
    src/shared/irrhypioimp.cc
    src/shared/irrhypioimp.hh
    src/shared/sound.cc
    src/shared/sound.hh
    src/irrshared/opensavegui.cc
    src/irrshared/opensavegui.hh)

set(WIN32OPTION)
if(WIN32 AND NOT UNIX)
list(APPEND hypermaze_SRCS "res/icons/hypermaze.rc")
set(WIN32OPTION WIN32)
endif(WIN32 AND NOT UNIX)

add_executable(hypermaze ${WIN32OPTION} ${hypermaze_SRCS})

CMAKE_DEPENDENT_OPTION("USE_GAMES" "Install in \${CMAKE_INSTALL_PREFIX}/games rather than \${CMAKE_INSTALL_PREFIX}/bin" ON "UNIX" OFF)

if(UNIX)
   message(STATUS "On linux so using linux setup")
   if(${USE_GAMES})
     set(BINDIR "games")
   else(${USE_GAMES})
     set(BINDIR "bin")
   endif(${USE_GAMES})
   set(DATADIR "share/hypermaze")
   string(REGEX MATCH "^/usr(/|$)" INSTALL_IN_USR "${CMAKE_INSTALL_PREFIX}")
   if(INSTALL_IN_USR)
     set(SYSTEMCONFDIR "/etc/hypermaze/")
   else(INSTALL_IN_USR)
     GET_FILENAME_COMPONENT(SYSTEMCONFDIR "${CMAKE_INSTALL_PREFIX}/etc/hypermaze" ABSOLUTE)
   endif(INSTALL_IN_USR)
   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:IOSTREAM>)
   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS UNIX DATADIR="${CMAKE_INSTALL_PREFIX}/${DATADIR}" SYSTEMCONFDIR="${SYSTEMCONFDIR}")
   set(CPACK_SET_DESTDIR ON)

elseif(WIN32)
   message(STATUS "On windows so using all in one directory setup")
   set(BINDIR ".")
   set(DATADIR ".")
   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS WIN32)

elseif(APPLE)
  message(FATAL_ERROR "I don't have a mac and so haven't setup the code locations\
 for macs. If you have a mac and know where things are normally installed please contact me.")

else(UNIX)
  message(FATAL_ERROR "Unknown system type (not unix win32 or apple) so please contact me.")

endif(UNIX)

if(MSVC)
set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:IOSTREAM> $<$<CONFIG:Debug>:DEBUG> MSVC)
else(MSVC)
set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:IOSTREAM> $<$<CONFIG:Debug>:DEBUG>)
endif(MSVC)

install(TARGETS hypermaze RUNTIME DESTINATION ${BINDIR})

install(DIRECTORY levels irrlicht config DESTINATION ${DATADIR})

if(WIN32 AND NOT UNIX)
  # install icon so irrlicht finds it and uses it for the window
  install(FILES res/icons/irrlicht.ico DESTINATION ${BINDIR})
endif(WIN32 AND NOT UNIX)

if(MSVC)
message(STATUS "Using Visual Studio to run")
set_property(TARGET hypermaze PROPERTY VS_GLOBAL_LocalDebuggerEnvironment "HYPERMAZE_DATA_DIR=${CMAKE_SOURCE_DIR}/
$(LocalDebuggerEnvironment)")
else(MSVC)
add_custom_target(run ${CMAKE_COMMAND} -Daddtoenv=HYPERMAZE_DATA_DIR -DHYPERMAZE_DATA_DIR=${CMAKE_SOURCE_DIR}/ -Dprog=$<TARGET_FILE:hypermaze> -P ${CMAKE_SOURCE_DIR}/cmake/runprog.cmake DEPENDS hypermaze VERBATIM)
endif(MSVC)

set(CONST_LIB "lib")

function(generate_dll_search_paths var path)
  get_filename_component(dir "${path}" DIRECTORY)
  get_filename_component(name "${path}" NAME)
  if(name STREQUAL "")
    set(${var} ${path} PARENT_SCOPE)
  else(name STREQUAL "")
    generate_dll_search_paths(subpaths ${dir})
    foreach(subpath ${subpaths})
      string(CONCAT subpath ${subpath} "/" ${name})
      list(APPEND tmp ${subpath})
    endforeach(subpath)
    if(name STREQUAL CONST_LIB)
      foreach(subpath ${subpaths})
        string(CONCAT subpath ${subpath} "/bin")
        list(APPEND tmp ${subpath})
      endforeach(subpath)
    endif(name STREQUAL CONST_LIB)
    set(${var} ${tmp} PARENT_SCOPE)
  endif(name STREQUAL "")
endfunction()

function(hypermaze_lib_install lib var package)
  if(NOT lib MATCHES "\\.dll$")
    get_filename_component(libname "${lib}" NAME)
    get_filename_component(libdir "${lib}" DIRECTORY)
    string(LENGTH ${libname} libnamelen)

    if(libname MATCHES "\\.dll\\.a$")
      math(EXPR libnamelen "${libnamelen} - 2")
      string(SUBSTRING "${libname}" 0 ${libnamelen} libname)
    else(libname MATCHES "\\.dll\\.a$")
      if(libname MATCHES "\\.lib$")
        math(EXPR libnamelen "${libnamelen} - 4")
        string(SUBSTRING "${libname}" 0 ${libnamelen} libname)
      elseif(libname MATCHES "\\.a$")
        mathEXPR (libnamelen "${libnamelen} - 2")
        string(SUBSTRING "${libname}" 0 ${libnamelen} libname)
      endif(libname MATCHES "\\.lib$")
      string(CONCAT libname "${libname}" ".dll")
    endif(libname MATCHES "\\.dll\\.a$")

    if(libname MATCHES "^lib")
      string(SUBSTRING ${libname} 3 -1 tmp)
      list(APPEND libname ${tmp})
    endif(libname MATCHES "^lib")

    generate_dll_search_paths(libsearchpath ${libdir})
    mark_as_advanced(${var})
    find_program(${var} NAMES ${libname} HINTS ${libsearchpath} DOC "The dll for ${lib} from ${package} for distribution and running")

  else(NOT lib MATCHES "\\.dll$")
    set(${var} ${lib})

  endif(NOT lib MATCHES "\\.dll$")

  if(${var})
    install(FILES ${${var}} DESTINATION ${BINDIR})
    add_custom_command(TARGET hypermaze POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${var}} $<TARGET_FILE_DIR:hypermaze> VERBATIM)
  else(${var})
    message(SEND_ERROR "Can't find dll for ${libs} from ${package}")
  endif(${var})
endfunction()

function(hypermaze_add_package package libsvar includesvar)
    target_link_libraries(hypermaze ${${libsvar}})
    target_include_directories(hypermaze PUBLIC ${${includesvar}})
    if(WIN32 AND NOT UNIX)
      list(LENGTH ${libsvar} libcount)
      if(libcount EQUAL "1")
        string(TOUPPER ${package} var)
        set(var "${var}_DLL")
        hypermaze_lib_install(${${libsvar}} ${var} ${package})
      else(libcount EQUAL "1")
        set(used_vars "")
        foreach(lib ${${libsvar}})
          string(TOUPPER ${package} var)
          get_filename_component(basevar "${lib}" NAME_WE)
          if(basevar MATCHES "^lib")
            string(SUBSTRING ${basevar} 3 -1 basevar)
          endif(basevar MATCHES "^lib")
          string(TOUPPER ${basevar} basevar)
          set(basevar "${var}_${basevar}")
          set(var "${basevar}_DLL")
          set(i 0)
          set(found 1)
          while(found)
            set(found 0)
            foreach(used_var ${used_vars})
              if(used_var STREQUAL var)
                math(EXPR i "${i} + 1")
                set(var "${basevar}_${i}_DLL")
                set(found 1)
                break()
              endif(used_var STREQUAL var)
            endforeach(used_var)
          endwhile(found)
          list(APPEND used_vars "${var}")
          hypermaze_lib_install(${lib} ${var} ${package})
        endforeach()
      endif(libcount EQUAL "1")
    endif(WIN32 AND NOT UNIX)
endfunction()

function(hypermaze_find_required_package package foundvar libsvar includesvar defn)
  find_package(${package} REQUIRED)
  if(${foundvar})
    hypermaze_add_package(${package} ${libsvar} ${includesvar})
    set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS ${defn})
  endif(${foundvar})
endfunction()

function(hypermaze_find_optional_packages option defn description default count)
  math(EXPR count "${count} * 4 + 1")
  set(foundvars)
  foreach(index RANGE 5 ${count} 4)
    math(EXPR indexfoundvar "${index} + 1")
    find_package(${ARGV${index}})
    list(APPEND foundvars ${ARGV${indexfoundvar}})
  endforeach(index)
  CMAKE_DEPENDENT_OPTION(${option} ${description} ${default} "${foundvars}" OFF)
  if(${option})
    set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS ${defn})
    foreach(index RANGE 5 ${count} 4)
      math(EXPR indexlibsvar "${index} + 2")
      math(EXPR indexincludesvar "${index} +3")
      hypermaze_add_package(${ARGV${index}} ${ARGV${indexlibsvar}} ${ARGV${indexincludesvar}})
    endforeach(index)
  endif(${option})
endfunction()

hypermaze_find_required_package(Irrlicht IRRLICHT_FOUND IRRLICHT_LIBRARIES IRRLICHT_INCLUDE_DIRS IRRLICHT)

hypermaze_find_optional_packages(USE_OPENAL USE_OPENAL "Use OpenAL library for sound" ON 2 ALUT ALUT_FOUND ALUT_LIBRARIES ALUT_INCLUDE_DIR OpenAL  OPENAL_FOUND OPENAL_LIBRARY OPENAL_INCLUDE)

hypermaze_find_optional_packages(USE_CURL USE_CURL "Use the curl library for url opening (enables url opening feature)" ON 1 CURL CURL_FOUND CURL_LIBRARIES CURL_INCLUDE_DIR)

if(WIN32 AND NOT UNIX)
  CHECK_CXX_SOURCE_COMPILES( "
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0601
#include \"windows.h\"
#include \"Shlobj.h\"
int main(){
  wchar_t* winpath=0;
  SHGetKnownFolderPath(FOLDERID_ProgramData,0,0,&winpath);
  CoTaskMemFree(winpath);
  SHGetKnownFolderPath(FOLDERID_RoamingAppData,0,0,&winpath);
  CoTaskMemFree(winpath);
  return 0;
}
" HAVE_WIN_KNOWN_FOLDERS )
  if(HAVE_WIN_KNOWN_FOLDERS)
    message(STATUS "Have known folders")
    set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS WIN32_USE_KNOWNFOLDER)
  endif(HAVE_WIN_KNOWN_FOLDERS)
endif(WIN32 AND NOT UNIX)

########### next target ###############

set(scriptedit_SRCS
    src/scriptedit/scriptedit.cc
    src/core/string.hh
    src/core/string.cc
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

if(NOT MSVC)
add_custom_target(run-scriptedit $<TARGET_FILE:scriptedit> DEPENDS scriptedit VERBATIM)
endif(NOT MSVC)


########### next target ###############

set(levelgen_SRCS
    src/test/test.cc
    src/core/string.hh
    src/core/string.cc
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

if(NOT MSVC)
add_custom_target(run-levelgen $<TARGET_FILE:levelgen> DEPENDS levelgen VERBATIM)
endif(NOT MSVC)
########### really compile all ###############

add_custom_target(all-full DEPENDS hypermaze scriptedit levelgen)

########### make the documentation ###############

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  file(GLOB_RECURSE ALL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/DoxyfileNOMACRO.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} doc/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/doc/DoxyfileNOMACRO.in ${ALL_SOURCES})
endif(DOXYGEN_FOUND)

########### cpack ###############

set(CPACK_PACKAGE_NAME "Hypermaze")

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${BINDIR})

INCLUDE(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION "Hypermaze puzzle game")

set(CPACK_PACKAGE_VENDOR "staircase")

set(CPACK_PACKAGE_CONTACT "staircase <staircase@darkfeldgames.net>")

SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")

set(CPACK_PACKAGE_VERSION_MAJOR "\${CPACK_PACKAGE_VERSION_MAJOR}")

set(CPACK_PACKAGE_VERSION_MINOR "\${CPACK_PACKAGE_VERSION_MINOR}")

set(CPACK_PACKAGE_VERSION_PATCH "\${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "3D maze like puzzle")

set(CPACK_STRIP_FILES "ON")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CPackExtra.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/CPackExtra.cmake" @ONLY)

set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackExtra.cmake")

#set(CPACK_INSTALL_COMMANDS "${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/cmake/Test.cmake")

# DEB options

set(CPACK_DEBIAN_PACKAGE_NAME "hypermaze")

set(CPACK_DEBIAN_PACKAGE_SECTION "games")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS "ON")

#add -deb so can have -s in "upstream version" part
set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-deb")

set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "3D maze like puzzle where you have to get a \\\"string\\\" through a \\\"maze\\\"
 For more explanations run the puzzle or for information about hypermazes in
 general see http://www.astrolog.org/labyrnth/hypermaz.htm. A tutorial and a
 level generator are included.")

# NSIS windows installer options

set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}") # should still show full version even though install version only contains

set(CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_PACKAGE_NAME}")

set(CPACK_NSIS_MENU_LINKS "hypermaze.exe" "Hypermaze")

set(CPACK_NSIS_INSTALLED_ICON_NAME "hypermaze.exe,0")

set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/icons/install.ico")

set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/res/icons/uninstall.ico")

set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")

set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")

set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

set(CPACK_NSIS_MUI_FINISHPAGE_RUN "hypermaze.exe")

set(CPACK_NSIS_DEFINES "!define MUI_STARTMENUPAGE_DEFAULTFOLDER \\\"${CPACK_PACKAGE_INSTALL_DIRECTORY}\\\"")

# Source package options

set(CPACK_SOURCE_IGNORE_FILES /\\\\.git/;;/build/;.*~$;/\\\\..*;.*\\\\.aps)

include(CPack)
