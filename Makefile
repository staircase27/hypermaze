all: irrtest
run: run-irrtest

CPP=g++
CPPOPTS=-ggdb
CPPLIB=

test: test.o
test.o: test.cc dirns.hh maze.hh vector.hh string.hh mazegen.hh
run-test: test
	./test

CPPLIBS=-lIrrlicht -I/usr/include/irrlicht/

irrtest: irrtest.o iMyCamera.o
irrtest.o: irrtest.cc dirns.hh maze.hh vector.hh string.hh mazegen.hh gui.hh iMyCamera.hh
iMyCamera.o: iMyCamera.cpp iMyCamera.hh
run-irrtest: irrtest
	./irrtest

clean:
	rm -f *.o
	rm -f test

%: %.o
	$(CPP) $(CPPOPTS) $(CPPLIBS) -o $@ $^
	
%.o: %.cc
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
%.o: %.cpp
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
