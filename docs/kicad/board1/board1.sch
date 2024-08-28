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
Text GLabel 3900 2200 2    50   Input ~ 0
S_PLUS
Connection ~ 3250 2100
$Comp
L power:GND #PWR01
U 1 1 66C82859
P 6400 950
F 0 "#PWR01" H 6400 700 50  0001 C CNN
F 1 "GND" V 6400 750 50  0000 C CNN
F 2 "" H 6400 950 50  0001 C CNN
F 3 "" H 6400 950 50  0001 C CNN
	1    6400 950 
	0    -1   1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 66C81EEA
P 5950 950
F 0 "R1" V 5850 950 50  0001 C CNN
F 1 "220" V 5950 950 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 5880 950 50  0001 C CNN
F 3 "~" H 5950 950 50  0001 C CNN
	1    5950 950 
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR09
U 1 1 66C7A52F
P 3550 2100
F 0 "#PWR09" H 3550 1850 50  0001 C CNN
F 1 "GND" V 3550 1900 50  0000 C CNN
F 2 "" H 3550 2100 50  0001 C CNN
F 3 "" H 3550 2100 50  0001 C CNN
	1    3550 2100
	0    -1   1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 66C7A184
P 3400 2100
F 0 "R6" V 3300 2100 50  0001 C CNN
F 1 "2K" V 3400 2100 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3330 2100 50  0001 C CNN
F 3 "~" H 3400 2100 50  0001 C CNN
	1    3400 2100
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 66C796DC
P 3100 2100
F 0 "R5" V 3000 2100 50  0001 C CNN
F 1 "10K" V 3100 2100 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3030 2100 50  0001 C CNN
F 3 "~" H 3100 2100 50  0001 C CNN
	1    3100 2100
	0    1    1    0   
$EndComp
$Comp
L Device:Q_NPN_CBE Q1
U 1 1 66C61D51
P 2900 3350
F 0 "Q1" H 3090 3396 50  0001 L CNN
F 1 "BC547" H 3100 3350 50  0000 L CNN
F 2 "0_my_footprints2:transistor" H 3100 3450 50  0001 C CNN
F 3 "~" H 2900 3350 50  0001 C CNN
	1    2900 3350
	-1   0    0    1   
$EndComp
$Comp
L Device:R R11
U 1 1 66C61D50
P 3250 3350
F 0 "R11" V 3150 3300 50  0001 L CNN
F 1 "4.7K" V 3250 3350 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3180 3350 50  0001 C CNN
F 3 "~" H 3250 3350 50  0001 C CNN
	1    3250 3350
	0    1    1    0   
$EndComp
Text GLabel 2550 3150 0    50   Input ~ 0
C2
$Comp
L cnc3018_Library:BUCK01 M1
U 1 1 61B26C43
P 2950 1250
F 0 "M1" H 3100 1250 50  0001 C CNN
F 1 "BUCK01" H 2850 1250 50  0000 C CNN
F 2 "0_my_footprints2:buck-mini360" H 2900 1550 50  0001 C CNN
F 3 "" H 2900 1550 50  0001 C CNN
	1    2950 1250
	1    0    0    1   
$EndComp
$Comp
L Switch:SW_DIP_x01 SW4
U 1 1 61DE95D4
P 6050 5300
F 0 "SW4" H 6050 5550 50  0001 C CNN
F 1 "BUTTON3" H 6050 5450 50  0000 C CNN
F 2 "0_my_footprints2:button_6mm_H12" H 6050 5300 50  0001 C CNN
F 3 "~" H 6050 5300 50  0001 C CNN
	1    6050 5300
	-1   0    0    -1  
$EndComp
$Comp
L Switch:SW_DIP_x01 SW3
U 1 1 61D638ED
P 6050 4900
F 0 "SW3" H 6050 5150 50  0001 C CNN
F 1 "BUTTON2" H 6050 5050 50  0000 C CNN
F 2 "0_my_footprints2:button_6mm_H12" H 6050 4900 50  0001 C CNN
F 3 "~" H 6050 4900 50  0001 C CNN
	1    6050 4900
	-1   0    0    -1  
$EndComp
$Comp
L power:+5V #PWR02
U 1 1 61D92C26
P 3750 1100
F 0 "#PWR02" H 3750 950 50  0001 C CNN
F 1 "+5V" V 3750 1300 50  0000 C CNN
F 2 "" H 3750 1100 50  0001 C CNN
F 3 "" H 3750 1100 50  0001 C CNN
	1    3750 1100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 61D9E980
