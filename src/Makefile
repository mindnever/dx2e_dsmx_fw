BINARY = dx2e_alt

CSTD = -std=gnu99
CFLAGS += -Wno-redundant-decls
DEFS += -DPROTO_HAS_CYRF6936 -DFREQ_MHz=48
OBJS = uart_util.o dx2e_adc.o miniprintf.o dsm2_cyrf6936.o cyrf6936.o common.o clock.o clock_isr.o led.o

LDSCRIPT = app.stm32f051k4.ld

include Makefile.inc
