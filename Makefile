mainc = main.c
daccal = daccal.c
adccal = adccal.c
blink = blinkled.c
biquadtest = biquadtest.c biquad_filter.c
biquadblocktest = biquadblocktest.c biquadblock_filter.c
firtest = firtest.c arpfir.c
firblocktest = firblocktest.c arpfirblock.c

#Uncomment the 'arpld' line for
#the device being compiled for
#STM32F105RCT6 (256k flash / 64k ram)
# arpld = -T arp.rc.ld
#STM32F105RBT6 (128k flash / 32k ram)
arpld = -T arp.rb.ld
#STM32F105R8T6 (64k flash / 20k ram)
# arpld = -T arp.r8.ld

executable = arp

def105 = -D STM32F10X_CL
defstdperiph = -D USE_STDPERIPH_DRIVER
defhse = -D 'HSE_VALUE=((uint32_t)8000000)'
defs = $(def105) $(defstdperiph) $(def8mhz)

cm3root = /home/arp/stm/arp

cmsisroot = $(cm3root)/libraries/CMSIS/CM3
stdperiphroot = $(cm3root)/libraries/STM32F10x_StdPeriph_Driver
stdperiphinc = $(stdperiphroot)/inc
stdperiphsrc = $(stdperiphroot)/src

incdevice = $(cmsisroot)/DeviceSupport/ST/STM32F10x
inccore = $(cmsisroot)/CoreSupport

startupscript = $(incdevice)/startup/gcc_ride7/startup_stm32f10x_cl.s

cm3inc = -I$(inccore) -I$(incdevice) -I$(cm3root) -I$(stdperiphinc)

miscsrc = $(stdperiphsrc)/misc.c
rccsrc = $(stdperiphsrc)/stm32f10x_rcc.c
gpiosrc = $(stdperiphsrc)/stm32f10x_gpio.c
adcsrc = $(stdperiphsrc)/stm32f10x_adc.c
dacsrc = $(stdperiphsrc)/stm32f10x_dac.c
dmasrc = $(stdperiphsrc)/stm32f10x_dma.c
timsrc = $(stdperiphsrc)/stm32f10x_tim.c
flashsrc = $(stdperiphsrc)/stm32f10x_flash.c

arpitf = $(cm3root)/arpinit.c \
			$(cm3root)/arpint.c \
			$(cm3root)/arpsample.c \
			$(cm3root)/arperr.c

cfiles = $(incdevice)/*.c $(rccsrc) $(gpiosrc) $(miscsrc) $(adcsrc) \
			$(dacsrc) $(dmasrc) $(timsrc) $(flashsrc) $(arpitf)

ccpath = /home/arp/stm/ctc/bin
CC = $(ccpath)/arm-eabi-gcc

CFLAGS = -march=armv7-m -mthumb

objcopy = $(ccpath)/arm-eabi-objcopy
goodsections = -j .isr_vector -j .text -j .init -j .fini -j .rodata -j .data -j .init_array -j .fini_array -j .jcr -j .bss
objcout = -O binary
elftobin = $(objcopy) $(objcout) $(goodsections)

mainc : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(mainc) -o $(executable).eabi
	
firtest : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(firtest) -o $(executable).eabi -O3

firblocktest : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(firblocktest) -o $(executable).eabi -O3

firtests : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(firblocktest) -S -O3
	
biquadtest : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(biquadtest) -o $(executable).eabi -O3

biquadblocktest : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(biquadblocktest) -o $(executable).eabi -O3

	$(elftobin) $(executable).eabi $(executable).bin
	
biquadtests : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(biquadblocktest) -S -O3
	
adccal : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(adccal) -o $(executable).eabi
	
daccal : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(daccal) -o $(executable).eabi
	
blink : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(blink) -o $(executable).eabi
	
main-all-libs : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(stdperiphsrc)/*.c $(startupscript) -o $(executable).eabi.alllibs

.PHONY : clean
clean:
	rm $(executable).eabi $(executable).eabi.alllibs
	
#arm-eabi-strip
