# Directories.
export BINDIR    = $(CURDIR)/bin
export SRCDIR    = $(CURDIR)/src

# Executable name.
export EXEC = geneticProgramming

# Toolchain.
export CXX=g++ -std=c++11 -g

# Toolchain configuration. -std=c99  
CXXFLAGS = -I -ansi -pedantic -Wall -Wextra -Werror
CXXFLAGS += -D NDEBUG -O3

# Builds everything.
all:
	mkdir -p $(BINDIR)
	cd $(SRCDIR) && $(MAKE) all

# Cleans compilation files.
clean:
	$(MAKE) clean
