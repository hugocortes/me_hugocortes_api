#############################################################################
#
# Makefile for librf24mesh examples on Raspberry Pi
#
# By:  TMRh20
# Date:    2013/09
#
# Description:
# ------------
# use make all and make install to install the examples
# You can change the install directory by editing the prefix line
#
prefix := /usr/local

ARCH=armv6zk
ifeq "$(shell uname -m)" "armv7l"
ARCH=armv7-a
endif

# Detect the Raspberry Pi from cpuinfo
#Count the matches for BCM2708 or BCM2709 in cpuinfo
RPI=$(shell cat /proc/cpuinfo | grep Hardware | grep -c BCM2708)
ifneq "${RPI}" "1"
RPI=$(shell cat /proc/cpuinfo | grep Hardware | grep -c BCM2709)
endif

ifeq "$(RPI)" "1"
# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x
endif

# define programs
PROGRAM = radio_comms
SOURCES = ${PROGRAMS:=.cpp} S_NRF24.cpp S_NRF24.h

HEADERS = -NRF24_Comms.o

all: ${PROGRAM}

${PROGRAM}:${SOURCES}
	g++ ${CCFLAGS} -Wall -I../ -lrf24-bcm -lrf24network -lrf24mesh $@.cpp -o $@ ${SOURCES}
    
clean:
	rm -rf $(PROGRAM)
	
