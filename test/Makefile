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

include ../Makefile.inc

LIBTESTMAKE = libTestMake $(shell pkg-config --cflags libTest)
ifeq ($(BUILD_VERBOSE), 0)
	LIBTESTMAKE += -q
endif
LIBTESTMAKE += -o../JSONAtom.o -o../JSONErrors.o -o../String.o -o../Memory.o
LIBTEST = libTest

READER_TESTS = testJSONNull.so testJSONBool.so testJSONInt.so testJSONFloat.so testJSONString.so testJSONObject.so testJSONArray.so testParser.so
TESTS = $(READER_TESTS) testWriter.so

quiet_cmd_testMake = -n
cmd_testMake = $(LIBTESTMAKE) -o$(2) $(3)
quiet_cmd_test = -n
cmd_test = $(LIBTEST) $(2)

default: all

all: $(TESTS)

check: all
	$(call run-cmd,test,$(subst .so,,$(TESTS)))

clean:
	$(call run-cmd,rm,tests,$(TESTS))

testJSONObject.so testJSONArray.so: LIBTESTMAKE += -o../JSONInt.o
testParser.so testWriter.so: LIBTESTMAKE += -o../JSONNull.o -o../JSONBool.o -o../JSONInt.o -o../JSONFloat.o -o../JSONString.o -o../JSONObject.o -o../JSONArray.o
$(READER_TESTS): LIBTESTMAKE += -o../Writer.o
$(TESTS): O_FILE = ../$(subst test,,$(@:.so=)).o
%.so: %.cpp
	$(call run-cmd,testMake,$(O_FILE),$<)

.PHONY: default all check clean
.SUFFIXES: .cpp .so
