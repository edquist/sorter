#!/usr/bin/make -f

# Carlako's super-duper autodep makefile
#
# Define SRCS.program (*.cpp sources for each program) in SOURCES.mk
# Header deps are automatic

RM = rm -rf
CXX = g++
CXXFLAGS = -Wall -O3 $(WIPFLAGS)
CPPFLAGS = -std=c++11 $(DEPFLAGS)
DEPFLAGS = -MMD -MP -MF $(DREC) -MT $(OREC) -MT $(AREC)

ifdef WIP
WIPFLAGS = -Wno-unused-function -Wno-unused-variable
endif

SPAT = %.cpp
ODIR = .o
ADIR = asm
OPAT = $(ODIR)/%.o
DPAT = $(ODIR)/%.d
APAT = $(ADIR)/%.s
SREC = $(subst %,$*,$(SPAT))
DREC = $(subst %,$*,$(DPAT))
OREC = $(subst %,$*,$(OPAT))
AREC = $(subst %,$*,$(APAT))
OBJS = $(SRCS.$@:$(SPAT)=$(OPAT))
ASMS = $(SRCS:$(SPAT)=$(APAT))

COMP = $(Q) $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
LINK = $(Q) $(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

include SOURCES.mk

SRCS_VARS := $(filter SRCS.%,$(.VARIABLES))
PROGS     := $(sort $(SRCS_VARS:SRCS.%=%))

ALL_SRCS  := $(sort $(foreach prog,$(PROGS),$(SRCS.$(prog))))
ALL_OBJS  := $(ALL_SRCS:$(SPAT)=$(OPAT))
ALL_ASMS  := $(ALL_SRCS:$(SPAT)=$(APAT))

CLEAN     := $(wildcard $(PROGS) $(ODIR) $(ADIR))

all: $(PROGS)
asm: $(ALL_ASMS)

clean:
	$(if $(CLEAN),$(RM) $(CLEAN))

.PHONY: all clean asm

ifndef VERBOSE
ifndef V
define Q
$(if $(SHORT_TYPE),@echo "[$(SHORT_TYPE)] $(notdir $@)")
@
endef
endif
endif

$(ALL_OBJS): SHORT_TYPE := CXX
$(ALL_ASMS): SHORT_TYPE := CXX-S
$(PROGS):    SHORT_TYPE := LINK
$(ODIR):     SHORT_TYPE :=
$(ADIR)/:    SHORT_TYPE :=

$(ODIR) $(ADIR)/:
	$(Q) mkdir -p $@

$(ALL_OBJS): | $(ODIR)
$(ALL_ASMS): | $(ODIR) $(ADIR)/

ifneq ($(MAKECMDGOALS),clean)
ifeq ($(MAKECMDGOALS),asm)
  -include $(patsubst $(APAT),$(DPAT),$(wildcard $(ALL_ASMS)))
else
  -include $(patsubst $(OPAT),$(DPAT),$(wildcard $(ALL_OBJS)))
endif
endif

$(ALL_OBJS): $(OPAT): $(SPAT)
	$(COMP)

$(ALL_ASMS): $(APAT): $(SPAT)
	$(COMP) -S

.SECONDEXPANSION:

$(PROGS): $$(OBJS)
	$(LINK)

