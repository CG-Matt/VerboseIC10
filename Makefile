.PHONY: main clean

CXX			= g++
CXXFLAGS	= -Wall -Wextra -O2 -std=c++17 -Iinclude
LDFLAGS		=

SRCDIR		= src
INCDIR		= include
BLDDIR		= build

SRC			= $(wildcard $(SRCDIR)/*.cpp)
OBJ			= $(patsubst $(SRCDIR)/%.cpp,$(BLDDIR)/%.o,$(SRC))
INC			= $(wildcard $(INCDIR)/*.hpp) $(wildcard $(SRCDIR)/*.hpp)

BLDNAME		= victc

main: $(BLDDIR)/$(BLDNAME)

$(BLDDIR)/$(BLDNAME): $(OBJ) | $(BLDDIR)
	$(CXX) $(LDFLAGS) -s -o $@ $(OBJ)

$(BLDDIR)/%.o: $(SRCDIR)/%.cpp $(INC) | $(BLDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BLDDIR):
	mkdir -p $(BLDDIR)

clean:
	rm -rf $(BLDDIR)