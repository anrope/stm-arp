mainc = main.c
daccal = daccal.c
adccal = adccal.c
blink = blinkled.c
biquad = biquad.c biquad_filter.c
firtest = firtest.c arpfirblock.c

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

arpld = -T arp.ld

mainc : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(mainc) -o arp.eabi
	
firtest : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(firtest) -o arp.eabi -O3
	
biquad : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(biquad) -o arp.eabi -O3
	
biquads : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(biquad) -S

firtests : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(firtest) -S -O3
	
adccal : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(adccal) -o arp.eabi
	
daccal : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(daccal) -o arp.eabi
	
blink : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(startupscript) $(blink) -o arp.eabi
	
main-all-libs : $(arpitf) stm32f10x_conf.h
	$(CC) $(CFLAGS) $(defs) $(cm3inc) $(arpld) $(cfiles) \
	$(stdperiphsrc)/*.c $(startupscript) -o arp.eabi.alllibs

.PHONY : clean
clean:
	rm arp.eabi arp.eabi.alllibs
	
#arm-eabi-strip
