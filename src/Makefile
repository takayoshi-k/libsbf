TARGETDIR = ../lib
TARGET = $(TARGETDIR)/libsbf.a

CC = gcc
AR = ar

INCDIR = -I ../include

CSRCS = libsbf.c
CHDRS = ../include/libsbf.h
CFLAGS = $(INCDIR)

COBJS = $(CSRCS:.c=.o)

all: create_dir $(TARGET)

$(COBJS): %.o: %.c $(CHDRS)
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(COBJS)
	$(AR) r $@ $(COBJS)

create_dir:
	mkdir -p $(TARGETDIR)

clean:
	rm -fr $(TARGETDIR)
	rm -f $(COBJS)

