EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 12
Title "ClockIOT"
Date "2020-01-27"
Rev "rev 5"
Comp "www.wyolum.com"
Comment1 "https://github.com/wyolum/ClockIOT"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 1075 1400 0    60   ~ 0
DI_D
Text HLabel 950  2350 0    60   Input ~ 12
GND
$Comp
L ClockIOT:APA102 U49
U 1 1 56C2744C
P 1700 1600
AR Path="/56C2744C" Ref="U49"  Part="1" 
AR Path="/542543F9/56C2744C" Ref="U49"  Part="1" 
F 0 "U49" H 1700 1900 40  0000 C CNB
F 1 "APA102" H 1700 1275 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 1600 2000 60  0001 C CNN
F 3 "" H 1700 1600 60  0001 C CNN
F 4 "mfr_pn" H 1700 1600 60  0001 C CNN "manf#"
	1    1700 1600
	1    0    0    -1  
$EndComp
Text Label 1225 2350 0    60   ~ 0
GND
Text Label 1075 2850 0    60   ~ 0
5V
$Comp
L ClockIOT:GND #PWR07
U 1 1 56C27465
P 1150 2400
F 0 "#PWR07" H 1150 2400 30  0001 C CNN
F 1 "GND" H 1150 2330 30  0001 C CNN
F 2 "" H 1150 2400 60  0001 C CNN
F 3 "" H 1150 2400 60  0001 C CNN
F 4 "Value" H 1150 2400 50  0001 C CNN "manf#"
	1    1150 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 1800 1150 2350
Wire Wire Line
	950  2350 1150 2350
Wire Wire Line
	10800 3100 10800 1600
Wire Wire Line
	950  2850 1800 2850
Connection ~ 1150 2350
Text HLabel 950  2850 0    60   Input ~ 12
5V
$Comp
L ClockIOT:APA102 U50
U 1 1 56C27477
P 2900 1600
AR Path="/56C27477" Ref="U50"  Part="1" 
AR Path="/542543F9/56C27477" Ref="U50"  Part="1" 
F 0 "U50" H 2900 1900 40  0000 C CNB
F 1 "APA102" H 2900 1275 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 2800 2000 60  0001 C CNN
F 3 "" H 2900 1600 60  0001 C CNN
F 4 "mfr_pn" H 2900 1600 60  0001 C CNN "manf#"
	1    2900 1600
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U51
U 1 1 56C2747F
P 4100 1600
AR Path="/56C2747F" Ref="U51"  Part="1" 
AR Path="/542543F9/56C2747F" Ref="U51"  Part="1" 
F 0 "U51" H 4100 1900 40  0000 C CNB
F 1 "APA102" H 4100 1275 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 4000 2000 60  0001 C CNN
F 3 "" H 4100 1600 60  0001 C CNN
F 4 "mfr_pn" H 4100 1600 60  0001 C CNN "manf#"
	1    4100 1600
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U52
U 1 1 56C27487
P 5300 1600
AR Path="/56C27487" Ref="U52"  Part="1" 
AR Path="/542543F9/56C27487" Ref="U52"  Part="1" 
F 0 "U52" H 5300 1900 40  0000 C CNB
F 1 "APA102" H 5300 1275 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 5200 2000 60  0001 C CNN
F 3 "" H 5300 1600 60  0001 C CNN
F 4 "mfr_pn" H 5300 1600 60  0001 C CNN "manf#"
	1    5300 1600
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U53
U 1 1 56C2748F
P 6500 1600
AR Path="/56C2748F" Ref="U53"  Part="1" 
AR Path="/542543F9/56C2748F" Ref="U53"  Part="1" 
F 0 "U53" H 6500 1900 40  0000 C CNB
F 1 "APA102" H 6500 1275 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 6400 2000 60  0001 C CNN
F 3 "" H 6500 1600 60  0001 C CNN
F 4 "mfr_pn" H 6500 1600 60  0001 C CNN "manf#"
	1    6500 1600
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U54
U 1 1 56C27497
P 7700 1600
AR Path="/56C27497" Ref="U54"  Part="1" 
AR Path="/542543F9/56C27497" Ref="U54"  Part="1" 
F 0 "U54" H 7700 1900 40  0000 C CNB
F 1 "APA102" H 7700 1275 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 7600 2000 60  0001 C CNN
F 3 "" H 7700 1600 60  0001 C CNN
F 4 "mfr_pn" H 7700 1600 60  0001 C CNN "manf#"
	1    7700 1600
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U55
U 1 1 56C2749F
P 8900 1600
AR Path="/56C2749F" Ref="U55"  Part="1" 
AR Path="/542543F9/56C2749F" Ref="U55"  Part="1" 
F 0 "U55" H 8900 1900 40  0000 C CNB
F 1 "APA102" H 8900 1275 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 8800 2000 60  0001 C CNN
F 3 "" H 8900 1600 60  0001 C CNN
F 4 "mfr_pn" H 8900 1600 60  0001 C CNN "manf#"
	1    8900 1600
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U56
U 1 1 56C274A7
P 10100 1600
AR Path="/56C274A7" Ref="U56"  Part="1" 
AR Path="/542543F9/56C274A7" Ref="U56"  Part="1" 
F 0 "U56" H 10100 1900 40  0000 C CNB
F 1 "APA102" H 10100 1275 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 10000 2000 60  0001 C CNN
F 3 "" H 10100 1600 60  0001 C CNN
F 4 "mfr_pn" H 10100 1600 60  0001 C CNN "manf#"
	1    10100 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 1400 2500 1400
