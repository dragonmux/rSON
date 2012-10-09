include Makefile.inc

PKG_CONFIG_PKGS = 
EXTRA_DEFINE = -D__rSON__
EXTRA_CFLAGS = 
#$(shell pkg-config --cflags $(PKG_CONFIG_PKGS))
CFLAGS = -c $(OPTIM_FLAGS) -pedantic -Wall $(EXTRA_CFLAGS) $(EXTRA_DEFINE) -o $@ $<
EXTRA_LIBS = $(shell pkg-config --libs $(PKG_CONFIG_PKGS))
LIBS = 
#$(EXTRA_LIBS)
LFLAGS = -shared $(O) $(LIBS) -Wl,-soname,$(SOMAJ) -o $(SO)

SED = sed -e 's:@LIBDIR@:$(LIBDIR):g'

LIBDIR ?= /usr/lib
PKGDIR = $(LIBDIR)/pkgconfig/
INCDIR = /usr/include/

H = rSON.h
O = JSONErrors.o JSONAtom.o JSONNull.o JSONBool.o JSONInt.o JSONFloat.o JSONString.o JSONObject.o JSONArray.o Memory.o String.o Parser.o
VERMAJ = .0
VERMIN = .0
VERREV = .1
VER = $(VERMAJ)$(VERMIN)$(VERREV)
SOMAJ = librSON.so
SOMIN = $(SOMAJ)$(VERMAJ)
SOREV = $(SOMIN)$(VERMIN)
SO = $(SOREV)$(VERREV)
A = librSON.a
PC = librSON.pc
IN = librSON.pc.in

default: all

all: $(SO)
# $(PC)

install: all
	$(call run-cmd,install_file,$(SO),$(LIBDIR))
	$(call run-cmd,install_file,$(PC),$(PKGDIR))
	$(call run-cmd,install_file,$(H),$(PKGDIR))
	$(call run-cmd,ln,$(LIBDIR)/$(SO),$(LIBDIR)/$(SOREV))
	$(call run-cmd,ln,$(LIBDIR)/$(SOREV),$(LIBDIR)/$(SOMIN))
	$(call run-cmd,ln,$(LIBDIR)/$(SOMIN),$(LIBDIR)/$(SOMAJ))
	$(call run-cmd,ldconfig)

uninstall:
	rm $(LIBDIR)/$(SOMAJ)*
	rm $(LIBDIR)/$(A)
	rm $(PKGDIR)/$(PC)

$(SO): $(O)
	$(call run-cmd,ar,$(A),$(O))
	$(call run-cmd,ranlib,$(A))
	$(call run-cmd,ccld,$(LFLAGS))
	$(call debug-strip,$(SO))

libTest.pc: $(IN)
	$(call run-cmd,sed,$(IN),$(PC))

clean: test
	@cd test && $(MAKE) clean
	$(call run-cmd,rm,rSON,*.o $(SOMAJ)* $(A) $(PC))

tests: all test
	@cd test && $(MAKE)

check: all test
	@cd test && $(MAKE) check

.cpp.o:
	$(call run-cmd,cxx,$(CFLAGS))

.PHONY: clean librSON.pc install all uninstall default .cpp.o tests check
