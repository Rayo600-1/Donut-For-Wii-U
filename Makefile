
#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)

#-------------------------------------------------------------------------------
# Application metadata
#-------------------------------------------------------------------------------

APP_NAME := Donut 3D
APP_SHORTNAME := Donut
APP_AUTHOR := Rayo600

include $(DEVKITPRO)/wut/share/wut_rules

#-------------------------------------------------------------------------------
# Target / source configuration
#-------------------------------------------------------------------------------

TARGET := donut
BUILD := build

SOURCES := source
DATA := data
INCLUDES := include

# Include assets in the WUHB content
CONTENT := assets

ICON := assets/icon.png
TV_SPLASH := assets/splash_tv.png
DRC_SPLASH := assets/splash_tv.png

#-------------------------------------------------------------------------------
# Compiler options
#-------------------------------------------------------------------------------

CFLAGS := -g -Wall -Wextra -O2 -ffunction-sections \
$(MACHDEP) \
$(shell powerpc-eabi-pkg-config --cflags SDL2_mixer)

CFLAGS += $(INCLUDE) -D__WIIU__ -D__WUT__

CXXFLAGS := $(CFLAGS)

ASFLAGS := -g $(ARCH)

LDFLAGS = -g $(ARCH) $(RPXSPECS) -Wl,-Map,$(notdir $*.map)

# SDL2_mixer and its static dependencies
LIBS := $(shell powerpc-eabi-pkg-config --static --libs SDL2_mixer)

#-------------------------------------------------------------------------------
# Library directories
#-------------------------------------------------------------------------------

LIBDIRS := $(PORTLIBS) $(WUT_ROOT)

#-------------------------------------------------------------------------------
# Build system
#-------------------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))

#-------------------------------------------------------------------------------

export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)

export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#-------------------------------------------------------------------------------
# Use CC for C projects
#-------------------------------------------------------------------------------

ifeq ($(strip $(CPPFILES)),)

export LD := $(CC)

else

export LD := $(CXX)

endif

#-------------------------------------------------------------------------------

export OFILES_BIN := $(addsuffix .o,$(BINFILES))
export OFILES_SRC := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES := $(OFILES_BIN) $(OFILES_SRC)

export HFILES_BIN := $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
-I$(CURDIR)/$(BUILD)

export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifneq (,$(strip $(CONTENT)))
export APP_CONTENT := $(TOPDIR)/$(CONTENT)
endif

#-------------------------------------------------------------------------------
# Metadata
#-------------------------------------------------------------------------------

ifneq (,$(strip $(ICON)))
export APP_ICON := $(TOPDIR)/$(ICON)
else ifneq (,$(wildcard $(TOPDIR)/$(TARGET).png))
export APP_ICON := $(TOPDIR)/$(TARGET).png
else ifneq (,$(wildcard $(TOPDIR)/icon.png))
export APP_ICON := $(TOPDIR)/icon.png
endif

ifneq (,$(strip $(TV_SPLASH)))
export APP_TV_SPLASH := $(TOPDIR)/$(TV_SPLASH)
else ifneq (,$(wildcard $(TOPDIR)/tv-splash.png))
export APP_TV_SPLASH := $(TOPDIR)/$(TV_SPLASH)
else ifneq (,$(wildcard $(TOPDIR)/splash.png))
export APP_TV_SPLASH := $(TOPDIR)/splash.png
endif

ifneq (,$(strip $(DRC_SPLASH)))
export APP_DRC_SPLASH := $(TOPDIR)/$(DRC_SPLASH)
else ifneq (,$(wildcard $(TOPDIR)/drc-splash.png))
export APP_DRC_SPLASH := $(TOPDIR)/$(DRC_SPLASH)
else ifneq (,$(wildcard $(TOPDIR)/splash.png))
export APP_DRC_SPLASH := $(TOPDIR)/splash.png
endif

#-------------------------------------------------------------------------------

.PHONY: $(BUILD) clean all

#-------------------------------------------------------------------------------

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#-------------------------------------------------------------------------------

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).wuhb $(TARGET).rpx $(TARGET).elf

#-------------------------------------------------------------------------------

else

.PHONY: all

DEPENDS := $(OFILES:.o=.d)

#-------------------------------------------------------------------------------
# Main targets
#-------------------------------------------------------------------------------

all: $(OUTPUT).wuhb

$(OUTPUT).wuhb: $(OUTPUT).rpx

$(OUTPUT).rpx: $(OUTPUT).elf

$(OUTPUT).elf: $(OFILES)

$(OFILES_SRC): $(HFILES_BIN)

#-------------------------------------------------------------------------------
# Binary data
#-------------------------------------------------------------------------------

%_bin.h %.bin.o: %.bin
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#-------------------------------------------------------------------------------

endif
