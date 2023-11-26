CXX := g++
CXX_FLAGS := -g

SOURCES := $(wildcard *.cpp)
BINARIES := $(SOURCES:.cpp=)

all: $(BINARIES)

%: %.cpp
	$(CXX) $(CXX_FLAGS) -o $@ $<

clean:
	rm -f $(BINARIES) comparison *.s *.o *.out
