include ../Makefile.inc

LIBTESTMAKE = libTestMake
ifeq ($(BUILD_VERBOSE), 0)
	LIBTESTMAKE += -q
endif
LIBTESTMAKE += -o../JSONAtom.o -o../JSONErrors.o
LIBTEST = libTest

TESTS = testJSONNull.so testJSONBool.so testJSONInt.so testJSONFloat.so testJSONString.so testJSONObject.so testJSONArray.so

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

$(TESTS): O_FILE = ../$(subst test,,$(@:.so=)).o
%.so: %.cpp
	$(call run-cmd,testMake,$(O_FILE),$<)

.PHONY: default all check clean
.SUFFIXES: .cpp .so
