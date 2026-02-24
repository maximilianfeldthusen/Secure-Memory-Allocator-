
The Makefile works with GNU make on Linux/macOS and with MinGW/MSYS on Windows.
 It produces:
libsecurealloc.a – static library (POSIX) or securealloc.lib (Windows)
tests – test executable
#--------------------------------------------------------------
# Makefile – secure memory allocator (GPL‑v3)
#--------------------------------------------------------------

CC      ?= gcc
CFLAGS  := -Wall -Wextra -pedantic -std=c11 -O2 -fPIC
LDFLAGS :=
AR      := ar
ARFLAGS := rcs

# Detect Windows (mingw) vs POSIX
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PLATFORM = POSIX
else ifeq ($(UNAME_S),Darwin)
    PLATFORM = POSIX
else
    PLATFORM = MINGW
endif

#-----------------------------------------------------------------
SRC     := secure_alloc.c
OBJ     := $(SRC:.c=.o)

TEST_SRC := tests.c unity.c
TEST_OBJ := $(TEST_SRC:.c=.o)

LIBNAME_POSIX := libsecurealloc.a
LIBNAME_WIN   := securealloc.lib

#-----------------------------------------------------------------
.PHONY: all clean test install uninstall

all: $(LIBNAME)

$(LIBNAME): $(OBJ)
ifeq ($(PLATFORM),POSIX)
	$(AR) $(ARFLAGS) $@ $^
else
	$(AR) $(ARFLAGS) $@ $^
endif

%.o: %.c secure_alloc.h
	$(CC) $(CFLAGS) -c $< -o $@

#-----------------------------------------------------------------
# Test executable
test: $(LIBNAME) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ) -L. -lsecurealloc

#-----------------------------------------------------------------
install: $(LIBNAME)
	@mkdir -p $(DESTDIR)/usr/local/lib
	@mkdir -p $(DESTDIR)/usr/local/include
	install -m 0644 $(LIBNAME) $(DESTDIR)/usr/local/lib/
	install -m 0644 secure_alloc.h $(DESTDIR)/usr/local/include/

uninstall:
	rm -f $(DESTDIR)/usr/local/lib/$(LIBNAME)
	rm -f $(DESTDIR)/usr/local/include/secure_alloc.h

clean:
	rm -f *.o $(LIBNAME) test

# Platform‑specific library name
ifeq ($(PLATFORM),POSIX)
LIBNAME = $(LIBNAME_POSIX)
else
LIBNAME = $(LIBNAME_WIN)
endif
Building on POSIX (Linux/macOS)
$ make          # builds libsecurealloc.a
$ ./test        # runs the unit tests (compiled automatically)
$ sudo make install   # installs to /usr/local/{lib,include}
Building on Windows (MinGW/MSYS)
> mingw32-make         # produces securealloc.lib
> .\test.exe           # run the test binary
> mingw32-make install DESTDIR=C:\Program Files\SecureAlloc
If you use Visual Studio instead of MinGW, replace the CC, AR, 
and flags with cl/link equivalents – 
the source files are pure C and require only kernel32.lib for the Windows APIs.

