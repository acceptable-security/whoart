# Definitely not taken from StackOverflow

SOURCEDIR := src
BINARY := whoart
BUILDDIR := build
HEADERDIR := lib

SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
OBJECTS := $(addprefix $(BUILDDIR)/, $(SOURCES:%.c=%.o))

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BINARY)

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(dir $<) -Isrc -c $< -o $@
