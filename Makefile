IDIR =include# directory for header files
SDIR =src# source directory
ODIR =obj# object file directory
CPPC =g++# compiler used
CFLAGS =-std=c++11 -Wall -Wshadow -Werror -I$(IDIR)# compiler flags
LDFLAGS =# -lm library flags
TARGET = adtar # file executable generated

# Getting the list of all c and object files
SOURCES := $(wildcard $(SDIR)/*.cpp)
OBJECTS := $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SOURCES))

# TO manually list the header files
# _DEPS = common.h # dependency header files

# Getting the list of header files
HEADERS = $(wildcard $(IDIR)/*.h)

# To print the value of a variable
# $(info VAR is $(HEADERS))

# Pattern substitution
# $(patsubst pattern, substitution, text_to_insert)
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

all : $(TARGET)

$(ODIR)/%.o: $(SDIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CPPC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJECTS)
	$(CPPC) -o $@ $^ $(CFLAGS)

# Any file with the name clean will not interrupt the cmd clean
.PHONY: clean

clean: clean-obj clean-build clean-mac-fsys clean-ad

clean-obj:
	rm -rf $(ODIR)

clean-mac-fsys:
	rm -rf *.DS_Store

clean-build:
	rm -rf $(TARGET)

clean-ad:
	rm *.ad