P 3300 1400
F 0 "#PWR07" H 3300 1150 50  0001 C CNN
F 1 "GND" H 3305 1227 50  0000 C CNN
F 2 "" H 3300 1400 50  0001 C CNN
F 3 "" H 3300 1400 50  0001 C CNN
	1    3300 1400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR021
U 1 1 61DD0190
P 4000 4600
F 0 "#PWR021" H 4000 4450 50  0001 C CNN
F 1 "+5V" V 4000 4800 50  0000 C CNN
F 2 "" H 4000 4600 50  0001 C CNN
F 3 "" H 4000 4600 50  0001 C CNN
	1    4000 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR022
U 1 1 61DD1643
P 4200 4600
F 0 "#PWR022" H 4200 4350 50  0001 C CNN
F 1 "GND" V 4200 4400 50  0000 C CNN
F 2 "" H 4200 4600 50  0001 C CNN
F 3 "" H 4200 4600 50  0001 C CNN
	1    4200 4600
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR023
U 1 1 61DD26D1
P 4800 4600
F 0 "#PWR023" H 4800 4450 50  0001 C CNN
F 1 "+5V" V 4800 4800 50  0000 C CNN
F 2 "" H 4800 4600 50  0001 C CNN
F 3 "" H 4800 4600 50  0001 C CNN
	1    4800 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 61DFC811
P 2550 1400
F 0 "#PWR06" H 2550 1150 50  0001 C CNN
F 1 "GND" H 2555 1227 50  0000 C CNN
F 2 "" H 2550 1400 50  0001 C CNN
F 3 "" H 2550 1400 50  0001 C CNN
	1    2550 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C2
U 1 1 61F8466C
P 5050 2550
F 0 "C2" H 5168 2596 50  0001 L CNN
F 1 "100uf" H 4700 2550 50  0000 L CNN
F 2 "0_my_footprints2:CP_my100uf" H 5088 2400 50  0001 C CNN
F 3 "~" H 5050 2550 50  0001 C CNN
	1    5050 2550
	1    0    0    -1  
$EndComp
Text GLabel 5750 4900 0    50   Input ~ 0
BUTTON2
Text GLabel 4100 4600 1    50   Input ~ 0
EXT_LED
Text GLabel 4700 4600 1    50   Input ~ 0
SDA
Text GLabel 4600 4600 1    50   Input ~ 0
SCL
Text GLabel 5750 5300 0    50   Input ~ 0
BUTTON3
Text GLabel 5850 3500 3    50   Input ~ 0
PWM
Text GLabel 6450 2500 1    50   Input ~ 0
S_PLUS
Text GLabel 6550 2500 1    50   Input ~ 0
S_FAN
Text GLabel 6350 3500 3    50   Input ~ 0
SDA
Text GLabel 6650 3500 3    50   Input ~ 0
SCL
Text GLabel 5550 2500 1    50   Input ~ 0
EXT_LED
$Comp
L power:+3.3V #PWR017
U 1 1 61D8580D
P 5350 3500
F 0 "#PWR017" H 5350 3350 50  0001 C CNN
F 1 "+3.3V" V 5350 3750 50  0000 C CNN
F 2 "" H 5350 3500 50  0001 C CNN
F 3 "" H 5350 3500 50  0001 C CNN
	1    5350 3500
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR018
U 1 1 61D31B08
P 5450 3500
F 0 "#PWR018" H 5450 3250 50  0001 C CNN
F 1 "GND" H 5450 3150 50  0001 C CNN
F 2 "" H 5450 3500 50  0001 C CNN
F 3 "" H 5450 3500 50  0001 C CNN
	1    5450 3500
	1    0    0    -1  
$EndComp
$Comp
L cnc3018_Library:ESP32_DEV_0 U1
U 1 1 61B0E9CA
P 6150 3050
F 0 "U1" H 6000 3250 50  0000 L CNN
F 1 "ESP32_DEV_0" H 5800 3150 50  0000 L CNN
F 2 "0_my_footprints2:esp32_dev_0" H 5600 3150 50  0001 C CNN
F 3 "" H 5600 3150 50  0001 C CNN
	1    6150 3050
	1    0    0    -1  
