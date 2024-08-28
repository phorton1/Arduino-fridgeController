EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 2400 5600 3    61   ~ 0
FAN-\nFAN+
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 66F716C8
P 2250 5550
F 0 "J?" H 2168 5767 50  0001 C CNN
F 1 "FAN" V 2250 5850 50  0000 R CNN
F 2 "" H 2250 5550 50  0001 C CNN
F 3 "~" H 2250 5550 50  0001 C CNN
	1    2250 5550
	0    -1   1    0   
$EndComp
$Comp
L Device:LED D?
U 1 1 66F64829
P 2150 4900
F 0 "D?" H 2143 4645 50  0001 C CNN
F 1 "LED" H 2150 4750 50  0000 C CNN
F 2 "" H 2150 4900 50  0001 C CNN
F 3 "~" H 2150 4900 50  0001 C CNN
	1    2150 4900
	0    1    1    0   
$EndComp
Text Notes 2511 3800 1    61   ~ 0
DIODE-\nFAN-\nFAN/DIODE+\nBATT+
Connection ~ 6600 2250
Wire Wire Line
	6450 2400 6600 2250
Wire Wire Line
	6200 1950 6600 1950
Wire Wire Line
	7100 2400 7100 2550
Wire Wire Line
	6450 2400 7100 2400
Wire Wire Line
	5750 2250 6600 2250
Wire Wire Line
	6250 2550 6600 2550
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66E9B7DC
P 6900 2550
F 0 "J?" H 6980 2592 50  0001 L CNN
F 1 "asdfg" H 6980 2546 50  0001 L CNN
F 2 "" H 6900 2550 50  0001 C CNN
F 3 "~" H 6900 2550 50  0001 C CNN
	1    6900 2550
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66E9B7D6
P 6800 2550
F 0 "J?" H 6880 2592 50  0001 L CNN
F 1 "asdfg" H 6880 2546 50  0001 L CNN
F 2 "" H 6800 2550 50  0001 C CNN
F 3 "~" H 6800 2550 50  0001 C CNN
	1    6800 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 2550 5850 2550
$Comp
L Switch:SW_SPST SW?
U 1 1 66E7E697
P 6050 2550
F 0 "SW?" H 6050 2785 50  0001 C CNN
F 1 "THERM/FULL" H 6000 2400 50  0000 C CNN
F 2 "" H 6050 2550 50  0001 C CNN
F 3 "~" H 6050 2550 50  0001 C CNN
	1    6050 2550
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66E7505D
P 5550 2550
F 0 "J?" H 5630 2592 50  0001 L CNN
F 1 "asdfg" H 5630 2546 50  0001 L CNN
F 2 "" H 5550 2550 50  0001 C CNN
F 3 "~" H 5550 2550 50  0001 C CNN
	1    5550 2550
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66E75057
P 5450 2550
F 0 "J?" H 5530 2592 50  0001 L CNN
F 1 "asdfg" H 5530 2546 50  0001 L CNN
F 2 "" H 5450 2550 50  0001 C CNN
F 3 "~" H 5450 2550 50  0001 C CNN
	1    5450 2550
	1    0    0    1   
$EndComp
Wire Notes Line
	5000 2150 4900 2150
Wire Notes Line
	5000 2250 5000 2150
Wire Notes Line
	4900 2250 5000 2250
Wire Notes Line
	4900 2150 4900 2250
Wire Wire Line
	4950 2200 5250 2250
Connection ~ 4950 2200
Wire Wire Line
	4950 2200 5250 2550
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66E2E958
P 5550 2250
F 0 "J?" H 5630 2292 50  0001 L CNN
F 1 "asdfg" H 5630 2246 50  0001 L CNN
F 2 "" H 5550 2250 50  0001 C CNN
F 3 "~" H 5550 2250 50  0001 C CNN
	1    5550 2250
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66E2E952
P 5450 2250
F 0 "J?" H 5530 2292 50  0001 L CNN
F 1 "asdfg" H 5530 2246 50  0001 L CNN
F 2 "" H 5450 2250 50  0001 C CNN
F 3 "~" H 5450 2250 50  0001 C CNN
	1    5450 2250
	1    0    0    1   
