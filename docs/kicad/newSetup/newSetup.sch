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
Text Notes 2700 4750 2    61   ~ 0
BATT-\nBATT+\nFAN/DIODE+\nFAN-\nDIODE-\nC\nP\nT
Text Notes 3950 5050 2    50   ~ 0
12G PAIR\nFROM ELEC\nCOMPART
Wire Notes Line
	2000 5500 2000 3550
Wire Notes Line
	2750 3550 2750 5500
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 66E497FB
P 2800 4300
F 0 "J?" H 2718 3675 50  0001 C CNN
F 1 "Inverter" H 2718 4725 50  0000 C CNN
F 2 "" H 2800 4300 50  0001 C CNN
F 3 "~" H 2800 4300 50  0001 C CNN
	1    2800 4300
	-1   0    0    -1  
$EndComp
Wire Notes Line
	1850 4550 2600 4550
Wire Notes Line
	2000 3550 2750 3550
Text Notes 3950 4150 2    50   ~ 0
B\n\nR
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 66C6C947
P 2650 2550
F 0 "J?" H 2730 2542 50  0001 L CNN
F 1 "FAN" H 2800 2300 50  0000 L CNN
F 2 "" H 2650 2550 50  0001 C CNN
F 3 "~" H 2650 2550 50  0001 C CNN
	1    2650 2550
	-1   0    0    1   
$EndComp
Text Notes 1150 1750 0    50   ~ 0
Notes:\n(1) The inverter BATT+ is NOT run to the Controller.  No fuse is needed.\n(2) The MiniBox is sufficiient to run the Fridge.  The Controller is optional \n    and the fridge can run without it, or if it is turned off.\n(3) The Controller is powered separately so it can monitor Inverter Power\n(4) The Controller will be built to allow for two or more DS18B20\n    temperature probes. \n(5) The Controller details are undefined.  It probably duplicates the\n   MINI-BOX leds, adding a blue PumpOn Led, has at least one\n    relay, an LCD and buttons, an on/off switch, and possibly switches\n    or another relay or other devices to control the pump speed.
Wire Wire Line
	3000 4200 3200 4200
Wire Wire Line
	3200 4200 3200 3200
Wire Wire Line
	3000 4300 3250 4300
Wire Wire Line
	3250 4300 3250 3200
Wire Wire Line
	3000 4400 3300 4400
Wire Wire Line
	3300 4400 3300 3200
Wire Wire Line
	3000 4500 3350 4500
Wire Wire Line
	3350 4500 3350 3200
Wire Wire Line
	3000 4600 3400 4600
Wire Wire Line
	3400 4600 3400 3200
Wire Wire Line
	3000 4700 3450 4700
Wire Wire Line
	3450 4700 3450 3200
Wire Wire Line
	3000 4000 3650 4000
Wire Wire Line
	3800 4100 3900 4050
Wire Wire Line
	3900 4050 4100 4050
Wire Wire Line
	3000 4100 3750 4100
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 66CDA173
P 3650 3250
F 0 "J?" H 3730 3242 50  0001 L CNN
F 1 "INV_PWR" V 3650 2900 50  0000 L CNN
F 2 "" H 3650 3250 50  0001 C CNN
F 3 "~" H 3650 3250 50  0001 C CNN
	1    3650 3250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3650 3450 3650 4000
Connection ~ 3650 4000
Wire Wire Line
	3650 4000 4150 4000
Wire Wire Line
	3750 4100 3750 3450
Connection ~ 3750 4100
Wire Wire Line
	3750 4100 3800 4100
Text Notes 3900 2950 2    61   ~ 0
BATT-\nno connect\nFAN/DIODE+\nFAN-\nDIODE-\nC\nP\nT
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 66CDF232
P 4150 2500
F 0 "J?" H 4068 1875 50  0001 C CNN
F 1 "CONTROL" H 4750 3000 50  0000 C CNN
F 2 "" H 4150 2500 50  0001 C CNN
F 3 "~" H 4150 2500 50  0001 C CNN
	1    4150 2500
	1    0    0    -1  
$EndComp
Text Notes 3500 4700 0    61   ~ 0
orange\nyellow\ngreen\nblue\npurple\ngrey
Wire Wire Line
	4100 4050 4100 5050
Wire Wire Line
	4150 4000 4150 5050
Text Notes 6500 2950 0    61   ~ 0
black\nred\norange\nyellow\ngreen\nblue\npurple\nwhite
Wire Notes Line
	2700 2000 4200 2000
Wire Notes Line
	4200 2000 4200 2150
