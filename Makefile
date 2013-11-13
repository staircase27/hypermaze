BUILDDIR=build
DISTDIR=dist

all: hypermaze
run: run-hypermaze

hypermaze.zip: dist
	zip hypermaze.zip dist/* 
dist: dist/hypermaze dist/irrlicht dist/hypermaze.keymap.conf

dist/%:%
	cp -r $^ $@

CPP=g++
CPPOPTS=-ggdb
CPPLIBS=

IRRLIBS=-lIrrlicht -isystem/usr/include/irrlicht/

test: CPPOPTS+= -DIOSTREAM
test: test.o maze.o
test.o: test.cc maze.hh dirns.hh vector.hh string.hh mazegen.hh
run-test: test
	./test

hypermaze: CPPOPTS+= -DIRRLICHT -DOPENAL -DIOSTREAM
hypermaze: CPPLIBS+= $(IRRLIBS) -lopenal -lalut
hypermaze: hypermaze.o iMyCamera.o controller.o irrdisp.o maze.o keymap.o GUIFormattedText.o sound.o script.o

hypermaze.o: hypermaze.cc irrdisp.hh maze.hh dirns.hh vector.hh string.hh \
 iMyCamera.hh keymap.hh controller.hh irrio.hh script.hh scriptimpl.hh SmartPointer.hh

controller.o: controller.cc controller.hh string.hh maze.hh dirns.hh \
 vector.hh keymap.hh irrdisp.hh gui.hh mazegen.hh helpgui.hh keymapgui.hh GUIFormattedText.hh  irrio.hh SmartPointer.hh

irrdisp.o: irrdisp.cc irrdisp.hh maze.hh dirns.hh vector.hh string.hh \
 controller.hh keymap.hh irrio.hh SmartPointer.hh

maze.o: maze.cc maze.hh dirns.hh vector.hh SmartPointer.hh

keymap.o: keymap.cc keymap.hh dirns.hh vector.hh irrio.hh

iMyCamera.o: iMyCamera.cpp iMyCamera.hh

GUIFormatedText.o: GUIFormattedText.cc GUIFormatedText.hh

irrcurl.o: irrcurl.cc irrcurl.hh

sound.o: sound.cc sound.hh

script.o: script.cc script.hh scriptimpl.hh SmartPointer.hh

run-hypermaze: hypermaze
	./hypermaze

clean:
	rm -f *.o
	rm -f test
	rm -f hypermaze
	rm -rf dist/*

%: %.o
	$(CPP) $(CPPOPTS) -o $@ $^ $(CPPLIBS) 
%.o: %.cc
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
%.o: %.cpp
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
