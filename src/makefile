# Source files.
SRC = $(wildcard *.cpp)

OBJ = $(SRC:.cpp=.o)

# Builds everything.
all: $(OBJ)
	$(CXX) $(OBJ) -fopenmp -o $(BINDIR)/$(EXEC)
	
# Builds object file from Cpp source file.
%.o: %.cpp
	$(CXX) $(CFLAGS) $< -c -o $@
	
# Cleans compilation files.
clean:
	@rm -f $(OBJ)
	@rm -f $(BINDIR)/$(EXEC)