Wire Wire Line
	3300 1400 3700 1400
Wire Wire Line
	2100 1600 2500 1600
Wire Wire Line
	3300 1600 3700 1600
Wire Wire Line
	4500 1400 4900 1400
Wire Wire Line
	4500 1600 4900 1600
Wire Wire Line
	5700 1400 6100 1400
Wire Wire Line
	5700 1600 6100 1600
Wire Wire Line
	6900 1400 7300 1400
Wire Wire Line
	6900 1600 7300 1600
Wire Wire Line
	8100 1400 8500 1400
Wire Wire Line
	8100 1600 8500 1600
Wire Wire Line
	9300 1400 9700 1400
Wire Wire Line
	9300 1600 9700 1600
Wire Wire Line
	1300 1800 1150 1800
Wire Wire Line
	2500 1800 2350 1800
Wire Wire Line
	2350 1800 2350 2350
Connection ~ 2350 2350
Wire Wire Line
	2100 1800 2250 1800
Wire Wire Line
	2250 1800 2250 2850
Connection ~ 2250 2850
Wire Wire Line
	3300 1800 3450 1800
Wire Wire Line
	3450 1800 3450 2850
Connection ~ 3450 2850
Wire Wire Line
	3700 1800 3550 1800
Wire Wire Line
	3550 1800 3550 2350
Connection ~ 3550 2350
Wire Wire Line
	4500 1800 4650 1800
Wire Wire Line
	4650 1800 4650 2850
Connection ~ 4650 2850
Wire Wire Line
	4900 1800 4750 1800
Wire Wire Line
	4750 1800 4750 2350
Connection ~ 4750 2350
Wire Wire Line
	5700 1800 5850 1800
Wire Wire Line
	5850 1800 5850 2850
Connection ~ 5850 2850
Wire Wire Line
	6100 1800 5950 1800
Wire Wire Line
	5950 1800 5950 2350
Connection ~ 5950 2350
Wire Wire Line
	6900 1800 7050 1800
Wire Wire Line
	7050 1800 7050 2850
Connection ~ 7050 2850
Wire Wire Line
	7300 1800 7150 1800
Wire Wire Line
	7150 1800 7150 2350
Connection ~ 7150 2350
Wire Wire Line
	8100 1800 8250 1800
Wire Wire Line
	8250 1800 8250 2850
Connection ~ 8250 2850
Wire Wire Line
	8500 1800 8350 1800
Wire Wire Line
	8350 1800 8350 2350
Connection ~ 8350 2350
Wire Wire Line
	9300 1800 9450 1800
Wire Wire Line
	9450 1800 9450 2850
Wire Wire Line
	9700 1800 9550 1800
Wire Wire Line
	9550 1800 9550 2350
Wire Wire Line
	10500 1800 10650 1800
Wire Wire Line
	10650 1800 10650 2850
Connection ~ 9450 2850
Wire Wire Line
	10800 1600 10500 1600
Wire Wire Line
	10500 1400 10950 1400
