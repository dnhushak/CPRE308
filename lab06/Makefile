#AppServer makefile
CXX = gcc
LDFLAGS = -Wall
CFLAGS = -g -O3

#File Extensions
HEADER = .h
SOURCE = .c
BINARY = .o

#Directories
SRCEDIR = ./
DIRLIST = $(SRCEDIR)

#Final Executable Name
EXE = memory

# Generate list of source headers with extensions
HEADERS = $(foreach DIR, $(DIRLIST), $(wildcard $(DIR)*$(HEADER)))
SOURCES = $(foreach DIR, $(DIRLIST), $(wildcard $(DIR)*$(SOURCE)))
OBJECTS = $(addsuffix $(BINARY), $(notdir $(basename $(SOURCES))))
INCLUDE = $(foreach DIR, $(DIRLIST), -I$(DIR))

$(EXE): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $(EXE) $(LDFLAGS)

%$(BINARY) : $(SRCEDIR)%$(SOURCE)
	$(CXX) $(CFLAGS) $(INCLUDE) -c $< $(LDFLAGS) -o $@

clean:
	rm -rf $(EXE) *.o
	
depend:
	gccmakedep -- $(LDFLAGS3) -- $(SOURCES)