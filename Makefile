## Example usage:
##
## To compile 01.cpp (creates binary in .obj/):
##     make DAY=01
##
## To compile and run 02.cpp:
##     make test DAY=02
##
## To compile 03.cpp with symbols and no optimisation and run in debugger:
##     make CONFIG=debug debug DAY=03
##
## To set a breakpoint and debug 04.cpp:
##     make debug DAY=04 "DEBUGGER_ARGS=-ex \"break 03.cpp:41\" -ex run"
##
## To download the input for day 5 (see get-input.sh for instructions):
##     make get-input DAY=05
##
## On Windows (MSYS2 + MinGW-W64 + GCC), use native make (e.g., "mingw32-make").

DAY=01

## Some days might need extra source files
#DAY_04_OBJECTS=md5.o

ifeq ($(OS),Windows_NT)
SHELL=cmd
PATH=C:\msys64\mingw64\bin;C:\msys64\usr\bin
RM-RF=2>NUL rd /s /q $(1)
EXEEXT=.exe
else
RM-RF=rm -rf $(1)
PTHREAD=-pthread
endif

# Default config.
CONFIG=release

# Available configs.
CONFIGS=debug release

# Comment out to disable precompiled headers.
pch_NAME=precompiled.h

# Top-level targets.
all: $(CONFIG)-all
test: get-input $(CONFIG)-test
debug: $(CONFIG)-debug
clean: ; $(call RM-RF,.obj)
.PHONY: all test debug clean get-input

get-input: input/$(DAY)
input/%: | input ; bash get-input.sh $*

# # Package config.
# PACKAGES=libavcodec libavformat libavutil libswresample libswscale
# PACKAGE_CPPFLAGS=$(shell pkg-config --cflags-only-I $(PACKAGES))
# PACKAGE_CFLAGS=$(shell pkg-config --cflags-only-other $(PACKAGES))
# PACKAGE_LDFLAGS=$(shell pkg-config --libs-only-L $(PACKAGES))
# PACKAGE_LDLIBS=$(shell pkg-config --libs-only-l $(PACKAGES))

# Options common to all configs.
SRCDIR=.
WARNFLAGS=-Wall -Wextra -Werror
CPPFLAGS=$(PACKAGE_CPPFLAGS)
CFLAGS=-ggdb3 $(PTHREAD) $(PACKAGE_CFLAGS) $(WARNFLAGS)
CXXFLAGS=-std=c++2a
LDFLAGS=$(PACKAGE_LDFLAGS)
LDLIBS=$(PACKAGE_LDLIBS) -lpcre2-8
RESOURCES=
OBJECTS=$(DAY).o $(DAY_$(DAY)_OBJECTS) re.o
DEBUGGER_ARGS=-ex run -ex "bt full"

# Config-specific options.
release_CPPFLAGS=-DNDEBUG
release_CFLAGS=-O2 -march=native -mtune=native -mfpmath=sse -flto
release_CXXFLAGS=
release_LDFLAGS=
release_LDLIBS=

debug_CPPFLAGS=
debug_CFLAGS=-O0
debug_CXXFLAGS=
debug_LDFLAGS=
debug_LDLIBS=

release_FILENAME=$(DAY)$(EXEEXT)
debug_FILENAME=$(DAY)-debug$(EXEEXT)

# Internal variables.
filename=$($(config)_FILENAME)
objdir=.obj/$(config)
objects=$(OBJECTS:%=$(objdir)/%)
cppflags=$(CPPFLAGS) $($(config)_CPPFLAGS)
cflags=$(CFLAGS) $($(config)_CFLAGS)
cxxflags=$(cflags) $(CXXFLAGS) $($(config)_CXXFLAGS)
ldflags=$(LDFLAGS) $($(config)_LDFLAGS) $(LDFLAGS)
ldlibs=$(LDLIBS) $($(config)_LDLIBS)
resflags=-DCONFIG=$(config)
resources=$(RESOURCES)
pch_object=$(pch_NAME:%=$(objdir)/%.gch)
pch_cppflags=$(pch_NAME:%=-include $(objdir)/%)

# Rules that depend on config.
define config-defs

# Top-level targets (advanced).
$(config)-all: $(objdir)/$(filename)

$(config)-test: $(objdir)/$(filename)
	"$$<" $(ARGS)

$(config)-debug: $(objdir)/$(filename)
	gdb --quiet --batch $(DEBUGGER_ARGS) -ex quit --args "$$<" $(ARGS)

all-configs: $(config)

.PHONY: $(config)
.PHONY: $(config)-test
.PHONY: $(config)-debug
.PHONY: $(config)-clean
.PHONY: all-configs

# Bottom-level targets.
$(objdir)/$(filename): $(objects)
	$(CXX) -o $$@ $(cxxflags) $(ldflags) $$^ $(ldlibs)

$(objdir)/%.o: $(SRCDIR)/%.cpp $(pch_object) | $(objdir)
	$(CXX) -c -o $$@ -MMD -MP $(cppflags) $(pch_cppflags) $(cxxflags) $$<

$(objdir)/%.gch: $(SRCDIR)/% | $(objdir)
	$(CXX) -MMD $(cppflags) $(cxxflags) -x c++-header $$< -c -o $$@

# Miscellaneous config-dependent rules.
.obj/$(config): | .obj ; -mkdir "$$@"
.SECONDARY: $(pch_NAME:%=$(objdir)/%.gch)

# Automatic prerequisites.
-include $(objects:%.o=%.d) $(pch_NAME:%=$(objdir)/%.d)

endef

# Invoke config-defs for each config.
$(foreach config,$(CONFIGS),$(eval $(config-defs)))

# Miscellaneous.
.obj: ; -mkdir "$@"
input: ; -mkdir "$@"
