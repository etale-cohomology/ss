# -----------------------------------------------------------------------------------------------------------------------------#
CC      = gcc-8
WATCH   = makefile  ${mathisart}/mathisart.h ${mathisart}/mathisart.c
INSTALL = ~/bin

all: ssr ssw ssmeta

clean: ssr ssw ssmeta

install:
	cp ssr    $(INSTALL)
	cp ssw    $(INSTALL)
	cp ssmeta $(INSTALL)

uninstall:
	rm -f $(INSTALL)/ssr
	rm -f $(INSTALL)/ssw
	rm -f $(INSTALL)/ssmeta

# -----------------------------------------------------------------------------------------------------------------------------#
ssr: ssr.c ss.h  $(WATCH)
	t $(CC)  $< -o $@  ${cflags} ${cnopie} ${cfast}  -lxcb -lxcb-shm  -l:libblosc.a -lpthread
ssr_debug: ssr.c ss.h  $(WATCH)
	t $(CC)  $< -o ssr  -lxcb -lxcb-shm  -l:libblosc.a -lpthread

ssw: ssw.c ss.h  $(WATCH)
	t $(CC)  $< -o $@  ${cflags} ${cnopie} ${cfast}  -lxcb -lxcb-shm  -l:libblosc.a -lpthread

ssmeta: ssmeta.c ss.h  $(WATCH)
	t $(CC)  $< -o $@  ${cflags} ${cnopie} ${cfast}  -l:libblosc.a -lpthread