$EndComp
Text GLabel 6150 2500 1    50   Input ~ 0
BUTTON2
Text GLabel 5650 2500 1    50   Input ~ 0
BUTTON1
$Comp
L power:+5V #PWR010
U 1 1 61E93031
P 5350 2300
F 0 "#PWR010" H 5350 2150 50  0001 C CNN
F 1 "+5V" V 5350 2500 50  0000 C CNN
F 2 "" H 5350 2300 50  0001 C CNN
F 3 "" H 5350 2300 50  0001 C CNN
	1    5350 2300
	1    0    0    -1  
$EndComp
Text GLabel 6650 2500 1    50   Input ~ 0
S_DIODE
$Comp
L Connector_Generic:Conn_01x03 J5
U 1 1 63C2EBA7
P 4100 4800
F 0 "J5" H 4300 4750 50  0001 L CNN
F 1 "EXT_LED" V 4250 4650 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 4100 4800 50  0001 C CNN
F 3 "~" H 4100 4800 50  0001 C CNN
	1    4100 4800
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J6
U 1 1 63C83A53
P 4800 4800
F 0 "J6" H 5100 4650 50  0001 C CNN
F 1 "LCD" V 4950 4750 50  0000 C CNN
F 2 "0_my_footprints2:myLCD_2x16" H 4800 4800 50  0001 C CNN
F 3 "~" H 4800 4800 50  0001 C CNN
	1    4800 4800
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR024
U 1 1 61DD1C37
P 4900 4600
F 0 "#PWR024" H 4900 4350 50  0001 C CNN
F 1 "GND" V 4900 4400 50  0000 C CNN
F 2 "" H 4900 4600 50  0001 C CNN
F 3 "" H 4900 4600 50  0001 C CNN
	1    4900 4600
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR028
U 1 1 66CDFAE3
P 6350 5450
F 0 "#PWR028" H 6350 5200 50  0001 C CNN
F 1 "GND" H 6355 5277 50  0000 C CNN
F 2 "" H 6350 5450 50  0001 C CNN
F 3 "" H 6350 5450 50  0001 C CNN
	1    6350 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 4900 6350 5300
Connection ~ 6350 5300
Wire Wire Line
	6350 5300 6350 5450
$Comp
L Device:CP C1
U 1 1 61F3EFF1
P 7000 2500
F 0 "C1" H 7250 2550 50  0001 R CNN
F 1 "4.7uf" V 6850 2600 50  0000 R CNN
F 2 "0_my_footprints:CP_my10uf" H 7038 2350 50  0001 C CNN
F 3 "~" H 7000 2500 50  0001 C CNN
	1    7000 2500
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR014
U 1 1 66D25FC4
P 7150 2600
F 0 "#PWR014" H 7150 2350 50  0001 C CNN
F 1 "GND" V 7150 2400 50  0000 C CNN
F 2 "" H 7150 2600 50  0001 C CNN
F 3 "" H 7150 2600 50  0001 C CNN
	1    7150 2600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6750 2500 6850 2500
Wire Wire Line
	7150 2500 7150 2600
$Comp
L 0_my_symbols:SD_TYP0 M2
U 1 1 66CCAE1D
P 7650 4600
F 0 "M2" H 8078 4613 50  0001 L CNN
F 1 "SD_TYP0" V 7300 4450 50  0000 L CNN
F 2 "0_my_footprints:SDCardReader" H 7450 4750 50  0001 C CNN
F 3 "" H 7450 4750 50  0001 C CNN
	1    7650 4600
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR016
U 1 1 61E1944A
P 5050 2700
F 0 "#PWR016" H 5050 2450 50  0001 C CNN
F 1 "GND" H 5055 2527 50  0000 C CNN
F 2 "" H 5050 2700 50  0001 C CNN
F 3 "" H 5050 2700 50  0001 C CNN
	1    5050 2700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 66CD78A0
P 1550 2350
F 0 "#PWR011" H 1550 2100 50  0001 C CNN
F 1 "GND" V 1550 2150 50  0000 C CNN
F 2 "" H 1550 2350 50  0001 C CNN
F 3 "" H 1550 2350 50  0001 C CNN
	1    1550 2350
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR013
U 1 1 66CDA219
P 5450 2500
F 0 "#PWR013" H 5450 2250 50  0001 C CNN
F 1 "GND" V 5450 2300 50  0000 C CNN
F 2 "" H 5450 2500 50  0001 C CNN
F 3 "" H 5450 2500 50  0001 C CNN
	1    5450 2500
	1    0    0    1   
