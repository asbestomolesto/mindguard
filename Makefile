# Makefile for MindGuard v 0.0.0.5

BINDIR = /usr/local/bin
MGDIR = /usr/local/share/MindGuard
CC = gcc
CFLAGS = -pedantic -Wall -O0 -g -lm

mindguard: mindguard.c mindguard.h 
	$(CC) mindguard.c -o mindguard `pkg-config --cflags gtk+-2.0` -DGTK_ENABLE_BROKEN `pkg-config --libs gtk+-2.0` $(CFLAGS)

#static: mindguard.c mindguard.h 
#	$(CC) -static mindguard.c -o mindguard `pkg-config --cflags gtk+-2.0` -DGTK_ENABLE_BROKEN `pkg-config --libs gtk+-2.0` $(CFLAGS)

install: mindguard install-bin install-carriers install-docs

install-nodocs: mindguard install-bin install-carriers

install-bin: mindguard
	mkdir -p $(BINDIR)
	install -c -s -m 0755 mindguard $(BINDIR)

install-carriers: carriers/
	mkdir -p $(MGDIR)/carriers
	install -c -m 0644 carriers/* $(MGDIR)/carriers

install-docs: docs/
	mkdir -p $(MGDIR)/docs
	install -c -m 0644 docs/* $(MGDIR)/docs

# Don't call this as root unless you want it in /root

install-home: install-bin-home install-carriers-home install-docs-home

install-home-dataonly: install-carriers-home install-docs-home

install-bin-home: mindguard
	mkdir -p $(HOME)/MindGuard/
	install -c -s -m 0755 mindguard $(HOME)/MindGuard/

install-carriers-home: carriers/
	mkdir -p $(HOME)/MindGuard/carriers
	install -c -m 0644 carriers/* $(HOME)/MindGuard/carriers

install-docs-home: docs/
	mkdir -p $(HOME)/MindGuard/docs
	install -c -m 0644 docs/* $(HOME)/MindGuard/docs

remove:
	rm -r $(MGDIR)
	rm $(BINDIR)/mindguard
	rm $(HOME)/MindGuard

clean:
	rm -f mindguard
