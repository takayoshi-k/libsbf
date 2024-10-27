TARGETDIR = ../bin

CSRCS = sbfbind.c sbfunbind.c sbfutil.c sbfcmd_main.c
CHDRS = sbfcmd.h sbfutil.h

CFLAGS = -I ../include -L ../lib

all: create_dir sbfbind sbfunbind

sbfbind: $(CHDRS) $(CSRCS)
	gcc $(CFLAGS) -o $(TARGETDIR)/sbfbind $(CSRCS) -lsbf

sbfunbind: $(CHDRS) $(CSRCS)
	gcc $(CFLAGS) -o $(TARGETDIR)/sbfunbind $(CSRCS) -lsbf

create_dir:
	mkdir -p $(TARGETDIR)

clean:
	rm -rf $(TARGETDIR)

