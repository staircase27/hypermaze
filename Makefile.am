bin_PROGRAMS = hypermaze scriptedit test

hypermaze_SOURCES = src/shared/irrhypioimp.cc src/shared/irrhypioimp.hh src/shared/bufhypio.cc src/shared/bufhypio.hh src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/hypermaze/helpgui.hh src/hypermaze/controller.hh src/hypermaze/GUIFormattedText.cc src/hypermaze/gui.cc src/hypermaze/keymap.hh src/hypermaze/iMyCamera.hh src/hypermaze/irrcurl.cc src/hypermaze/fonts.cc src/hypermaze/sound.hh src/hypermaze/sound.cc src/hypermaze/hypermaze.cc src/hypermaze/irrdisp.hh src/hypermaze/irrcurl.hh src/hypermaze/irrdisp.cc src/hypermaze/keymapgui.hh src/hypermaze/gui.hh src/hypermaze/GUIFormattedText.hh src/hypermaze/fonts.hh src/hypermaze/controller.cc src/hypermaze/autoscan.log src/hypermaze/keymap.cc src/hypermaze/iMyCamera.cpp
hypermaze_CPPFLAGS = -DIRRLICHT -DOPENAL -DIOSTREAM -isystem/usr/include/irrlicht/
hypermaze_LDFLAGS = -lIrrlicht -lopenal -lalut -lcurl

scriptedit_SOURCES = src/scriptedit/scriptedit.cc src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/shared/cpphypioimp.cc src/shared/cpphypioimp.hh
scriptedit_CPPFLAGS = -DIOSTREAM

test_SOURCES = src/test/test.cc src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/shared/cpphypioimp.cc src/shared/cpphypioimp.hh
test_CPPFLAGS = -DIOSTREAM

