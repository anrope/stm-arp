#add all your source (.c) files, e.g.:
#mainc = source1.c source2.c source3.c
mainc = biquadblocktest.c biquadblock_filter.c

#Uncomment the 'arpldname' line for
#the device being compiled for
#STM32F105RCT6 (256k flash / 64k ram)
# arpldname = arp.rc.ld
#STM32F105RBT6 (128k flash / 32k ram)
arpldname = arp.rb.ld
#STM32F105R8T6 (64k flash / 20k ram)
# arpldname = arp.r8.ld

#this sets the name of the final executable
executable = arp

###########################################
#don't change anything below this comment!#
###########################################

#path to linker scripts
arpldpath = /usr/linux/arm/stm32-gcc/lib/arplib

#tell gcc to use our linker script
arpld  = -T $(arpldpath)/$(arpldname)

#define some macros to pass to the c preprocessor via gcc
#to make the external libraries work properly for our chip

#using a STM32 connectivity line chip
defcl = -D STM32F10X_CL
#using the stdperiph library
defstdperiph = -D USE_STDPERIPH_DRIVER
#using a high speed external oscillator at 8mHz
defhse = -D 'HSE_VALUE=((uint32_t)25000000)'
defs = $(defcl) $(defstdperiph) $(defhse)

#create aliases for some directories we'll be
#referring to often

#root where our libraries live
cm3root = /usr/linux/arm/stm32-gcc/lib

cmsisroot = $(cm3root)/CMSIS/CM3
stdperiphroot = $(cm3root)/STM32F10x_StdPeriph_Driver
stdperiphinc = $(stdperiphroot)/inc
stdperiphsrc = $(stdperiphroot)/src

incdevice = $(cmsisroot)/DeviceSupport/ST/STM32F10x
inccore = $(cmsisroot)/CoreSupport

#point to the startup script that is run whenever the chip
#is powered on or coming out of a reset
#this script does things like setting up the .bss and .data sections
#(zero-filling the .bss, and copying ROM to RAM for .data)
#and provide weakly-defined handlers for all interrupts
startupscript = $(incdevice)/startup/gcc_ride7/startup_stm32f10x_cl.s

#list all of the source files we'll need from external libraries

miscsrc = $(stdperiphsrc)/misc.c
rccsrc = $(stdperiphsrc)/stm32f10x_rcc.c
gpiosrc = $(stdperiphsrc)/stm32f10x_gpio.c
adcsrc = $(stdperiphsrc)/stm32f10x_adc.c
dacsrc = $(stdperiphsrc)/stm32f10x_dac.c
dmasrc = $(stdperiphsrc)/stm32f10x_dma.c
timsrc = $(stdperiphsrc)/stm32f10x_tim.c
flashsrc = $(stdperiphsrc)/stm32f10x_flash.c

arpitfroot = $(cm3root)/arplib
arpitf = $(arpitfroot)/arpinit.c \
			$(arpitfroot)/arpint.c \
			$(arpitfroot)/arpsample.c \
			$(arpitfroot)/arperr.c

#tell gcc where to look for headers for our external libraries
cm3inc = -I$(inccore) -I$(incdevice) -I$(cm3root) -I$(stdperiphinc) \
		-iquote $(arpitfroot)

cfiles = $(incdevice)/*.c $(rccsrc) $(gpiosrc) $(miscsrc) $(adcsrc) \
			$(dacsrc) $(dmasrc) $(timsrc) $(flashsrc) $(arpitf)

#set the path to our cross-toolchain
ccpath = /usr/linux/arm/stm32-gcc/bin
#set the name of the cross-compiler
ccname = stm32-gcc
#set the name of our cross-toolchain's objcopy
objcopyname = stm32-objcopy

CC = $(ccpath)/$(ccname)
objcopy = $(ccpath)/$(objcopyname)

#necessary gcc flags to compile for our cortex-m3 core
CFLAGS = -march=armv7-m -mthumb

#list the sections we want to extract from the elf executable
#to put into the binary memory image (for objcopy)
goodsections = -j .isr_vector -j .text -j .init -j .fini -j .rodata \
				-j .data -j .init_array -j .fini_array -j .jcr -j .bss
#tell objcopy to output a binary file (memory image)
objcout = -O binary
elftobin = $(objcopy) $(objcout) $(goodsections)

#rule to compile source files in mainc along with the external libraries
mainc : $(arpitf) $(arpitfroot/stm32f10x_conf.h)
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(mainc) -o $(executable).elf -O3

	$(elftobin) $(executable).elf $(executable).bin

.PHONY : clean
clean :
	rm $(executable).elf
	rm $(executbale).bin