Wire Wire Line
	10950 1400 10950 3350
Wire Wire Line
	10950 3350 950  3350
Text HLabel 950  1400 0    60   Input ~ 12
DI_D
Text HLabel 950  1600 0    60   Input ~ 12
CI_D
Wire Wire Line
	1300 1400 950  1400
Wire Wire Line
	1300 1600 950  1600
Text Label 1075 1600 0    60   ~ 0
CI_D
Text HLabel 950  5450 0    60   Output ~ 12
CI_E
$Comp
L ClockIOT:APA102 U57
U 1 1 5A4F4C64
P 1700 3950
AR Path="/5A4F4C64" Ref="U57"  Part="1" 
AR Path="/542543F9/5A4F4C64" Ref="U57"  Part="1" 
F 0 "U57" H 1700 4250 40  0000 C CNB
F 1 "APA102" H 1700 3625 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 1600 4350 60  0001 C CNN
F 3 "" H 1700 3950 60  0001 C CNN
F 4 "mfr_pn" H 1700 3950 60  0001 C CNN "manf#"
	1    1700 3950
	1    0    0    -1  
$EndComp
Text Label 950  4700 0    60   ~ 0
GND
Text Label 950  5200 0    60   ~ 0
5V
Text Label 1075 5450 0    60   ~ 0
CI_E
$Comp
L ClockIOT:GND #PWR08
U 1 1 5A4F4C6E
P 1150 4750
F 0 "#PWR08" H 1150 4750 30  0001 C CNN
F 1 "GND" H 1150 4680 30  0001 C CNN
F 2 "" H 1150 4750 60  0001 C CNN
F 3 "" H 1150 4750 60  0001 C CNN
F 4 "Value" H 1150 4750 50  0001 C CNN "manf#"
	1    1150 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 4150 1150 4700
Wire Wire Line
	950  5450 10800 5450
Wire Wire Line
	950  4700 1150 4700
Wire Wire Line
	10800 5450 10800 3950
Wire Wire Line
	950  5200 1800 5200
Connection ~ 1150 4700
$Comp
L ClockIOT:APA102 U58
U 1 1 5A4F4C7C
P 2900 3950
AR Path="/5A4F4C7C" Ref="U58"  Part="1" 
AR Path="/542543F9/5A4F4C7C" Ref="U58"  Part="1" 
F 0 "U58" H 2900 4250 40  0000 C CNB
F 1 "APA102" H 2900 3625 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 2800 4350 60  0001 C CNN
F 3 "" H 2900 3950 60  0001 C CNN
F 4 "mfr_pn" H 2900 3950 60  0001 C CNN "manf#"
	1    2900 3950
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U59
U 1 1 5A4F4C83
P 4100 3950
AR Path="/5A4F4C83" Ref="U59"  Part="1" 
AR Path="/542543F9/5A4F4C83" Ref="U59"  Part="1" 
F 0 "U59" H 4100 4250 40  0000 C CNB
F 1 "APA102" H 4100 3625 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 4000 4350 60  0001 C CNN
F 3 "" H 4100 3950 60  0001 C CNN
F 4 "mfr_pn" H 4100 3950 60  0001 C CNN "manf#"
	1    4100 3950
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U60
U 1 1 5A4F4C8A
P 5300 3950
AR Path="/5A4F4C8A" Ref="U60"  Part="1" 
AR Path="/542543F9/5A4F4C8A" Ref="U60"  Part="1" 
F 0 "U60" H 5300 4250 40  0000 C CNB
F 1 "APA102" H 5300 3625 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 5200 4350 60  0001 C CNN
F 3 "" H 5300 3950 60  0001 C CNN
F 4 "mfr_pn" H 5300 3950 60  0001 C CNN "manf#"
	1    5300 3950
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U61
U 1 1 5A4F4C91
P 6500 3950
AR Path="/5A4F4C91" Ref="U61"  Part="1" 
AR Path="/542543F9/5A4F4C91" Ref="U61"  Part="1" 
F 0 "U61" H 6500 4250 40  0000 C CNB
F 1 "APA102" H 6500 3625 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 6400 4350 60  0001 C CNN
F 3 "" H 6500 3950 60  0001 C CNN
F 4 "mfr_pn" H 6500 3950 60  0001 C CNN "manf#"
	1    6500 3950
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U62
U 1 1 5A4F4C98
P 7700 3950
AR Path="/5A4F4C98" Ref="U62"  Part="1" 
AR Path="/542543F9/5A4F4C98" Ref="U62"  Part="1" 
F 0 "U62" H 7700 4250 40  0000 C CNB
F 1 "APA102" H 7700 3625 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 7600 4350 60  0001 C CNN
F 3 "" H 7700 3950 60  0001 C CNN
F 4 "mfr_pn" H 7700 3950 60  0001 C CNN "manf#"
	1    7700 3950
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U63
U 1 1 5A4F4C9F
P 8900 3950
AR Path="/5A4F4C9F" Ref="U63"  Part="1" 
AR Path="/542543F9/5A4F4C9F" Ref="U63"  Part="1" 
F 0 "U63" H 8900 4250 40  0000 C CNB
F 1 "APA102" H 8900 3625 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 8800 4350 60  0001 C CNN
F 3 "" H 8900 3950 60  0001 C CNN
F 4 "mfr_pn" H 8900 3950 60  0001 C CNN "manf#"
	1    8900 3950
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:APA102 U64
U 1 1 5A4F4CA6
P 10100 3950
AR Path="/5A4F4CA6" Ref="U64"  Part="1" 
AR Path="/542543F9/5A4F4CA6" Ref="U64"  Part="1" 
F 0 "U64" H 10100 4250 40  0000 C CNB
F 1 "APA102" H 10100 3625 40  0000 C CNN
F 2 "ClockIOT_libs:APA102" H 10000 4350 60  0001 C CNN
F 3 "" H 10100 3950 60  0001 C CNN
F 4 "mfr_pn" H 10100 3950 60  0001 C CNN "manf#"
	1    10100 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 3750 2500 3750
