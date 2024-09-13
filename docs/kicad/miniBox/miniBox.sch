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
Wire Notes Line
	8450 -7250 8450 -7100
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 66C6C947
P 1300 2100
F 0 "J1" H 1380 2092 50  0001 L CNN
F 1 "FAN" H 1400 2050 50  0000 L CNN
F 2 "Connector_Phoenix_MC_HighVoltage:PhoenixContact_MC_1,5_2-G-5.08_1x02_P5.08mm_Horizontal" H 1300 2100 50  0001 C CNN
F 3 "~" H 1300 2100 50  0001 C CNN
	1    1300 2100
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 66CDA173
P 4000 5050
F 0 "J4" H 4080 5042 50  0001 L CNN
F 1 "INV_PWR" V 4000 5200 50  0000 L CNN
F 2 "Connector_Phoenix_MC_HighVoltage:PhoenixContact_MC_1,5_2-G-5.08_1x02_P5.08mm_Horizontal" H 4000 5050 50  0001 C CNN
F 3 "~" H 4000 5050 50  0001 C CNN
	1    4000 5050
	0    1    1    0   
$EndComp
Text Notes 3400 5150 3    61   ~ 0
orange\nyellow\ngreen\nblue\npurple\ngrey
$Comp
L power:GND #PWR01
U 1 1 66C82859
P 3800 2000
F 0 "#PWR01" H 3800 1750 50  0001 C CNN
F 1 "GND" V 3800 1800 50  0000 C CNN
F 2 "" H 3800 2000 50  0001 C CNN
F 3 "" H 3800 2000 50  0001 C CNN
	1    3800 2000
	0    -1   1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 66C81EEA
P 3650 2000
F 0 "R1" V 3550 2000 50  0001 C CNN
F 1 "1K" V 3650 2000 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3580 2000 50  0001 C CNN
F 3 "~" H 3650 2000 50  0001 C CNN
	1    3650 2000
	0    1    1    0   
$EndComp
$Comp
L Device:LED D1
U 1 1 66C8152F
P 3350 2000
F 0 "D1" H 3343 1745 50  0001 C CNN
F 1 "GREEN" H 3350 1900 50  0000 C CNN
F 2 "0_my_footprints2:led_small_green" H 3350 2000 50  0001 C CNN
F 3 "~" H 3350 2000 50  0001 C CNN
	1    3350 2000
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 66CA8018
P 3800 2300
F 0 "#PWR03" H 3800 2050 50  0001 C CNN
F 1 "GND" V 3800 2100 50  0000 C CNN
F 2 "" H 3800 2300 50  0001 C CNN
F 3 "" H 3800 2300 50  0001 C CNN
	1    3800 2300
	0    -1   1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 66CA801E
P 3650 2300
F 0 "R2" V 3550 2300 50  0001 C CNN
F 1 "1.2K" V 3650 2300 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3580 2300 50  0001 C CNN
F 3 "~" H 3650 2300 50  0001 C CNN
	1    3650 2300
	0    1    1    0   
$EndComp
$Comp
L Device:LED D2
U 1 1 66CA8024
P 3350 2300
F 0 "D2" H 3343 2045 50  0001 C CNN
F 1 "YELLOW" H 3350 2200 50  0000 C CNN
F 2 "0_my_footprints2:led_small_yellow" H 3350 2300 50  0001 C CNN
F 3 "~" H 3350 2300 50  0001 C CNN
	1    3350 2300
	-1   0    0    1   
