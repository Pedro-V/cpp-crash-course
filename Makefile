CXX := g++
CXX_FLAGS := -g

CHAPTERS := $(wildcard ch*.cpp)
EXTRA_PARTS := dedication.cpp

SOURCES := $(CHAPTERS) $(EXTRA_PARTS)
BINARIES := $(SOURCES:.cpp=)

all: $(BINARIES) comparison

%: %.cpp
	$(CXX) $(CXX_FLAGS) -o $@ $<

comparison: comparison.c
	gcc -g $^ -o $@

clean:
	rm -f $(BINARIES) comparison *.s *.o *.out
