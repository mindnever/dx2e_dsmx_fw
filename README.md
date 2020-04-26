# dx2e_dsmx_fw

MCU pin maps

	
PA0   J2 (steering) ADC
PA1	  J3 (throttle) ADC
PA2	  R1 (st trim) ADC
PA3	  R2 (th trim) ADC
PA4	  Sawtooth signal (ADC)
PA5	  R3 (st rate) ADC
PA6	  Vbat     (ADC)
PA7	  CYRF / RF ?? AUX SW (3 pos)
PA8	  CYRF IRQ  (cut and use as LED2)
PA9	  J4_TX1
PA10	J4_RX1
PA11	TX_EN
PA12	RX_EN
PA13	DBG_SWDIO
PA14	DBG_SWCLK
PA15	CYRF      SPI_SS
PB0	  CYRF      RST
PB1	  Power_off_bypass
PB2	  S2        (bind button)
PB3	  CYRF      SPI_SCK
PB4	  CYRF      SPI_MISO
PB5	  SPI_ MOSI
PB6	  D1        (LED)
PB7	  S3        (th direction)
PB8	  S4        (st direction)
PF0   OSC_IN	  CYRF_XOUT
PF1   OSC_OUT	  TP3
	

CYRF STUFF
SPI_SS	24	OK
SPI_SCK	25	OK
IRQ	26	OK
MOSI	27	OK
MISO	28	OK
XOUT	29	OK
PACTL	30	
RST	34	OK
		

J4
1.	VDD
2.	GND
3.	TX
4.	RX
5.	BOOT0
6.	NRST


DEBUG
1.	VDD
2.	GND
3.	GND
4.	N/C
5.	GND
6.	NRST
7.	N/C
8.	N/C
9.	SWCLK
10.	SWDIO
