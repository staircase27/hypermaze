all: hypermaze
run: run-hypermaze

CPP=g++
CPPOPTS=-ggdb
CPPLIBS=

IRRLIBS=-lIrrlicht -I/usr/include/irrlicht/

test: test.o
test.o: test.cc dirns.hh maze.hh vector.hh string.hh mazegen.hh
run-test: test
	./test

hypermaze: CPPLIBS+= $(IRRLIBS)
hypermaze: hypermaze.o iMyCamera.o
hypermaze.o: hypermaze.cc dirns.hh maze.hh vector.hh string.hh mazegen.hh irrdisp.hh iMyCamera.hh gui.hh keymap.hh keymapgui.hh
iMyCamera.o: iMyCamera.cpp iMyCamera.hh
run-hypermaze: hypermaze
	./hypermaze

clean:
	rm -f *.o
	rm -f test
	rm -f irrtest

%: %.o
	$(CPP) $(CPPOPTS) -o $@ $^ $(CPPLIBS) 
%.o: %.cc
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
%.o: %.cpp
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