Wire Wire Line
	3300 3750 3700 3750
Wire Wire Line
	2100 3950 2500 3950
Wire Wire Line
	3300 3950 3700 3950
Wire Wire Line
	4500 3750 4900 3750
Wire Wire Line
	4500 3950 4900 3950
Wire Wire Line
	5700 3750 6100 3750
Wire Wire Line
	5700 3950 6100 3950
Wire Wire Line
	6900 3750 7300 3750
Wire Wire Line
	6900 3950 7300 3950
Wire Wire Line
	8100 3750 8500 3750
Wire Wire Line
	8100 3950 8500 3950
Wire Wire Line
	9300 3750 9700 3750
Wire Wire Line
	9300 3950 9700 3950
Wire Wire Line
	1300 4150 1150 4150
Wire Wire Line
	2500 4150 2350 4150
Wire Wire Line
	2350 4150 2350 4700
Connection ~ 2350 4700
Wire Wire Line
	2100 4150 2250 4150
Wire Wire Line
	2250 4150 2250 5200
Connection ~ 2250 5200
Wire Wire Line
	3300 4150 3450 4150
Wire Wire Line
	3450 4150 3450 5200
Connection ~ 3450 5200
Wire Wire Line
	3700 4150 3550 4150
Wire Wire Line
	3550 4150 3550 4700
Connection ~ 3550 4700
Wire Wire Line
	4500 4150 4650 4150
Wire Wire Line
	4650 4150 4650 5200
Connection ~ 4650 5200
Wire Wire Line
	4900 4150 4750 4150
Wire Wire Line
	4750 4150 4750 4700
Connection ~ 4750 4700
Wire Wire Line
	5700 4150 5850 4150
Wire Wire Line
	5850 4150 5850 5200
Connection ~ 5850 5200
Wire Wire Line
	6100 4150 5950 4150
Wire Wire Line
	5950 4150 5950 4700
Connection ~ 5950 4700
Wire Wire Line
	6900 4150 7050 4150
Wire Wire Line
	7050 4150 7050 5200
Connection ~ 7050 5200
Wire Wire Line
	7300 4150 7150 4150
Wire Wire Line
	7150 4150 7150 4700
Connection ~ 7150 4700
Wire Wire Line
	8100 4150 8250 4150
Wire Wire Line
	8250 4150 8250 5200
Connection ~ 8250 5200
Wire Wire Line
	8500 4150 8350 4150
Wire Wire Line
	8350 4150 8350 4700
