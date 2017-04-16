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
CRUNCHMAKE += -o../JSONAtom.o -o../JSONErrors.o -o../String.o -o../Memory.o -o../Writer.o -o../Stream.o
CRUNCHMAKE += -o../JSONNull.o -o../JSONBool.o -o../JSONInt.o -o../JSONFloat.o -o../JSONString.o -o../JSONObject.o -o../JSONArray.o
CRUNCH = crunch

READER_TESTS = testJSONNull.so testJSONBool.so testJSONInt.so testJSONFloat.so testJSONString.so testJSONObject.so testJSONArray.so testParser.so
TESTS = testJSONErrors.so $(READER_TESTS) testWriter.so
DEPS = ../.dep/test

quiet_cmd_crunchMake = -n
cmd_crunchMake = $(CRUNCHMAKE) -o$(2) $(3)
quiet_cmd_crunch = -n
cmd_crunch = $(CRUNCH) $(2)

default: all

all: $(TESTS)

$(DEPS):
	$(call run-cmd,install_dir,$@)

check: all
	$(call run-cmd,crunch,$(subst .so,,$(TESTS)))

clean:
	$(call run-cmd,rm,tests,$(TESTS))
	$(call run-cmd,rm,makedep,../.dep/test/*.d)

testParser.so testWriter.so: CRUNCHMAKE += -o../Parser.o
%.so: %.cpp
	$(call run-cmd,crunchMake,$(O_FILE),$<)

rSON.o: $(DEPS) rSON.cpp
	$(MAKE) -C .. test/rSON.o

.PHONY: default all check clean
.SUFFIXES: .cpp .so
