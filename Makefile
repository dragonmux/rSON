# This file is part of rSON
# Copyright © 2012-2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

DEFS = $(OPTIM_FLAGS) -pedantic -Wall -Wextra -std=c++11 -D__rSON__
CFLAGS = -c $(DEFS) -o $@ $<
DEPFLAGS = -E -MM $(DEFS) -o .dep/$*.d $<
LFLAGS = $(OPTIM_FLAGS) -shared $^ -Wl,-soname,$@ -o $@ -lstdc++ -lm -z defs
ifeq ($(strip $(FOR_TESTS)), 1)
	CFLAGS += $(shell pkg-config --cflags crunch)
endif

SED = sed -e 's:@LIBDIR@:$(LIBDIR):g' -e 's:@PREFIX@:$(PREFIX):g' -e 's:@VERSION@:$(VER):g'

LIBDIR ?= $(PREFIX)/lib
PKGDIR = $(LIBDIR)/pkgconfig/
INCDIR = $(PREFIX)/include/

H = rSON.h rSON_socket.h
O = JSONErrors.o JSONAtom.o JSONNull.o JSONBool.o JSONInt.o JSONFloat.o JSONString.o JSONObject.o JSONArray.o String.o Stream.o Parser.o Writer.o
O_SOCK = rSON_socket.o
VERMAJ = .0
VERMIN = $(VERMAJ).2
VERREV = $(VERMIN).2
VER = $(VERREV)
SO = librSON.so librSON.socket.so
PC = rSON.pc rSON_socket.pc

DEPS = .dep

# The last line of this is a magic new-line that makes the $(foreach) below work.
define ln-so
	$(call run-cmd,ln,$(LIBDIR)/$(1)$(VERREV),$(LIBDIR)/$(1)$(VERMIN))
	$(call run-cmd,ln,$(LIBDIR)/$(1)$(VERMIN),$(LIBDIR)/$(1)$(VERMAJ))
	$(call run-cmd,ln,$(LIBDIR)/$(1)$(VERMAJ),$(LIBDIR)/$(1))

endef

default: all

all: $(DEPS) $(SO)

$(DEPS) $(LIBDIR) $(PKGDIR) $(INCDIR):
	$(call run-cmd,install_dir,$@)

install: all $(LIBDIR) $(PKGDIR) $(INCDIR) $(PC)
	$(call run-cmd,install_file,$(addsuffix $(VER),$(SO)),$(LIBDIR))
	$(call run-cmd,install_file,$(PC),$(PKGDIR))
	$(call run-cmd,install_file,$(H),$(INCDIR))
	$(foreach lib,$(SO),$(call ln-so,$(lib)))
	$(call ldconfig)

uninstall:
	rm $(addprefix $(LIBDIR)/,$(addsuffix *,$(SOMAJ)))
	rm $(LIBDIR)/$(A)
	rm $(PKGDIR)/$(PC)

librSON.so: $(O)
librSON.socket.so: $(O_SOCK)
$(SO): A = $(patsubst %.so,%.a,$@)
$(SO):
	$(call run-cmd,ar,$(A),$^)
	$(call run-cmd,ranlib,$(A))
	$(call run-cmd,ccld,$(LFLAGS))
	$(call debug-strip,$@)
	$(call run-cmd,ln,$@,$@$(VER))

%.pc: %.pc.in
	$(call run-cmd,sed,$<,$@)

%.o: %.cpp $(DEPS)
	$(call makedep,$(CXX),$(DEPFLAGS))
	$(call run-cmd,cxx,$(CFLAGS))

clean: test $(DEPS)
	@$(MAKE) -C test clean
	$(call run-cmd,rm,rSON,*.o $(addsuffix *,$(SO)) $(patsubst %.so,%.a,$(SO)) $(PC) *.gcda *.gcno)
	$(call run-cmd,rm,makedep,.dep/*.d)

tests: all test
	@$(MAKE) -C test

check: all test
	@$(MAKE) -C test check

.PHONY: clean librSON.pc install all uninstall default .cpp.o tests check
.SUFFIXES: .cpp .so .o
-include .dep/*.d
