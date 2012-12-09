all: irrtest
run: run-irrtest

CPP=g++
CPPOPTS=-ggdb
CPPLIBS=

IRRLIBS=-lIrrlicht -I/usr/include/irrlicht/

test: test.o
test.o: test.cc dirns.hh maze.hh vector.hh string.hh mazegen.hh
run-test: test
	./test

irrtest: CPPLIBS+= $(IRRLIBS)
irrtest: irrtest.o iMyCamera.o
irrtest.o: irrtest.cc dirns.hh maze.hh vector.hh string.hh mazegen.hh irrdisp.hh iMyCamera.hh gui.hh
iMyCamera.o: iMyCamera.cpp iMyCamera.hh
run-irrtest: irrtest
	./irrtest

clean:
	rm -f *.o
	rm -f test
	rm -f irrtest

%: %.o
	$(CPP) $(CPPOPTS) $(CPPLIBS) -o $@ $^
%.o: %.cc
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
%.o: %.cpp
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
