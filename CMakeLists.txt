
find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include(KDE4Defaults)

include(MacroLibrary)

include(ConvenienceLibs.cmake)

include(ManualStuff.cmake)

include(ConfigureChecks.cmake)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


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

kde4_add_executable(hypermaze ${hypermaze_SRCS})

target_link_libraries(hypermaze ${KDE4_KDECORE_LIBS} Irrlicht openal alut curl)

install(TARGETS hypermaze ${INSTALL_TARGETS_DEFAULT_ARGS})


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

kde4_add_executable(scriptedit ${scriptedit_SRCS})

target_link_libraries(scriptedit ${KDE4_KDECORE_LIBS})

install(TARGETS scriptedit ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(test_SRCS
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

kde4_add_executable(test ${test_SRCS})

target_link_libraries(test ${KDE4_KDECORE_LIBS})

install(TARGETS test ${INSTALL_TARGETS_DEFAULT_ARGS})


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