$EndComp
Text Notes 8300 2350 2    50   ~ 0
black
Wire Wire Line
	7100 2250 9250 2250
Text Notes 6150 2350 2    50   ~ 0
RED
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66DDFD08
P 6900 2250
F 0 "J?" H 6980 2292 50  0001 L CNN
F 1 "asdfg" H 6980 2246 50  0001 L CNN
F 2 "" H 6900 2250 50  0001 C CNN
F 3 "~" H 6900 2250 50  0001 C CNN
	1    6900 2250
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66DDFD02
P 6800 2250
F 0 "J?" H 6880 2292 50  0001 L CNN
F 1 "asdfg" H 6880 2246 50  0001 L CNN
F 2 "" H 6800 2250 50  0001 C CNN
F 3 "~" H 6800 2250 50  0001 C CNN
	1    6800 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 2400 9250 2250
Wire Wire Line
	9250 2200 9250 2000
Wire Wire Line
	7300 2200 9250 2200
Wire Wire Line
	7300 1950 7300 2200
Text Notes 8500 2150 2    50   ~ 0
w/white stripe
Wire Wire Line
	7100 1950 7300 1950
$Comp
L Switch:SW_SPST SW?
U 1 1 66D55B3C
P 9450 2200
F 0 "SW?" H 9450 2435 50  0001 C CNN
F 1 "Danforth Thermostat" V 9150 2300 50  0000 C CNN
F 2 "" H 9450 2200 50  0001 C CNN
F 3 "~" H 9450 2200 50  0001 C CNN
	1    9450 2200
	0    -1   -1   0   
$EndComp
Text Notes 5900 2100 2    50   ~ 0
RED
Text Notes 6400 2100 2    50   ~ 0
RED
Wire Wire Line
	5900 1950 5750 1950
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66D3B1B3
P 5550 1950
F 0 "J?" H 5630 1992 50  0001 L CNN
F 1 "asdfg" H 5630 1946 50  0001 L CNN
F 2 "" H 5550 1950 50  0001 C CNN
F 3 "~" H 5550 1950 50  0001 C CNN
	1    5550 1950
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66D3B1AD
P 5450 1950
F 0 "J?" H 5530 1992 50  0001 L CNN
F 1 "asdfg" H 5530 1946 50  0001 L CNN
F 2 "" H 5450 1950 50  0001 C CNN
F 3 "~" H 5450 1950 50  0001 C CNN
	1    5450 1950
	1    0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66D3B1A6
P 6900 1950
F 0 "J?" H 6980 1992 50  0001 L CNN
F 1 "asdfg" H 6980 1946 50  0001 L CNN
F 2 "" H 6900 1950 50  0001 C CNN
F 3 "~" H 6900 1950 50  0001 C CNN
	1    6900 1950
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 66D3B1A0
P 6800 1950
F 0 "J?" H 6880 1992 50  0001 L CNN
F 1 "asdfg" H 6880 1946 50  0001 L CNN
F 2 "" H 6800 1950 50  0001 C CNN
F 3 "~" H 6800 1950 50  0001 C CNN
	1    6800 1950
	1    0    0    1   
$EndComp
Text Notes 6150 2100 2    50   ~ 0
10W
$Comp
L Device:R R?
U 1 1 66D3B199
P 6050 1950
F 0 "R?" V 5950 1950 50  0001 C CNN
F 1 "1.5K" V 6050 1950 50  0000 C CNN
F 2 "" V 5980 1950 50  0001 C CNN
F 3 "~" H 6050 1950 50  0001 C CNN
	1    6050 1950
	0    -1   1    0   
$EndComp
Text Notes 4850 2600 0    50   ~ 0
heavy\nsingle\nbrowns
Wire Wire Line
	4650 2150 4950 2200
Wire Wire Line
	4650 2100 5250 1950
Text Notes 2750 2250 2    59   ~ 0
R\n\nB
Text Notes 3250 2450 2    50   ~ 0
18GA PAIR\nTO THERM\nCONTROL
Wire Notes Line
	4500 2200 4500 2050
Wire Notes Line
	2800 2050 2800 2200
