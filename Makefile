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
test.o: test.cc dirns.hh maze.hh vector.hh string.hh mazegen.hh
run-test: test
	./test

hypermaze: CPPLIBS+= -DIRRLICHT $(IRRLIBS)
hypermaze: hypermaze.o iMyCamera.o controller.o irrdisp.o
hypermaze.o: hypermaze.cc dirns.hh maze.hh vector.hh string.hh irrdisp.hh iMyCamera.hh gui.hh keymap.hh controller.hh
controller.o: controller.cc irrdisp.hh  keymapgui.hh helpgui.hh keymap.hh maze.hh string.hh dirns.hh vector.hh  mazegen.hh 
irrdisp.o: irrdisp.hh
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
