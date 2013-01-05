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

test: test.o
test.o: test.cc maze.hh dirns.hh vector.hh string.hh mazegen.hh
run-test: test
	./test

hypermaze: CPPLIBS+= -DIRRLICHT $(IRRLIBS)
hypermaze: hypermaze.o iMyCamera.o controller.o irrdisp.o maze.o keymap.o

hypermaze.o: hypermaze.cc irrdisp.hh maze.hh dirns.hh vector.hh string.hh \
 iMyCamera.hh keymap.hh controller.hh

controller.o: controller.cc controller.hh string.hh maze.hh dirns.hh \
 vector.hh keymap.hh irrdisp.hh gui.hh mazegen.hh helpgui.hh keymapgui.hh

irrdisp.o: irrdisp.cc irrdisp.hh maze.hh dirns.hh vector.hh string.hh \
 controller.hh keymap.hh

maze.o: maze.cc maze.hh dirns.hh vector.hh

keymap.o: keymap.cc keymap.hh dirns.hh vector.hh

iMyCamera.o: iMyCamera.cpp iMyCamera.hh

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