$EndComp
Text GLabel 1550 2450 2    50   Input ~ 0
FAN_DIODE+
Text GLabel 1550 2550 2    50   Input ~ 0
FAN-
Text GLabel 1550 2650 2    50   Input ~ 0
DIAG_DIODE
Text GLabel 1550 2750 2    50   Input ~ 0
C2
Text GLabel 1550 2850 2    50   Input ~ 0
T
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 66CF29D6
P 1350 1300
F 0 "J1" H 1268 1517 50  0001 C CNN
F 1 "12V_IN" H 1350 1000 50  0000 C CNN
F 2 "0_my_footprints:myJSTx02" H 1350 1300 50  0001 C CNN
F 3 "~" H 1350 1300 50  0001 C CNN
	1    1350 1300
	-1   0    0    1   
$EndComp
$Comp
L Switch:SW_SPDT SW1
U 1 1 66CFD48E
P 2150 1200
F 0 "SW1" H 2150 1485 50  0001 C CNN
F 1 "ON/OFF" H 2150 1393 50  0000 C CNN
F 2 "0_my_footprints2:SW_DPDT_BLUE_LEVER" H 2150 1200 50  0001 C CNN
F 3 "~" H 2150 1200 50  0001 C CNN
	1    2150 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 1200 1950 1200
Wire Wire Line
	1950 1200 1950 1300
Connection ~ 1950 1200
Wire Wire Line
	1950 1300 2350 1300
Wire Wire Line
	2350 1100 2550 1100
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 66D2CE52
P 3500 1300
F 0 "J2" H 3418 1517 50  0001 C CNN
F 1 "5V_JUMP" V 3650 1250 50  0000 C CNN
F 2 "0_my_footprints2:pinHeader1x2" H 3500 1300 50  0001 C CNN
F 3 "~" H 3500 1300 50  0001 C CNN
	1    3500 1300
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR020
U 1 1 66D5AB9F
P 6950 4400
F 0 "#PWR020" H 6950 4150 50  0001 C CNN
F 1 "GND" V 6950 4200 50  0000 C CNN
F 2 "" H 6950 4400 50  0001 C CNN
F 3 "" H 6950 4400 50  0001 C CNN
	1    6950 4400
	0    1    -1   0   
$EndComp
Text GLabel 6950 4500 0    50   Input ~ 0
MISO
Text GLabel 6950 4600 0    50   Input ~ 0
SCLK
Text GLabel 6950 4700 0    50   Input ~ 0
MOSI
Text GLabel 6950 4800 0    50   Input ~ 0
SD_CS
$Comp
L power:+3.3V #PWR027
U 1 1 66D7933C
P 7350 5000
F 0 "#PWR027" H 7350 4850 50  0001 C CNN
F 1 "+3.3V" V 7350 5250 50  0000 C CNN
F 2 "" H 7350 5000 50  0001 C CNN
F 3 "" H 7350 5000 50  0001 C CNN
	1    7350 5000
	-1   0    0    1   
$EndComp
$Comp
L Device:R R12
U 1 1 66D7BDE5
P 7200 4900
F 0 "R12" V 7100 4900 50  0001 C CNN
F 1 "10K" V 7200 4900 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 7130 4900 50  0001 C CNN
F 3 "~" H 7200 4900 50  0001 C CNN
	1    7200 4900
	0    1    1    0   
$EndComp
Wire Wire Line
	7350 5000 7350 4900
Connection ~ 7350 4900
Wire Wire Line
	7050 4900 7050 4500
Wire Wire Line
	7050 4500 7350 4500
Wire Wire Line
	6950 4800 7350 4800
Wire Wire Line
	6950 4700 7350 4700
Wire Wire Line
	6950 4600 7350 4600
Wire Wire Line
	6950 4500 7050 4500
Connection ~ 7050 4500
Wire Wire Line
	6950 4400 7350 4400
Text GLabel 6250 3500 3    50   Input ~ 0
MISO
Text GLabel 6150 3500 3    50   Input ~ 0
SCLK
Text GLabel 6750 3500 3    50   Input ~ 0
MOSI
Text GLabel 6050 3500 3    50   Input ~ 0
SD_CS
Wire Wire Line
	3300 1100 3500 1100
Wire Wire Line
	5350 2300 5350 2400
Wire Wire Line
	3750 1100 3600 1100
