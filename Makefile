# This file is part of rSON
# Copyright © 2012-2013 Rachel Mant (dx-mon@users.sourceforge.net)
#
# rSON is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# rSON is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

include Makefile.inc

CFLAGS = -c $(OPTIM_FLAGS) -pedantic -Wall -Wextra -std=c++11 -D__rSON__ -o $@ $<
LFLAGS = -shared $(O) -Wl,-soname,$(SOMAJ) -o $(SO) -lstdc++ -lm -z defs

SED = sed -e 's:@LIBDIR@:$(LIBDIR):g' -e 's:@PREFIX@:$(PREFIX):g'

LIBDIR ?= $(PREFIX)/lib
PKGDIR = $(LIBDIR)/pkgconfig/
INCDIR = $(PREFIX)/include/

H = rSON.h
O = JSONErrors.o JSONAtom.o JSONNull.o JSONBool.o JSONInt.o JSONFloat.o JSONString.o JSONObject.o JSONArray.o Memory.o String.o Parser.o Writer.o
VERMAJ = .0
VERMIN = .0
VERREV = .1
VER = $(VERMAJ)$(VERMIN)$(VERREV)
SOMAJ = librSON.so
SOMIN = $(SOMAJ)$(VERMAJ)
SOREV = $(SOMIN)$(VERMIN)
SO = $(SOREV)$(VERREV)
A = librSON.a
PC = rSON.pc
IN = rSON.pc.in

default: all

all: $(SO) $(PC)

$(LIBDIR):
	$(call run-cmd,install_dir,$(LIBDIR))

$(PKGDIR):
	$(call run-cmd,install_dir,$(PKGDIR))

$(INCDIR):
	$(call run-cmd,install_dir,$(INCDIR))

install: all $(LIBDIR) $(PKGDIR) $(INCDIR)
	$(call run-cmd,install_file,$(SO),$(LIBDIR))
	$(call run-cmd,install_file,$(PC),$(PKGDIR))
	$(call run-cmd,install_file,$(H),$(INCDIR))
	$(call run-cmd,ln,$(LIBDIR)/$(SO),$(LIBDIR)/$(SOREV))
	$(call run-cmd,ln,$(LIBDIR)/$(SOREV),$(LIBDIR)/$(SOMIN))
	$(call run-cmd,ln,$(LIBDIR)/$(SOMIN),$(LIBDIR)/$(SOMAJ))
	$(call ldconfig)

uninstall:
	rm $(LIBDIR)/$(SOMAJ)*
	rm $(LIBDIR)/$(A)
	rm $(PKGDIR)/$(PC)

$(SO): $(O)
	$(call run-cmd,ar,$(A),$(O))
	$(call run-cmd,ranlib,$(A))
	$(call run-cmd,ccld,$(LFLAGS))
	$(call debug-strip,$(SO))

$(PC): $(IN)
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