Wire Notes Line
	2600 2400 2600 2000
Wire Notes Line
	2600 2000 2650 2000
Wire Notes Line
	2600 3200 4200 3200
Wire Notes Line
	4200 3200 4200 2950
Wire Notes Line
	2600 2600 2600 3200
Wire Notes Line
	4050 3950 4050 4950
Wire Notes Line
	4050 4950 4200 4950
Wire Notes Line
	4200 4950 4200 3950
Wire Notes Line
	4200 3950 4050 3950
Text Notes 2750 2250 0    50   ~ 10
MINI\nBOX
$Comp
L Connector_Generic:Conn_01x08 J?
U 1 1 66CF39C8
P 6250 2500
F 0 "J?" H 6168 1875 50  0001 C CNN
F 1 "CONTROL" H 6500 2950 50  0000 C CNN
F 2 "" H 6250 2500 50  0001 C CNN
F 3 "~" H 6250 2500 50  0001 C CNN
	1    6250 2500
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 66CFD4FD
P 8000 2150
F 0 "J?" H 8080 2142 50  0001 L CNN
F 1 "MECH_TERMOSTAT" H 8050 2250 50  0000 L CNN
F 2 "" H 8000 2150 50  0001 C CNN
F 3 "~" H 8000 2150 50  0001 C CNN
	1    8000 2150
	1    0    0    -1  
$EndComp
$Comp
L Motor:Fan M?
U 1 1 66CFF0BA
P 1450 2600
F 0 "M?" H 1608 2696 50  0000 L CNN
F 1 "Fan" H 1608 2605 50  0000 L CNN
F 2 "" H 1450 2610 50  0001 C CNN
F 3 "~" H 1450 2610 50  0001 C CNN
	1    1450 2600
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 66D001BC
P 8000 2800
F 0 "J?" H 8080 2842 50  0001 L CNN
F 1 "EXT_LED" H 8050 3000 50  0000 L CNN
F 2 "" H 8000 2800 50  0001 C CNN
F 3 "~" H 8000 2800 50  0001 C CNN
	1    8000 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 2800 1800 2800
Wire Wire Line
	1800 2800 1950 2550
Wire Wire Line
	1950 2550 2600 2550
Wire Wire Line
	2600 2450 1950 2450
Wire Wire Line
	1950 2450 1800 2300
Wire Wire Line
	1800 2300 1450 2300
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 66D083F8
P 6850 3250
F 0 "J?" H 6930 3292 50  0001 L CNN
F 1 "TEMP1" V 6950 3450 50  0000 L CNN
F 2 "" H 6850 3250 50  0001 C CNN
F 3 "~" H 6850 3250 50  0001 C CNN
	1    6850 3250
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 66D0CD18
P 7550 3250
F 0 "J?" H 7630 3292 50  0001 L CNN
F 1 "TEMPn" V 7650 3400 50  0000 L CNN
F 2 "" H 7550 3250 50  0001 C CNN
F 3 "~" H 7550 3250 50  0001 C CNN
	1    7550 3250
	0    -1   1    0   
$EndComp
Wire Notes Line
	6250 2150 6250 1900
Wire Notes Line
	6250 1900 8000 1900
Wire Notes Line
	8000 1900 8000 2100
Wire Notes Line
	8000 2300 8000 2650
Wire Notes Line
	8000 2950 8000 3250
Wire Notes Line
	6250 3250 6250 2950
Text Notes 7000 2400 0    50   ~ 10
Fridge\nController
Text Notes 2200 3700 0    50   ~ 10
Inverter
$Comp
L LED:WS2812B D?
U 1 1 66D1AB0C
P 9350 2800
F 0 "D?" H 9694 2846 50  0001 L CNN
F 1 "EXT_LED" H 9800 2800 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 9400 2500 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 9450 2425 50  0001 L TNN
	1    9350 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 2750 8050 2700
Wire Wire Line
	8250 2850 8000 2900
Wire Wire Line
	9350 2500 9100 2500
Wire Wire Line
	9100 2500 8950 2750
Wire Wire Line
	8950 2750 8250 2750
Wire Wire Line
	8050 2800 9050 2800
Wire Wire Line
	9350 3100 9100 3100
Wire Wire Line
	9100 3100 8950 2850
Wire Wire Line
	8950 2850 8250 2850
$Comp
L Switch:SW_SPST SW?
U 1 1 66D253C7
P 9250 2150
F 0 "SW?" H 9250 2385 50  0001 C CNN
F 1 "MECH_THERMOSTAT" H 9250 2293 50  0000 C CNN
F 2 "" H 9250 2150 50  0001 C CNN
F 3 "~" H 9250 2150 50  0001 C CNN
	1    9250 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 2150 9050 2150
