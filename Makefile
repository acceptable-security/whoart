# Definitely not taken from StackOverflow

SOURCEDIR := src
BINARY := whoart
BUILDDIR := build
HEADERDIR := lib
CFLAGS := -g
LDFLAGS := -lm -lglfw  -lGL -lX11 -lpthread -lXrandr -lXi -lGLEW

SOURCES := $(shell find $(SOURCEDIR) -name '*.c')
OBJECTS := $(addprefix $(BUILDDIR)/, $(SOURCES:%.c=%.o))

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BINARY)

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(dir $<) -Isrc -c $< -o $@

clean:
	rm -f $(OBJECTS)