Text GLabel 2950 2100 0    50   Input ~ 0
FAN_DIODE+
Wire Wire Line
	3250 2200 3250 2100
Text GLabel 3900 2500 2    50   Input ~ 0
S_FAN
Connection ~ 3250 2400
$Comp
L power:GND #PWR012
U 1 1 66E1AC0E
P 3550 2400
F 0 "#PWR012" H 3550 2150 50  0001 C CNN
F 1 "GND" V 3550 2200 50  0000 C CNN
F 2 "" H 3550 2400 50  0001 C CNN
F 3 "" H 3550 2400 50  0001 C CNN
	1    3550 2400
	0    -1   1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 66E1AC14
P 3400 2400
F 0 "R8" V 3300 2400 50  0001 C CNN
F 1 "2K" V 3400 2400 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3330 2400 50  0001 C CNN
F 3 "~" H 3400 2400 50  0001 C CNN
	1    3400 2400
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 66E1AC1A
P 3100 2400
F 0 "R7" V 3000 2400 50  0001 C CNN
F 1 "10K" V 3100 2400 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3030 2400 50  0001 C CNN
F 3 "~" H 3100 2400 50  0001 C CNN
	1    3100 2400
	0    1    1    0   
$EndComp
Text GLabel 2700 2400 0    50   Input ~ 0
FAN-
Wire Wire Line
	3250 2500 3250 2400
Text GLabel 3900 2800 2    50   Input ~ 0
S_DIODE
Connection ~ 3250 2700
$Comp
L power:GND #PWR015
U 1 1 66E1B869
P 3550 2700
F 0 "#PWR015" H 3550 2450 50  0001 C CNN
F 1 "GND" V 3550 2500 50  0000 C CNN
F 2 "" H 3550 2700 50  0001 C CNN
F 3 "" H 3550 2700 50  0001 C CNN
	1    3550 2700
	0    -1   1    0   
$EndComp
$Comp
L Device:R R10
U 1 1 66E1B86F
P 3400 2700
F 0 "R10" V 3300 2700 50  0001 C CNN
F 1 "2K" V 3400 2700 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3330 2700 50  0001 C CNN
F 3 "~" H 3400 2700 50  0001 C CNN
	1    3400 2700
	0    1    1    0   
$EndComp
$Comp
L Device:R R9
U 1 1 66E1B875
P 3100 2700
F 0 "R9" V 3000 2700 50  0001 C CNN
F 1 "10K" V 3100 2700 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 3030 2700 50  0001 C CNN
F 3 "~" H 3100 2700 50  0001 C CNN
	1    3100 2700
	0    1    1    0   
$EndComp
Text GLabel 2900 2700 0    50   Input ~ 0
DIAG_DIODE
Wire Wire Line
	3250 2800 3250 2700
Wire Wire Line
	2700 2400 2950 2400
Wire Wire Line
	2900 2700 2950 2700
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 66E5C146
P 1700 4300
F 0 "J4" H 1618 4517 50  0001 C CNN
F 1 "MECH_THERM" H 1780 4246 50  0000 L CNN
F 2 "0_my_footprints:myJSTx02" H 1700 4300 50  0001 C CNN
F 3 "~" H 1700 4300 50  0001 C CNN
	1    1700 4300
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR019
U 1 1 66E5FDB9
P 2050 4400
F 0 "#PWR019" H 2050 4250 50  0001 C CNN
F 1 "+3.3V" V 2050 4650 50  0000 C CNN
F 2 "" H 2050 4400 50  0001 C CNN
F 3 "" H 2050 4400 50  0001 C CNN
	1    2050 4400
	0    1    -1   0   
$EndComp
Text GLabel 1900 4300 2    50   Input ~ 0
MECH_THERM
Wire Wire Line
	1900 4400 2050 4400
Text GLabel 5550 3500 3    50   Input ~ 0
MECH_THERM
Text Notes 2700 5400 2    50   ~ 0
DS18B20
$Comp
L Connector_Generic:Conn_01x03 J7
U 1 1 66E723EC
P 1700 4850
F 0 "J7" H 1900 4800 50  0001 L CNN
F 1 "TEMP1" H 1800 4850 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 1700 4850 50  0001 C CNN
F 3 "~" H 1700 4850 50  0001 C CNN
	1    1700 4850
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J8
U 1 1 66E750BA
P 1700 5250
F 0 "J8" H 1900 5200 50  0001 L CNN
F 1 "TEMP2" H 1800 5250 50  0000 L CNN
F 2 "0_my_footprints2:JST3" H 1700 5250 50  0001 C CNN
F 3 "~" H 1700 5250 50  0001 C CNN
	1    1700 5250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1900 4750 2000 4750