$EndComp
Text GLabel 2800 2300 0    50   Input ~ 0
FAN-
Text GLabel 2900 2600 0    50   Input ~ 0
DIODE-
Text GLabel 2850 4850 1    50   Input ~ 0
FAN_DIODE+
Text GLabel 2950 4850 1    50   Input ~ 0
FAN-
Text GLabel 3050 4850 1    50   Input ~ 0
DIODE-
Text GLabel 3250 4850 1    50   Input ~ 0
P
Text GLabel 3350 4850 1    50   Input ~ 0
T
Text Notes 4050 5150 3    61   ~ 0
black\nred
$Comp
L power:GND #PWR04
U 1 1 66D0A539
P 3900 4850
F 0 "#PWR04" H 3900 4600 50  0001 C CNN
F 1 "GND" V 3900 4650 50  0000 C CNN
F 2 "" H 3900 4850 50  0001 C CNN
F 3 "" H 3900 4850 50  0001 C CNN
	1    3900 4850
	1    0    0    1   
$EndComp
Text GLabel 1500 2100 2    50   Input ~ 0
FAN-
Text GLabel 1500 2000 2    50   Input ~ 0
FAN_DIODE+
$Comp
L power:GND #PWR02
U 1 1 66D31071
P 5350 2150
F 0 "#PWR02" H 5350 1900 50  0001 C CNN
F 1 "GND" V 5350 1950 50  0000 C CNN
F 2 "" H 5350 2150 50  0001 C CNN
F 3 "" H 5350 2150 50  0001 C CNN
	1    5350 2150
	0    1    -1   0   
$EndComp
Text Notes 4050 5450 3    61   ~ 0
BATT-\nBATT+
Text GLabel 3050 2000 0    50   Input ~ 0
FAN_DIODE+
Wire Wire Line
	3050 2000 3200 2000
Text Notes 750  950  0    50   ~ 0
Notes:\n\n(1) DIODE- appears to be pulled down to GND through a 1K resistor or similar. \n    I have never needed a resistor in series with a regular 5V LED.\n    Doc says "10ma max current"
Wire Wire Line
	3100 2900 3300 2900
Text GLabel 3100 2900 0    50   Input ~ 0
FAN_DIODE+
$Comp
L Transistor_BJT:BC557 Q1
U 1 1 66D60766
P 3500 2800
F 0 "Q1" V 3828 2800 50  0001 C CNN
F 1 "BC557 PNP" V 3737 2800 50  0000 C CNN
F 2 "0_my_footprints2:transistor" H 3700 2725 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC557.pdf" H 3500 2800 50  0001 L CNN
	1    3500 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 2600 3200 2600
$Comp
L Device:LED D3
U 1 1 66CAA16D
P 3350 2600
F 0 "D3" H 3343 2345 50  0001 C CNN
F 1 "RED" H 3350 2500 50  0000 C CNN
F 2 "0_my_footprints2:led_small_red" H 3350 2600 50  0001 C CNN
F 3 "~" H 3350 2600 50  0001 C CNN
	1    3350 2600
	1    0    0    1   
$EndComp
Wire Wire Line
	2800 2300 3200 2300
Text GLabel 3900 2900 2    50   Input ~ 0
DIAG_DIODE
Wire Wire Line
	3700 2900 3900 2900
Text GLabel 5350 2250 0    50   Input ~ 0
FAN_DIODE+
Text GLabel 5350 2350 0    50   Input ~ 0
FAN-
Text GLabel 5350 2450 0    50   Input ~ 0
DIAG_DIODE
Text GLabel 5350 2650 0    50   Input ~ 0
T
Text Notes 5700 2661 0    61   ~ 0
red\nblack\norange\nyellow\ngreen\nblue\nwhite
Text GLabel 2250 3700 0    50   Input ~ 0
C
Text GLabel 4050 3600 2    50   Input ~ 0
T
Wire Wire Line
	3800 3800 3850 3800
Wire Wire Line
	3850 3800 3850 3600
Connection ~ 3850 3600
Wire Wire Line
	2950 3700 3000 3700
Wire Wire Line
	2850 3800 2950 3700
Wire Wire Line
	2650 3800 2850 3800
Wire Wire Line
	3400 3600 3850 3600
Wire Wire Line
	3400 3800 3500 3800
