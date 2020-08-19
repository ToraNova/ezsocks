#################################################################
#
# easysocks - if using sockets is as easy as putting up socks.
# a mini project by toranova
# to allow easy use to tcp/udp sockets (including tls)
#
#################################################################

SRCDIR   = .
OBJDIR   = .
SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJECTFULL = $(OBJDIR)/*.o

TESTDIR = tests
TESTS := $(wildcard $(TESTDIR)/*.c)
TESTP := $(TESTS:$(TESTDIR)/%.c=$(TESTDIR)/%.testbin)

CC = gcc
CLIBS =
CFLAGS= -O3 -Wall -march=native

.PHONY: all clean test


all: test
	@echo 'ezsock make all done.'
	@ls $(TESTDIR)

test: $(TESTP)
	@echo 'ezsock make test done.'

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(TESTDIR)/*.testbin

$(TESTP) : $(TESTDIR)/%.testbin : $(TESTDIR)/%.c $(OBJECTS)
	$(CC) $^ -o $@ $(CLIBS) $(CFLAGS)

# compile objects
$(OBJECTS) : $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) -c $< -o $@ $(CLIBS) $(CFLAGS)