Wire Wire Line
	2350 2100 4650 2100
Wire Wire Line
	1950 1950 2350 2100
Wire Wire Line
	1950 2150 4650 2150
Text Notes 8600 1850 2    50   ~ 0
shiny black\nw/white stripe pair\n
Wire Notes Line
	2500 1550 2500 1400
Wire Notes Line
	4500 1550 2500 1550
Wire Notes Line
	4500 1400 4500 1550
Wire Notes Line
	2500 1400 4500 1400
Wire Wire Line
	2200 1500 4650 1500
Wire Wire Line
	1950 1550 2200 1500
Wire Wire Line
	1950 1450 4650 1450
Text Notes 1650 2200 2    61   ~ 0
BATT-\nBATT+\nFAN/DIODE+\nFAN-\nDIODE-\nC\nP\nT
Text Notes 3200 1400 2    50   ~ 0
12G PAIR\nFROM ELEC\nCOMPART
Wire Notes Line
	950  2950 950  1000
Wire Notes Line
	1700 1000 1700 2950
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 66E497FB
P 1750 1750
F 0 "J?" H 1668 1125 50  0001 C CNN
F 1 "Inverter" H 1668 2175 50  0000 C CNN
F 2 "" H 1750 1750 50  0001 C CNN
F 3 "~" H 1750 1750 50  0001 C CNN
	1    1750 1750
	-1   0    0    -1  
$EndComp
Text Notes 5750 4150 2    50   ~ 0
grey w/black\nstripe pairs
Text Notes 6450 4950 0    50   ~ 0
to 3D \nprinted\nlamp
Wire Wire Line
	6300 4600 5900 4600
Wire Wire Line
	6300 4500 6300 4600
Wire Wire Line
	6200 4500 6300 4500
Wire Wire Line
	6300 4200 6300 4300
Wire Wire Line
	6200 4200 6300 4200
$Comp
L Device:LED D?
U 1 1 6701D081
P 6050 4500
F 0 "D?" H 6043 4245 50  0001 C CNN
F 1 "BLUE" H 6050 4400 50  0000 C CNN
F 2 "" H 6050 4500 50  0001 C CNN
F 3 "~" H 6050 4500 50  0001 C CNN
	1    6050 4500
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D?
U 1 1 6701D087
P 6050 4200
F 0 "D?" H 6043 3945 50  0001 C CNN
F 1 "RED" H 6050 4100 50  0000 C CNN
F 2 "" H 6050 4200 50  0001 C CNN
F 3 "~" H 6050 4200 50  0001 C CNN
	1    6050 4200
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 67035B9C
P 2350 2700
F 0 "J?" V 2222 2880 50  0001 L CNN
F 1 "myController" V 2350 3050 50  0000 L CNN
F 2 "" H 2350 2700 50  0001 C CNN
F 3 "~" H 2350 2700 50  0001 C CNN
	1    2350 2700
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 6709AC74
P 2350 3900
F 0 "J?" V 2222 4080 50  0001 L CNN
F 1 "myController" V 2300 4250 50  0000 L CNN
F 2 "" H 2350 3900 50  0001 C CNN
F 3 "~" H 2350 3900 50  0001 C CNN
	1    2350 3900
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 670E7033
P 2750 4500
F 0 "J?" H 2830 4542 50  0001 L CNN
F 1 "LEDS" H 2650 4250 50  0000 L CNN
F 2 "" H 2750 4500 50  0001 C CNN
F 3 "~" H 2750 4500 50  0001 C CNN
	1    2750 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 1850 2150 1850
Wire Wire Line
	2150 1850 2150 2500
Wire Wire Line
	1950 1750 2250 1750
Wire Wire Line
	2250 1750 2250 2500
Wire Wire Line
	1950 1650 2350 1650
Wire Wire Line
	1950 1550 2450 1600
Wire Wire Line
	2450 1600 2450 2500
Connection ~ 1950 1550
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 6720059F
P 4600 4500
F 0 "J?" H 4680 4542 50  0001 L CNN
F 1 "LEDS" H 4500 4250 50  0000 L CNN
F 2 "" H 4600 4500 50  0001 C CNN
F 3 "~" H 4600 4500 50  0001 C CNN
	1    4600 4500
	-1   0    0    -1  