Wire Wire Line
	1900 4850 2100 4850
Wire Wire Line
	1900 4950 2200 4950
Wire Wire Line
	1900 5150 2000 5150
Wire Wire Line
	1900 5250 2100 5250
Wire Wire Line
	1900 5350 2200 5350
Wire Wire Line
	2000 5150 2000 4750
Connection ~ 2000 4750
Wire Wire Line
	2000 4750 2350 4750
Wire Wire Line
	2100 5250 2100 4850
Connection ~ 2100 4850
Wire Wire Line
	2100 4850 2350 4850
Wire Wire Line
	2200 5350 2200 4950
Connection ~ 2200 4950
Wire Wire Line
	2200 4950 2350 4950
$Comp
L power:+3.3V #PWR026
U 1 1 66EA164B
P 2350 4750
F 0 "#PWR026" H 2350 4600 50  0001 C CNN
F 1 "+3.3V" V 2350 5000 50  0000 C CNN
F 2 "" H 2350 4750 50  0001 C CNN
F 3 "" H 2350 4750 50  0001 C CNN
	1    2350 4750
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR025
U 1 1 66EA4989
P 2350 4850
F 0 "#PWR025" H 2350 4600 50  0001 C CNN
F 1 "GND" V 2350 4650 50  0000 C CNN
F 2 "" H 2350 4850 50  0001 C CNN
F 3 "" H 2350 4850 50  0001 C CNN
	1    2350 4850
	0    -1   -1   0   
$EndComp
Text GLabel 2350 4950 2    50   Input ~ 0
ONE_WIRE
Text GLabel 5750 3500 3    50   Input ~ 0
ONE_WIRE
$Comp
L power:GND #PWR03
U 1 1 66EAE27F
P 6400 1150
F 0 "#PWR03" H 6400 900 50  0001 C CNN
F 1 "GND" V 6400 950 50  0000 C CNN
F 2 "" H 6400 1150 50  0001 C CNN
F 3 "" H 6400 1150 50  0001 C CNN
	1    6400 1150
	0    -1   1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 66EAE285
P 5950 1150
F 0 "R2" V 5850 1150 50  0001 C CNN
F 1 "220" V 5950 1150 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 5880 1150 50  0001 C CNN
F 3 "~" H 5950 1150 50  0001 C CNN
	1    5950 1150
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 66EAF596
P 6400 1350
F 0 "#PWR04" H 6400 1100 50  0001 C CNN
F 1 "GND" V 6400 1150 50  0000 C CNN
F 2 "" H 6400 1350 50  0001 C CNN
F 3 "" H 6400 1350 50  0001 C CNN
	1    6400 1350
	0    -1   1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 66EAF59C
P 5950 1350
F 0 "R3" V 5850 1350 50  0001 C CNN
F 1 "220" V 5950 1350 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 5880 1350 50  0001 C CNN
F 3 "~" H 5950 1350 50  0001 C CNN
	1    5950 1350
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR08
U 1 1 66EB07F4
P 6400 1550
F 0 "#PWR08" H 6400 1300 50  0001 C CNN
F 1 "GND" V 6400 1350 50  0000 C CNN
F 2 "" H 6400 1550 50  0001 C CNN
F 3 "" H 6400 1550 50  0001 C CNN
	1    6400 1550
	0    -1   1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 66EB07FA
P 5950 1550
F 0 "R4" V 5850 1550 50  0001 C CNN
F 1 "220" V 5950 1550 50  0000 C CNN
F 2 "0_my_footprints2:resistor" V 5880 1550 50  0001 C CNN
F 3 "~" H 5950 1550 50  0001 C CNN
	1    5950 1550
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_DIP_x01 SW2
U 1 1 66EE3FD7
P 6050 4500
F 0 "SW2" H 6050 4750 50  0001 C CNN
F 1 "BUTTON1" H 6050 4650 50  0000 C CNN
F 2 "0_my_footprints2:button_6mm_H12" H 6050 4500 50  0001 C CNN
F 3 "~" H 6050 4500 50  0001 C CNN
	1    6050 4500
	-1   0    0    -1  