$Comp
L Switch:SW_SPDT SW2
U 1 1 66D9CEE6
P 3200 3700
F 0 "SW2" H 3200 3985 50  0001 C CNN
F 1 "LOW/HIGH" H 3200 3893 50  0000 C CNN
F 2 "0_my_footprints2:SlideSPDT_small" H 3200 3700 50  0001 C CNN
F 3 "~" H 3200 3700 50  0001 C CNN
	1    3200 3700
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPDT SW1
U 1 1 66D9908E
P 2450 3700
F 0 "SW1" H 2450 3985 50  0001 C CNN
F 1 "OVERRIDE" H 2450 3893 50  0000 C CNN
F 2 "0_my_footprints2:SlideSPDT_small" H 2450 3700 50  0001 C CNN
F 3 "~" H 2450 3700 50  0001 C CNN
	1    2450 3700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 66CCE3FF
P 3650 3800
F 0 "R3" V 3550 3800 50  0001 C CNN
F 1 "1.5K" V 3650 3800 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3580 3800 50  0001 C CNN
F 3 "~" H 3650 3800 50  0001 C CNN
	1    3650 3800
	0    1    1    0   
$EndComp
Text GLabel 3150 4850 1    50   Input ~ 0
C
Wire Wire Line
	3850 3600 4050 3600
Text GLabel 5350 2550 0    50   Input ~ 0
C
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 66CE9E9E
P 3050 5050
F 0 "J3" V 2922 4662 50  0001 R CNN
F 1 "INV_CONN" V 3050 5850 50  0000 R CNN
F 2 "0_my_footprints2:myJST6_right" H 3050 5050 50  0001 C CNN
F 3 "~" H 3050 5050 50  0001 C CNN
	1    3050 5050
	0    -1   1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 66E1FA47
P 3600 4300
F 0 "R4" V 3500 4300 50  0001 C CNN
F 1 "10K?" V 3600 4300 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3530 4300 50  0001 C CNN
F 3 "~" H 3600 4300 50  0001 C CNN
	1    3600 4300
	0    1    1    0   
$EndComp
Text GLabel 3450 4300 0    50   Input ~ 0
C
Text GLabel 3750 4300 2    50   Input ~ 0
P
Text Notes 3450 4200 0    50   ~ 0
OPT_RES
$Comp
L Connector_Generic:Conn_01x08 J2
U 1 1 66E30B57
P 5550 2450
F 0 "J2" H 5630 2442 50  0001 L CNN
F 1 "CONTROLLER" H 5400 1850 50  0000 L CNN
F 2 "0_my_footprints2:myJST8_right" H 5550 2450 50  0001 C CNN
F 3 "~" H 5550 2450 50  0001 C CNN
	1    5550 2450
	1    0    0    1   
$EndComp
$Comp
L power:+12V #PWR05
U 1 1 66E3C90D
P 4000 4850
F 0 "#PWR05" H 4000 4700 50  0001 C CNN
F 1 "+12V" V 4000 5050 50  0000 C CNN
F 2 "" H 4000 4850 50  0001 C CNN
F 3 "" H 4000 4850 50  0001 C CNN
	1    4000 4850
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR06
U 1 1 66E40BD2
P 5350 2050
F 0 "#PWR06" H 5350 1900 50  0001 C CNN
F 1 "+12V" V 5350 2250 50  0000 C CNN
F 2 "" H 5350 2050 50  0001 C CNN
F 3 "" H 5350 2050 50  0001 C CNN
	1    5350 2050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 66E439EF
P 5350 2750
F 0 "#PWR0101" H 5350 2500 50  0001 C CNN
F 1 "GND" V 5350 2550 50  0000 C CNN
F 2 "" H 5350 2750 50  0001 C CNN
F 3 "" H 5350 2750 50  0001 C CNN
	1    5350 2750
	0    1    -1   0   
$EndComp
$EndSCHEMATC
