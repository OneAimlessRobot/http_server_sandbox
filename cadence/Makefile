

LDLIBS=-lSDL2 -lpthread -lm


INCLUDE= ./Includes .
BINARY=cadence.exe
SOURCES= ./Sources .

CURRDIR=echo `pwd`

CXX= g++
DEPFLAGS= -MP -MD

CXXFLAGS= -Wall -DPROGRAMPATHAUX="$(CURRDIR)" -std=gnu++11 -Wextra -g $(foreach D, $(INCLUDE), -I$(D)) $(DEPFLAGS) `sdl2-config --libs`

SOURCEFILES=$(foreach D,$(SOURCES), $(wildcard $(D)/*.cpp))

OBJECTS=$(patsubst %.cpp,%.o,$(SOURCEFILES))

DEPFILES= $(patsubst %.cpp,%.d,$(SOURCEFILES))


all: $(BINARY)
	echo $(LDLIBS)
	echo $(CURRDIR)


$(BINARY): $(OBJECTS)
	$(CXX) -g  $(CXXFLAGS) -o $@ $^ $(LDLIBS)


%.o:%.c
	$(CXX)  $(CXXFLAGS) -c -o $@ $<  $(LDLIBS)
	echo $(CXXFLAGS)
clean:
	rm -rf $(BINARY) $(OBJECTS) $(DEPFILES)
