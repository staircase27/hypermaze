bin_PROGRAMS = hypermaze scriptedit test

hypermaze_SOURCES = src/shared/irrhypioimp.cc src/shared/irrhypioimp.hh src/shared/bufhypio.cc src/shared/bufhypio.hh src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/irrshared/GUIFormattedText.cc src/irrshared/GUIFormattedText.hh src/irrshared/fonts.hh src/irrshared/fonts.cc src/irrshared/irrcurl.cc src/irrshared/irrcurl.hh  src/irrshared/iMyCamera.cpp src/irrshared/iMyCamera.hh src/hypermaze/gui.cc src/hypermaze/helpgui.hh src/hypermaze/controller.hh src/hypermaze/keymap.hh src/shared/sound.hh src/shared/sound.cc src/hypermaze/hypermaze.cc src/hypermaze/irrdisp.hh src/hypermaze/irrdisp.cc src/hypermaze/keymapgui.hh src/hypermaze/gui.hh src/hypermaze/controller.cc src/hypermaze/keymap.cc
hypermaze_CPPFLAGS = -DIRRLICHT -DOPENAL -DIOSTREAM -isystem/usr/include/irrlicht/
hypermaze_LDFLAGS = -lIrrlicht -lopenal -lalut -lcurl

scriptedit_SOURCES = src/scriptedit/scriptedit.cc src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/shared/cpphypioimp.cc src/shared/cpphypioimp.hh
scriptedit_CPPFLAGS = -DIOSTREAM

test_SOURCES = src/test/test.cc src/core/string.hh src/core/maze.hh src/core/script.hh src/core/dirns.hh src/core/maze.cc src/core/mazegen.hh src/core/SmartPointer.hh src/core/hypio.cc src/core/hypio.hh src/core/script.cc src/core/vector.hh src/core/scriptimpl.hh src/shared/cpphypioimp.cc src/shared/cpphypioimp.hh
test_CPPFLAGS = -DIOSTREAM

