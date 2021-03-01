# Parameters

APPTEST     = wengine
CXX         = gcc
CXXFLAGS    = -Wall
LIBS        = -Iinc -lm -lncurses
OFILES 		= wengine.o \

# Build / Clean

all:
	make test

test: test.o $(OFILES)
	$(CXX) $(CXXFLAGS) -o bin/$(APPTEST) test.o $(OFILES) $(LIBS)

clean:
	rm *.o

# Source Files

test.o: test/test.c
	$(CXX) $(CXXFLAGS) -c test/test.c $(LIBS)

wengine.o: inc/wengine.h src/wengine.c
	$(CXX) $(CXXFLAGS) -c src/wengine.c $(LIBS)
