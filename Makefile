-include config.mk

PROXY        = ver-ix86-1
LDFLAGS      = -mdll -Wl,--enable-stdcall-fixup -Wl,--strip-all -Wl,--exclude-all-symbols -Wl,--gc-sections -fno-asynchronous-unwind-tables
LDFLAGSSYM      = -mdll -Wl,--enable-stdcall-fixup
NFLAGS       = -f elf -Iinc/
CC            = gcc
CFLAGS       = -std=c99 -Iinc/
CPPFLAGS       = -masm=intel -I./ -Iinc/ -Os -mtune=i586

OBJS = w2p.o checkrevision.o dllmain.o ezini.o
       
NASM        ?= nasm

.PHONY: default
default: $(PROXY).mpq
	mpqsigner.exe $(PROXY).dll

$(PROXY).mpq: $(PROXY).dll

%.o: %.asm
	$(NASM) $(NFLAGS) -o $@ $<

$(PROXY).dll: $(OBJS) modexp.def
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lstdc++ -static

clean:
	$(RM) $(OBJS) $(PROXY).dll $(PROXY).mpq
cleanmpq:
	$(RM) $(PROXY).mpq
