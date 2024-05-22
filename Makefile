# Compiler settings
CXX := g++ -Wall -std=c++11
# Copy for backup
CP := cp -t
# Deletion for clean
RM := rm -f

# Directory initialization
SRCDIR := ./src/cpp/
BINDIR := ./bin/
SAVEDIR := ./save/

# File types to clean
CLEANEXTS := *.o *~

# File definitions
SRC := $(wildcard $(SRCDIR)*.cpp)                        # All cpp files in the SRC directory
OBJ := $(SRC:$(SRCDIR)%.cpp=$(BINDIR)%.o)               # Object files to be compiled
HEAD := $(wildcard $(SRCDIR)*.hpp)                       # Header files
PROG := $(BINDIR)delivery_optimizer                     # The executable program

# Default target
all: $(PROG)

# Linking the program
$(PROG): $(OBJ)
	$(CXX) $^ -o $@

# Compiling source files into object files
$(BINDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) -c $< -o $@

# Phony targets for cleanliness and backups
.PHONY: clean save restore

# Clean the object files and the executable
clean:
	$(RM) $(OBJ) $(PROG)
	$(RM) $(CLEANEXTS)
	rm -f bin/*

# Save the source and header files
save:
	$(CP) $(SAVEDIR) $(SRC) $(HEAD)

# Restore files from backup
restore:
	$(CP) $(SRCDIR) $(SAVEDIR)*