$EndComp
Text Notes 2900 4661 0    61   ~ 0
BATT+\nFAN+\nFAN-
Text Notes 3350 4650 2    59   ~ 0
R\nG\nB
Wire Notes Line
	4500 2050 2800 2050
Wire Notes Line
	4500 2200 2800 2200
Wire Wire Line
	6300 4300 5900 4300
Wire Wire Line
	4800 4500 5900 4500
Wire Wire Line
	4800 4600 5050 4550
Wire Wire Line
	5050 4550 5700 4550
Wire Wire Line
	5700 4550 5900 4600
Wire Wire Line
	4800 4400 5000 4200
Wire Wire Line
	5000 4200 5900 4200
Wire Wire Line
	5900 4300 5750 4250
Wire Wire Line
	5750 4250 5100 4250
Wire Wire Line
	5100 4250 4800 4600
Connection ~ 4800 4600
Wire Wire Line
	4800 4400 5100 4800
Connection ~ 4800 4400
Wire Wire Line
	4800 4600 5100 4850
Wire Wire Line
	5100 4800 6300 4800
Wire Wire Line
	5100 4850 6300 4850
Wire Wire Line
	2450 4400 2450 4100
Wire Wire Line
	2350 5150 2150 5150
Wire Wire Line
	2150 5150 2150 5050
Wire Wire Line
	2150 4750 2150 4100
Wire Wire Line
	2350 4100 2350 4500
Wire Wire Line
	2250 4100 2250 4600
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 674B3F89
P 3450 4500
F 0 "J?" H 3530 4542 50  0001 L CNN
F 1 "LEDS" H 3350 4750 50  0000 L CNN
F 2 "" H 3450 4500 50  0001 C CNN
F 3 "~" H 3450 4500 50  0001 C CNN
	1    3450 4500
	-1   0    0    1   
$EndComp
Wire Wire Line
	3650 4400 3850 4450
Wire Wire Line
	3850 4450 4350 4450
Wire Wire Line
	4350 4450 4550 4400
Wire Wire Line
	3650 4500 4550 4500
Wire Wire Line
	3650 4600 3850 4550
Wire Wire Line
	3850 4550 4350 4550
Wire Wire Line
	4350 4550 4550 4600
Wire Notes Line
	3900 4400 3900 4600
Wire Notes Line
	3900 4600 4300 4600
Wire Notes Line
	4300 4600 4300 4400
Wire Notes Line
	4300 4400 3900 4400
Wire Wire Line
	2550 4400 2450 4400
Wire Wire Line
	2550 4500 2350 4500
Connection ~ 2350 4500
Wire Wire Line
	2350 4500 2350 5150
Wire Wire Line
	2550 4600 2250 4600
Wire Wire Line
	2350 5350 2350 5150
Connection ~ 2350 5150
Wire Wire Line
	2250 5350 2250 4600
Connection ~ 2250 4600
Text Notes 2200 3050 0    50   ~ 0
Four\nConductor\nCable
Text Notes 3950 4300 0    50   ~ 0
Three\nConductor\nExisting
Wire Notes Line
	3800 5100 3800 1100
Wire Notes Line
	10050 1100 10050 5100
Text Notes 7450 3800 0    197  ~ 0
EXISTS
Text Notes 2000 4500 2    50   ~ 0
BATT+\ncould be separately\nfused 2A slow on controler
Wire Notes Line
	950  2950 1700 2950
Wire Notes Line
	950  1000 1700 1000
Wire Notes Line
	10050 5100 3800 5100
Wire Notes Line
	3800 1100 10050 1100
Wire Wire Line
	9250 2000 9450 2000
Wire Wire Line
	9450 2400 9250 2400
Text Notes 2550 6000 2    50   ~ 0
0.5A 12V max
Wire Wire Line
	2350 1650 2350 2500
Text Notes 850  3450 0    50   ~ 0
If there was a water pump, a \nrelay would be added from the \nfan terminals that would then\ndrive both the fan and the water\npump from the BATT+
$EndSCHEMATC
