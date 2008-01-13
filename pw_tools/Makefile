# Generel makefile for Linux with gcc
# Builds all or specific tools
# No extra dependencies are needed

#################################

CPP = g++
OFLAG = -o
CFLAG = -c
OSUFFIX = .o
EXT = 
CPPFLAGS =
LIBS = 
DEL = rm -f

#################################

all: \
	block_extract

#################################
# block_extract tool
#################################

block_extract: block_extract.o
	$(CPP) block_extract.o $(LIBS) $(OFLAG) block_extract

block_extract.o: block_extract.cpp common.h
	$(CPP) $(CFLAG) block_extract.cpp $(OFLAG) block_extract.o

#################################

# clean up the build directory
clean:
	$(DEL) *$(OSUFFIX)
	$(DEL) block_extract