Connection ~ 8350 4700
Wire Wire Line
	9300 4150 9450 4150
Wire Wire Line
	9450 4150 9450 5200
Wire Wire Line
	9700 4150 9550 4150
Wire Wire Line
	9550 4150 9550 4700
Wire Wire Line
	10500 4150 10650 4150
Wire Wire Line
	10650 4150 10650 5200
Connection ~ 9450 5200
Wire Wire Line
	10800 3950 10500 3950
Wire Wire Line
	10500 3750 10950 3750
Wire Wire Line
	10950 3750 10950 5700
Wire Wire Line
	10950 5700 950  5700
Text HLabel 950  5700 0    60   Output ~ 12
DI_E
Wire Wire Line
	1300 3750 950  3750
Text Label 1075 5700 0    60   ~ 0
DI_E
Wire Wire Line
	950  3750 950  3350
Wire Wire Line
	750  3100 750  3950
Wire Wire Line
	750  3100 10800 3100
Wire Wire Line
	750  3950 1300 3950
$Comp
L ClockIOT:C_NP C7
U 1 1 5A8F08D6
P 1800 2600
F 0 "C7" H 1893 2645 50  0000 L CNN
F 1 "100nF" H 1893 2555 50  0000 L CNN
F 2 "ClockIOT_libs:c_0805" H 1800 2600 60  0001 C CNN
F 3 "" H 1800 2600 60  0000 C CNN
F 4 "Value" H 1800 2600 50  0001 C CNN "manf#"
	1    1800 2600
	1    0    0    -1  
$EndComp
$Comp
L ClockIOT:C_NP C8
U 1 1 5A8F0986
P 1800 4950
F 0 "C8" H 1893 4995 50  0000 L CNN
F 1 "100nF" H 1893 4905 50  0000 L CNN
F 2 "ClockIOT_libs:c_0805" H 1800 4950 60  0001 C CNN
F 3 "" H 1800 4950 60  0000 C CNN
F 4 "Value" H 1800 4950 50  0001 C CNN "manf#"
	1    1800 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 4700 1800 4800
Connection ~ 1800 4700
Wire Wire Line
	1800 5200 1800 5100
Connection ~ 1800 5200
Wire Wire Line
	1800 2350 1800 2450
Connection ~ 1800 2350
Wire Wire Line
	1800 2850 1800 2750
Connection ~ 1800 2850
Wire Wire Line
	1150 2350 1150 2400
Wire Wire Line
	1150 2350 1800 2350
Wire Wire Line
	2350 2350 3550 2350
Wire Wire Line
	2250 2850 3450 2850
Wire Wire Line
	3450 2850 4650 2850
Wire Wire Line
	3550 2350 4750 2350
Wire Wire Line
	4650 2850 5850 2850
Wire Wire Line
	4750 2350 5950 2350
Wire Wire Line
	5850 2850 7050 2850
Wire Wire Line
	5950 2350 7150 2350
Wire Wire Line
	7050 2850 8250 2850
Wire Wire Line
	7150 2350 8350 2350
Wire Wire Line
	8250 2850 9450 2850
Wire Wire Line
	8350 2350 9550 2350
Wire Wire Line
	9450 2850 10650 2850
Wire Wire Line
	1150 4700 1150 4750
Wire Wire Line
	1150 4700 1800 4700
Wire Wire Line
	2350 4700 3550 4700
Wire Wire Line
	2250 5200 3450 5200
Wire Wire Line
	3450 5200 4650 5200
Wire Wire Line
	3550 4700 4750 4700
Wire Wire Line
	4650 5200 5850 5200
Wire Wire Line
	4750 4700 5950 4700
Wire Wire Line
	5850 5200 7050 5200
Wire Wire Line
	5950 4700 7150 4700
Wire Wire Line
	7050 5200 8250 5200
Wire Wire Line
	7150 4700 8350 4700
Wire Wire Line
	8250 5200 9450 5200
Wire Wire Line
	8350 4700 9550 4700
Wire Wire Line
	9450 5200 10650 5200
Wire Wire Line
	1800 4700 2350 4700
Wire Wire Line
	1800 5200 2250 5200
Wire Wire Line
	1800 2350 2350 2350
Wire Wire Line
	1800 2850 2250 2850
$EndSCHEMATC