$EndComp
Text GLabel 5750 4500 0    50   Input ~ 0
BUTTON1
Wire Wire Line
	6350 4500 6350 4900
Connection ~ 6350 4900
Text GLabel 5650 950  0    50   Input ~ 0
LED_GREEN
Text GLabel 5650 1150 0    50   Input ~ 0
LED_YELLOW
Text GLabel 5650 1350 0    50   Input ~ 0
LED_RED
Text GLabel 5650 1550 0    50   Input ~ 0
LED_BLUE
Text GLabel 6250 2500 1    50   Input ~ 0
BUTTON3
Text GLabel 5750 2500 1    50   Input ~ 0
LED_GREEN
Text GLabel 5850 2500 1    50   Input ~ 0
LED_YELLOW
Text GLabel 5950 2500 1    50   Input ~ 0
LED_RED
Text GLabel 6050 2500 1    50   Input ~ 0
LED_BLUE
Text GLabel 3500 3350 2    50   Input ~ 0
PWM
Wire Wire Line
	2550 3150 2800 3150
Text GLabel 2600 3550 0    50   Input ~ 0
T
Wire Wire Line
	3250 2200 3900 2200
Wire Wire Line
	3250 2500 3900 2500
Wire Wire Line
	3250 2800 3900 2800
Wire Wire Line
	3400 3350 3500 3350
Wire Wire Line
	2600 3550 2800 3550
Text Notes 1700 1200 0    50   ~ 0
12V
$Comp
L power:GND #PWR05
U 1 1 670512B6
P 1550 1400
F 0 "#PWR05" H 1550 1150 50  0001 C CNN
F 1 "GND" H 1555 1227 50  0000 C CNN
F 2 "" H 1550 1400 50  0001 C CNN
F 3 "" H 1550 1400 50  0001 C CNN
	1    1550 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 1300 1550 1400
Wire Wire Line
	5050 2400 5350 2400
Connection ~ 5350 2400
Wire Wire Line
	5350 2400 5350 2500
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 67132439
P 1350 2550
F 0 "J3" H 1268 2967 50  0001 C CNN
F 1 "MINI_BOX" H 1268 2875 50  0000 C CNN
F 2 "0_my_footprints2:myJST6_right" H 1350 2550 50  0001 C CNN
F 3 "~" H 1350 2550 50  0001 C CNN
	1    1350 2550
	-1   0    0    -1  
$EndComp
Text Notes 1250 2900 2    63   ~ 0
black\norange\nyellow\ngreen\nblue\nwhite
Wire Wire Line
	5650 1550 5800 1550
Wire Wire Line
	5650 1350 5800 1350
Wire Wire Line
	5650 1150 5800 1150
Wire Wire Line
	5650 950  5800 950 
$Comp
L Device:LED D4
U 1 1 66EB0800
P 6250 1550
F 0 "D4" H 6243 1295 50  0001 C CNN
F 1 "BLUE" H 5650 1550 50  0000 C CNN
F 2 "0_my_footprints2:led_small_blue" H 6250 1550 50  0001 C CNN
F 3 "~" H 6250 1550 50  0001 C CNN
	1    6250 1550
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D3
U 1 1 66EAF5A2
P 6250 1350
F 0 "D3" H 6243 1095 50  0001 C CNN
F 1 "RED" H 5650 1350 50  0000 C CNN
F 2 "0_my_footprints2:led_small_red" H 6250 1350 50  0001 C CNN
F 3 "~" H 6250 1350 50  0001 C CNN
	1    6250 1350
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D2
U 1 1 66EAE28B
P 6250 1150
F 0 "D2" H 6243 895 50  0001 C CNN
F 1 "YELLOW" H 5600 1150 50  0000 C CNN
F 2 "0_my_footprints2:led_small_yellow" H 6250 1150 50  0001 C CNN
F 3 "~" H 6250 1150 50  0001 C CNN
	1    6250 1150
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D1
U 1 1 66C8152F
P 6250 950
F 0 "D1" H 6243 695 50  0001 C CNN
F 1 "GREEN" H 5600 950 50  0000 C CNN
F 2 "0_my_footprints2:led_small_green" H 6250 950 50  0001 C CNN
F 3 "~" H 6250 950 50  0001 C CNN
	1    6250 950 
	-1   0    0    1   
$EndComp
$EndSCHEMATC