Wire Wire Line
	9450 2150 9450 2250
Wire Wire Line
	9450 2250 9000 2250
Wire Wire Line
	9000 2250 8950 2200
Wire Wire Line
	8950 2200 8100 2200
Wire Wire Line
	8100 2200 8050 2250
Wire Wire Line
	4200 2500 6200 2500
Wire Wire Line
	4200 2600 4400 2550
Wire Wire Line
	4400 2550 5950 2550
Wire Wire Line
	5950 2550 6200 2600
Wire Wire Line
	4200 2700 4450 2600
Wire Wire Line
	4450 2600 5950 2600
Wire Wire Line
	5950 2600 6200 2700
Wire Wire Line
	4200 2800 4450 2650
Wire Wire Line
	4450 2650 5950 2650
Wire Wire Line
	5950 2650 6200 2800
Wire Wire Line
	4200 2900 4450 2700
Wire Wire Line
	4450 2700 5950 2700
Wire Wire Line
	5950 2700 6200 2900
Wire Wire Line
	4200 2400 4400 2450
Wire Wire Line
	4400 2450 5950 2450
Wire Wire Line
	5950 2450 6200 2400
Wire Wire Line
	4200 2300 4400 2400
Wire Wire Line
	4400 2400 5900 2400
Wire Wire Line
	5900 2400 6200 2300
Wire Wire Line
	4200 2200 4450 2350
Wire Wire Line
	4450 2350 5900 2350
Wire Wire Line
	5900 2350 6200 2200
Wire Notes Line
	4550 2300 4550 2750
Wire Notes Line
	4550 2750 5850 2750
Wire Notes Line
	5850 2750 5850 2300
Wire Notes Line
	5850 2300 4550 2300
$Comp
L Sensor_Temperature:DS18B20 U?
U 1 1 66D3B4E5
P 6850 4600
F 0 "U?" H 6620 4646 50  0001 R CNN
F 1 "TEMP1" H 6950 4150 50  0000 R CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 5850 4350 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf" H 6700 4850 50  0001 C CNN
	1    6850 4600
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Temperature:DS18B20 U?
U 1 1 66D41DD3
P 7550 4600
F 0 "U?" H 7320 4646 50  0001 R CNN
F 1 "TEMPn" H 7650 4150 50  0000 R CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 6550 4350 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf" H 7400 4850 50  0001 C CNN
	1    7550 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 3300 6800 3450
Wire Wire Line
	6800 3450 6800 4300
Wire Wire Line
	6950 3300 6900 3450
Wire Wire Line
	6900 3450 6900 4300
Wire Wire Line
	7450 3300 7500 3450
Wire Wire Line
	7500 3450 7500 4300
Wire Wire Line
	7650 3300 7600 3450
Wire Wire Line
	7600 3450 7600 4300
Wire Wire Line
	6850 3300 6850 4300
Wire Wire Line
	7550 3300 7550 4300
Wire Wire Line
	6800 4300 6550 4300
Wire Wire Line
	6550 4300 6550 4900
Wire Wire Line
	6550 4900 6850 4900
Wire Wire Line
	6900 4300 7150 4300
Wire Wire Line
	7150 4300 7150 4600
Wire Notes Line
	6250 3250 6700 3250
Wire Notes Line
	7000 3250 7400 3250
Wire Notes Line
	7700 3250 8000 3250
Wire Wire Line
	7500 4300 7250 4300
Wire Wire Line
	7250 4300 7250 4900
Wire Wire Line
	7250 4900 7550 4900
Wire Wire Line
	7600 4300 7850 4300
Wire Wire Line
	7850 4300 7850 4600
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 66D8BB57
P 7100 1900
F 0 "J?" H 7180 1892 50  0001 L CNN
F 1 "POWER" V 7250 1750 50  0000 L CNN
F 2 "" H 7100 1900 50  0001 C CNN
F 3 "~" H 7100 1900 50  0001 C CNN
	1    7100 1900
	0    -1   -1   0   
$EndComp
Text Notes 4350 1350 0    50   ~ 0
The MiniBox will have GREEN (power), YELLOW (fan) and RED \n(diagnostic diode) LEDs.  It will probably have an OVERRIDE\nswtch that will bypass the Controllers's C-T connection and/or a\nswitch that adds a 1.5K resistor to that in order to allow the \npump to be run at high speed without the controller.
$EndSCHEMATC
