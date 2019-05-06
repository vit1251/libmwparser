all: compile

depend:
	apt install flex
	apt install bison

clean:

compile:
	meson builddir
	ninja -C builddir

check:

.PHONY: all depend clean compile check
