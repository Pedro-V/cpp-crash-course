CXX := g++
CXX_FLAGS := -Wall

CHAPTERS := $(wildcard ch*.cpp)
EXTRA_PARTS := dedication.cpp

SOURCES := $(CHAPTERS) $(EXTRA_PARTS)
BINARIES := $(SOURCES:.cpp=)

all: $(BINARIES)

%: %.cpp
	$(CXX) $(CXX_FLAGS) -o $@ $<

clean:
	rm -f $(BINARIES)
