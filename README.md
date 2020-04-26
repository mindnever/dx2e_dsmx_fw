# dx2e_dsmx_fw

MCU pin maps

<table>	
	<tr><td>PA0</td><td>J2</td><td>steering ADC</td></tr>
	<tr><td>PA1</td><td>J3</td><td>throttle ADC</td></tr>
<tr><td>PA2</td><td>R1</td><td>(st trim) ADC</td></tr>
<tr><td>PA3</td><td>R2</td><td>(th trim)ADC</td></tr>
<tr><td>PA4</td><td>RF signal</td><td>Sawtooth signal (ADC)</td></tr>
<tr><td>PA5</td><td>R3</td><td>(st rate) ADC</td></tr>
<tr><td>PA6</td><td>Vbat</td><td>(ADC)</td></tr>
<tr><td>PA7</td><td>CYRF / RF</td><td>cut trace and connect 3 pos AUX SW</td></tr>
<tr><td>PA8</td><td>CYRF IRQ</td><td>cut trace and use as LED2</td></tr>
<tr><td>PA9</td><td>J4_TX1</td></td><td></tr>
<tr><td>PA10</td><td>J4_RX1</td></td><td></tr>
<tr><td>PA11</td><td>RF module TX_EN</td></td><td></tr>
<tr><td>PA12</td><td>RF module RX_EN</td></td><td></tr>
<tr><td>PA13</td><td>DBG_SWDIO</td></td><td></tr>
<tr><td>PA14</td><td>DBG_SWCLK</td></td><td></tr>
<tr><td>PA15</td><td>RF module SPI_SS</td></td><td></tr>
<tr><td>PB0</td><td>RF module RST</td></td><td></tr>
<tr><td>PB1</td><td>Power_off_bypass</td></tr>
<tr><td>PB2</td><td>S2</td><td>bind button</td></tr>
<tr><td>PB3</td><td>RF module SPI_SCK</td></td><td></tr>
<tr><td>PB4</td><td>RF module SPI_MISO</td></td><td></tr>
<tr><td>PB5</td><td>RF module SPI_MOSI</td></td><td></tr>
<tr><td>PB6</td><td>D1</td><td>LED</td></tr>
<tr><td>PB7</td><td>S3</td><td>th direction</td></tr>
<tr><td>PB8</td><td>S4</td><td>st direction</td></tr>
<tr><td>PF0/OSC_IN</td><td>RF module XOUT</td></td><td></tr>
<tr><td>PF1/OSC_OUT</td><td>TP3</td></td><td></tr>
</table>

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
