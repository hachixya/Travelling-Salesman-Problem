# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -shared -fPIC $(shell python3 -m pybind11 --includes) $(shell python3-config --includes --ldflags)
CXXFLAGS_EXE = -std=c++17 -O3 -Wall

# Directories
SRCDIR = ./src/cpp/
INCLUDEDIR = ./src/cpp/include/
PYBINDIR = ./src/cpp/
BINDIR = ./bin/
SAVEDIR = ./save/

# File types to clean
CLEANEXTS = *.o *~

# File definitions
SRC_CPP = $(SRCDIR)main.cpp $(SRCDIR)City.cpp $(SRCDIR)TSP.cpp $(SRCDIR)Tree.cpp
SRC_PYBIND = $(SRCDIR)City.cpp $(SRCDIR)TSP.cpp $(SRCDIR)Tree.cpp $(PYBINDIR)delivery_optimizer_pybind.cpp
HEAD = $(wildcard $(INCLUDEDIR)*.hpp)
TARGET = $(BINDIR)delivery_optimizer$(shell python3-config --extension-suffix)
EXE = $(BINDIR)delivery_optimizer_exe

# Default target
all: $(TARGET) $(EXE)

# Building the pybind module
$(TARGET): $(SRC_PYBIND) $(HEAD)
	$(CXX) $(CXXFLAGS) $(SRC_PYBIND) -I$(INCLUDEDIR) -o $(TARGET)

# Building the executable
$(EXE): $(SRC_CPP) $(HEAD)
	$(CXX) $(CXXFLAGS_EXE) $(SRC_CPP) -I$(INCLUDEDIR) -o $(EXE)

# Phony targets for cleanliness and backups
.PHONY: clean save restore test

# Clean the object files and the executable
clean:
	rm -f $(TARGET) $(EXE) $(CLEANEXTS) $(BINDIR)/*
	rm -rf __pycache__ .pytest_cache 


# Save the source and header files
save:
	cp $(SRC_CPP) $(SRC_PYBIND) $(HEAD) $(SAVEDIR)

# Restore files from backup
restore:
	cp $(SAVEDIR)* $(SRCDIR)

# Run tests
test: $(TARGET)
	PYTHONPATH=$(BINDIR) python3 -m pytest

# Run python interface
run: $(TARGET)
	PYTHONPATH=$(BINDIR) python3 src/python/interface.py
	firefox Optimized_Route.html
