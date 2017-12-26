# generic Makefile for C

.PHONY: default
default: all

### Main Settings
CC = gcc
CFLAGS = -Wall

PROGRAMS = iterated_prisoner
SRCSFILE = iterated_prisoner.c strategies.c latex_output.c

iterated_prisoner : $(addprefix $(OBJSDIR),iterated_prisoner.o strategies.o latex_output.o)

###

SRCSDIR = src/
SRCS = $(addprefix $(SRCSDIR),$(SRCSFILE))
OBJSDIR = .objs/
OBJSFILE = $(SRCSFILE:.c=.o)
OBJS = $(addprefix $(OBJSDIR),$(OBJSFILE))

VPATH = $(SRCSDIR):$(OBJSDIR)

DEPFILE = .depend
DEPFLAGS = -MM


.PHONY: all
ifeq ($(wildcard $(DEPFILE)), )
all: $(DEPFILE)
	make --no-print-directory
else
include $(DEPFILE)

all: $(PROGRAMS)
endif




$(OBJS) : | $(OBJSDIR)

$(OBJSDIR) :
	mkdir $(OBJSDIR)

$(PROGRAMS) :
	$(CC) $(CFLAGS) $(LIBFLAGS) $^ -o $@ $(LIBLINKS)

$(OBJSDIR)%.o : %.c
	$(CC) $(CFLAGS) $(LIBFLAGS) -c $< -o $@


$(DEPFILE) : $(SRCS)
	-rm $(DEPFILE)
	$(CC) $(DEPFLAGS) $^ >> $@



.PHONY: clean
clean:
	-rm $(PROGRAMS) $(OBJS) $(DEPFILE)
	-rmdir $(OBJSDIR)
