# generic Makefile for C

.PHONY: default
default: all

### Main Settings
CC = gcc
CFLAGS = -Wall -pthread

PROGRAMS = iterated_prisoner populations town monitor
SRCSFILE = iterated_prisoner.c strategies.c latex_output.c utils.c \
		   population.c populations_simulation.c town.c town_simulation.c \
		   network.c args.c monitor.c neighbours.c

iterated_prisoner : $(addprefix $(OBJSDIR),iterated_prisoner.o strategies.o \
					  latex_output.o utils.o)

populations : $(addprefix $(OBJSDIR),populations_simulation.o strategies.o \
				population.o utils.o latex_output.o)

town : $(addprefix $(OBJSDIR),town_simulation.o town.o network.o utils.o \
		 strategies.o args.o network.o neighbours.o)

monitor : $(addprefix $(OBJSDIR), monitor.o network.o strategies.o utils.o)

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
