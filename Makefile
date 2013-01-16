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
test: test.o
test.o: test.cc maze.hh dirns.hh vector.hh string.hh mazegen.hh
run-test: test
	./test

hypermaze: CPPOPTS+= -DIRRLICHT -DIOSTREAM
hypermaze: CPPLIBS+= $(IRRLIBS)
hypermaze: hypermaze.o iMyCamera.o controller.o irrdisp.o maze.o keymap.o GUIFormattedText.o

hypermaze.o: hypermaze.cc irrdisp.hh maze.hh dirns.hh vector.hh string.hh \
 iMyCamera.hh keymap.hh controller.hh

controller.o: controller.cc controller.hh string.hh maze.hh dirns.hh \
 vector.hh keymap.hh irrdisp.hh gui.hh mazegen.hh helpgui.hh keymapgui.hh GUIFormattedText.hh

irrdisp.o: irrdisp.cc irrdisp.hh maze.hh dirns.hh vector.hh string.hh \
 controller.hh keymap.hh

maze.o: maze.cc maze.hh dirns.hh vector.hh

keymap.o: keymap.cc keymap.hh dirns.hh vector.hh

iMyCamera.o: iMyCamera.cpp iMyCamera.hh

GUIFormatedText.o: GUIFormattedText.cc GUIFormatedText.hh

run-hypermaze: hypermaze
	./hypermaze

clean:
	rm -f *.o
	rm -f test
	rm -f irrtest
	rm -rf dist/*

%: %.o
	$(CPP) $(CPPOPTS) -o $@ $^ $(CPPLIBS) 
%.o: %.cc
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
%.o: %.cpp
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
