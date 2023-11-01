CXX := g++
CXX_FLAGS := -g

SOURCES := $(wildcard *.cpp)
BINARIES := ch8

all: $(BINARIES)

ch8: $(SOURCES)
	$(CXX) $(CXX_FLAGS) -o $@ $(SOURCES)

clean: 
	rm -rf $(BINARIES)
