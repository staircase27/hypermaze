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

if(WIN32 OR APPLE)
   message(STATUS "On windows or mac so using all in one directory setup")
   
   set(BINDIR ".")
   
   set(DATADIR ".")
   
   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS DATADIR="")
   
else(WIN32 OR APPLE)

   message(STATUS "On linux so using linux setup")
   
   set(BINDIR "bin")
   
   set(DATADIR "share/hypermaze")

   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:IOSTREAM>)

   set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS DATADIR="../share/hypermaze")

endif(WIN32 OR APPLE)

set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:IOSTREAM>)

install(TARGETS hypermaze RUNTIME DESTINATION ${BINDIR})

install(DIRECTORY levels irrlicht DESTINATION ${DATADIR})

set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS IRRLICHT)

find_package(Irrlicht REQUIRED)

target_include_directories(hypermaze PUBLIC ${IRRLICHT_INCLUDE_DIRS})

target_link_libraries(hypermaze ${IRRLICHT_LIBRARIES})

if(WIN32)

  install(FILES ${IRRLICHT_LIBRARIES} DESTINATION ${BINDIR})

endif(WIN32)

find_package(OpenAL)

if(OPENAL_FOUND)

        target_link_libraries(hypermaze ${OPENAL_LIBRARY} alut)

        target_include_directories(hypermaze PUBLIC ${OPENAL_INCLUDE_DIR})

        set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS OPENAL)

        if(WIN32)

                install(FILES ${OPENAL_LIBRARY} DESTINATION ${BINDIR})

        endif(WIN32)

endif(OPENAL_FOUND)

find_package(CURL)

if(CURL_FOUND)

        target_link_libraries(hypermaze ${CURL_LIBRARIES})

        set_property(TARGET hypermaze APPEND PROPERTY COMPILE_DEFINITIONS USE_CURL)

        target_include_directories(hypermaze PUBLIC ${CURL_INCLUDE_DIRS})
        
        if(WIN32)

                install(FILES ${CURL_LIBRARIES} DESTINATION ${BINDIR})

        endif(WIN32)

endif(CURL_FOUND)

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


########### install files ###############



#original Makefile.am contents follow:

#bin_PROGRAMS = hypermaze scriptedit test
#
#hypermaze_SOURCES = src/shared/irrhypioimp.cc src/shared/irrhypioimp.hh src/shared/bufhypio.cc src/shared/bufhypio.hh src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/irrshared/GUIFormattedText.cc src/irrshared/GUIFormattedText.hh src/irrshared/fonts.hh src/irrshared/fonts.cc src/irrshared/irrcurl.cc src/irrshared/irrcurl.hh  src/irrshared/iMyCamera.cpp src/irrshared/iMyCamera.hh src/hypermaze/gui.cc src/hypermaze/helpgui.hh src/hypermaze/controller.hh src/hypermaze/keymap.hh src/shared/sound.hh src/shared/sound.cc src/hypermaze/hypermaze.cc src/hypermaze/irrdisp.hh src/hypermaze/irrdisp.cc src/hypermaze/keymapgui.hh src/hypermaze/gui.hh src/hypermaze/controller.cc src/hypermaze/keymap.cc
#hypermaze_CPPFLAGS = -DIRRLICHT -DOPENAL -DIOSTREAM -isystem/usr/include/irrlicht/
#hypermaze_LDFLAGS = -lIrrlicht -lopenal -lalut -lcurl
#
#scriptedit_SOURCES = src/scriptedit/scriptedit.cc src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/shared/cpphypioimp.cc src/shared/cpphypioimp.hh
#scriptedit_CPPFLAGS = -DIOSTREAM
#
#test_SOURCES = src/test/test.cc src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/shared/cpphypioimp.cc src/shared/cpphypioimp.hh
#test_CPPFLAGS = -DIOSTREAM
#
