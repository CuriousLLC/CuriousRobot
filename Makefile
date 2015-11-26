# The name of your project (used to name the compiled .hex file)
TARGET = $(notdir $(CURDIR))

# Path to the Arduino build
ARDUINOPATH ?= /home/rday/Development/Teensyduino/arduino-1.6.5-r5

# The teensy version to use, 30, 31, or LC
TEENSY = LC

# Set to 24000000, 48000000, or 96000000 to set CPU core speed
TEENSY_CORE_SPEED = 48000000

# Defines
DEFINES = -D__MKL26Z64__ -DTEENSYDUINO=125 -DARDUINO=10605 -DF_CPU=48000000 -DARDUINO_ARCH_AVR -DUSB_SERIAL -DLAYOUT_US_ENGLISH

# Chip specific
OPTIONS = $(DEFINES) -mthumb -mcpu=cortex-m0plus -fsingle-precision-constant -Wall

# directory to build in
BUILDDIR = $(abspath $(CURDIR)/build)

#************************************************************************
# Location of Teensyduino utilities, Toolchain, and Arduino Libraries.
# To use this makefile without Arduino, copy the resources from these
# locations and edit the pathnames.  The rest of Arduino is not needed.
#************************************************************************

# path location for Teensy Loader, teensy_post_compile and teensy_reboot
TOOLSPATH = $(ARDUINOPATH)/hardware/tools

# path location for Teensy 3 core
COREPATH = $(abspath $(CURDIR)/teensy3)

# Linker script
LDSCRIPT=$(COREPATH)/mkl26z64.ld

# path location for Arduino libraries
LIBRARYPATH = $(abspath $(CURDIR)/libraries)

# path location for the arm-none-eabi compiler
COMPILERPATH = $(TOOLSPATH)/arm/bin

TESTDIR = /home/rday/Development/Teensyduino/Robot/gtest/googletest/googletest
#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -Os -ffunction-sections -fdata-sections -nostdlib -MMD $(OPTIONS) -Isrc -I$(COREPATH)

# compiler options for C++ only
CXXFLAGS = -std=gnu++11 -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -Os -Wl,--gc-sections,--relax,--defsym=__rtc_localtime=0 -T$(LDSCRIPT) --specs=nano.specs -fsingle-precision-constant -mthumb -mcpu=cortex-m0plus

# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
OBJDUMP = $(abspath $(COMPILERPATH))/arm-none-eabi-objdump
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size
STRIP = $(abspath $(COMPILERPATH))/arm-none-eabi-strip
AR = $(abspath $(COMPILERPATH))/arm-none-eabi-ar

# Include all the files from the user libraries
LC_FILES := $(wildcard $(LIBRARYPATH)/*/*.c)
LCPP_FILES := $(wildcard $(LIBRARYPATH)/*/*.cpp)

# Include all the files from the Teensy Core
TC_FILES := $(wildcard $(COREPATH)/*.c)
TCPP_FILES := $(wildcard $(COREPATH)/*.cpp)

# Include all the user project files
C_FILES := $(wildcard src/**/*.c) $(wildcard src/*.c)
CPP_FILES := $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
INO_FILES := $(wildcard src/**/*.ino)
TEST_FILES := $(wildcard tests/*.cpp)

# include paths for libraries
L_INC := -I$(LIBRARYPATH) -Iteensy3/

TEENSY_SOURCES := $(TCPP_FILES:.cpp=.o) $(TC_FILES:.c=.o)
LIB_SOURCES := $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
PROJECT_SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(INO_FILES:.ino=.o)
TEST_SOURCES := $(TEST_FILES:.cpp=.o)

TEENSY_OBJS := $(foreach src,$(TEENSY_SOURCES), $(BUILDDIR)/$(src))
LIB_OBJS := $(foreach src,$(LIB_SOURCES), $(BUILDDIR)/$(src))
PROJECT_OBJS := $(foreach src,$(PROJECT_SOURCES), $(BUILDDIR)/$(src))
TEST_OBJS := $(foreach src,$(TEST_SOURCES), $(BUILDDIR)/$(src))

all: hex
	@echo "Building all"
	@echo "COREPATH\t$(COREPATH)"
	@echo "BUILDDIR\t$(BUILDDIR)"

build: $(TARGET).elf

hex: $(TARGET).hex

post_compile: $(TARGET).hex
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $<)" -path=$(CURDIR) -tools="$(abspath $(TOOLSPATH))" -board=TEENSYLC

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot

upload: post_compile reboot

$(BUILDDIR)/$(COREPATH)/%.o: $(COREPATH)/%.c
	@echo "[TEENSY CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"
	@$(AR) rcs $(BUILDDIR)/core.a "$@"

$(BUILDDIR)/$(COREPATH)/%.o: $(COREPATH)/%.cpp
	@echo "[TEENSY CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"
	@$(AR) rcs $(BUILDDIR)/core.a "$@"

$(BUILDDIR)/$(LIBRARYPATH)/%.o: $(LIBRARYPATH)/%.c
	@echo "[LIBS CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"
	@$(AR) rcs $(BUILDDIR)/libs.a "$@"

$(BUILDDIR)/$(LIBRARYPATH)/%.o: $(LIBRARYPATH)/%.cpp
	@echo "[LIBS CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"
	@$(AR) rcs $(BUILDDIR)/libs.a "$@"

$(BUILDDIR)/src/%.o: src/%.c
	@echo "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/src/%.o: src/%.cpp
	@echo "[CXX]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(TARGET).elf: $(TEENSY_OBJS) $(LIB_OBJS) $(PROJECT_OBJS)

%.hex: %.elf
	@echo "[LD]\t$@"
	$(CC) $(LDFLAGS) -o $(TARGET).elf $(BUILDDIR)/src/*.o $(BUILDDIR)/libs.a $(BUILDDIR)/core.a -L$(BUILDDIR)
	@echo "[HEX]\t$@"
	$(SIZE) "$<"
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 "$<" "$@"
	$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"
	$(OBJDUMP) -D $(TARGET).elf > $(TARGET).list

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(TARGET).elf" "$(TARGET).hex"
	@rm -f tests/*.o


$(BUILDDIR)/tests/%.o: tests/%.cpp
	@echo "[BUILDING TEST]\t$<"
	@mkdir -p "$(dir $@)"
	@g++ $(L_INC) $(DEFINES) -fsingle-precision-constant -Itests/ -c "$<" -o "$@"

$(BUILDDIR)/tests/Message.o:
	@g++ $(L_INC) $(DEFINES) -std=gnu++11 -Itests/ -c libraries/CuriousRobot/Message.cpp -o $(BUILDDIR)/tests/Message.o

$(BUILDDIR)/tests/ServoController.o:
	@g++ $(L_INC) $(DEFINES) -std=gnu++11 -Itests/ -c libraries/CuriousRobot/ServoController.cpp -o $(BUILDDIR)/tests/ServoController.o

test:  $(TEST_OBJS) $(BUILDDIR)/tests/Message.o $(BUILDDIR)/tests/ServoController.o
	@echo [RUNNING TESTS]
	@g++ $(BUILDDIR)/tests/*.o -o tests/run_tests
	@tests/run_tests
	
