# generic Makefile for C


### Main Settings
CC = gcc
CFLAGS = -Wall
LIBS =
LIBFLAGS = `pkg-config --cflags $(LIBS)`
LIBLINKS = `pkg-config --libs $(LIBS)`


PROGRAM = iterated_prisoner 
SRCS = iterated_prisoner.c strategies.c latex_output.c
OBJS = $(SRCS:.c=.o)

###

DEPFILE = .depend
DEPFLAGS = -MM


.PHONY: default
default: all

.PHONY: all
ifeq ($(wildcard $(DEPFILE)), )
all: $(DEPFILE)
	make --no-print-directory
else
include $(DEPFILE)

all: $(PROGRAM)
endif




$(PROGRAM) : $(OBJS)
	$(CC) $(CFLAGS) $(LIBFLAGS) $^ -o $@ $(LIBLINKS)

%.o : %.c
	$(CC) $(CFLAGS) $(LIBFLAGS) -c $< -o $@


$(DEPFILE) : $(SRCS)
	-rm $(DEPFILE)
	$(CC) $(DEPFLAGS) $^ >> $@



.PHONY: clean
clean:
	-rm $(PROGRAM) $(OBJS) $(DEPFILE)
