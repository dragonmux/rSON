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

include ../Makefile.inc

CRUNCHMAKE = crunchMake $(shell pkg-config --cflags --libs crunch) -lstdc++ -lm
ifeq ($(BUILD_VERBOSE), 0)
	CRUNCHMAKE += -q
endif
ifeq ($(strip $(COVERAGE)), 1)
	CRUNCHMAKE += -lgcov
endif
CRUNCHMAKE += ../JSONAtom.o ../JSONErrors.o ../String.o ../Writer.o ../Stream.o
CRUNCHMAKE += ../JSONNull.o ../JSONBool.o ../JSONInt.o ../JSONFloat.o ../JSONString.o ../JSONObject.o ../JSONArray.o
CRUNCH = crunch

READER_TESTS = testJSONNull.so testJSONBool.so testJSONInt.so testJSONFloat.so testJSONString.so testJSONObject.so testJSONArray.so testParser.so
TESTS = testJSONErrors.so $(READER_TESTS) testWriter.so testHeader.so testSocket.so
DEPS = ../.dep/test

quiet_cmd_crunchMake = -n
cmd_crunchMake = $(CRUNCHMAKE) $(2) $(3)
quiet_cmd_crunch = -n
cmd_crunch = $(CRUNCH) $(2)

default: all

all: $(TESTS)

$(DEPS):
	$(call run-cmd,install_dir,$@)

check: all
	$(call run-cmd,crunch,$(subst .so,,$(TESTS)))

clean:
	$(call run-cmd,rm,tests,$(TESTS) rSON.o)
	$(call run-cmd,rm,makedep,../.dep/test/*.d)

testParser.so testWriter.so: CRUNCHMAKE += ../Parser.o
testHeader.so: rSON.o
testHeader.so: CRUNCHMAKE += rSON.o
testSocket.so: CRUNCHMAKE += ../librSON.a ../rSON_socket.o -I..
%.so: %.cpp
	$(call run-cmd,crunchMake,$(O_FILE),$<)

rSON.o: $(DEPS) rSON.cpp
	$(Q)$(MAKE) FOR_TESTS=1 -C .. test/rSON.o

.PHONY: default all check clean
.SUFFIXES: .cpp .so
