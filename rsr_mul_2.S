﻿
/*
 * rsr_mul_2.s
 *
 * Created: 2023-05-21 오후 3:12:45
 *  Author: mingi
 */ 
#include "common.i"

.global rsr_mul_2
.type rsr_mul_2, @function

#define L0 R19
#define L1 R18
#define L2 R17
#define L3 R16

// reduction 2
rsr_mul_2:
	PUSH_REGS
	MOVW R28, R24	// Y = R
	MOVW R30, R22	// Z = L
	MOVW R26, R18	// X = M, 나중에 이 값 Y에 옮겨줌
	
	LDD R2, Y+12
	LDD R3, Y+13
	LDD R4, Y+14
	LDD R5, Y+15
	LDD R6, Y+16
	LDD R7, Y+17
	LDD R8, Y+18
	LDD R9, Y+19
	LDD R10, Y+20
	LDD R11, Y+21
	LDD R12, Y+22
	LDD R13, Y+23
	CLR R14	// CARRY 를 위한 레지스터 0으로 세팅
	CLR R15	// BURROW 를 위한 레지스터 0으로 세팅
	CLC		// CARRY FLAG 0으로 세팅

	LDD L0, Z+0
	LDD L1, Z+1
	LDD L2, Z+2
	LDD L3, Z+3

	ADD R2, L0		// L0,L1,L2,L3,0,...,0,CARRY1
	ADC R3, L1
	ADC R4, L2
	ADC R5, L3
	ADC R6, R1
	ADC R7, R1
	ADC R8, R1
	ADC R9, R1
	ADC R10, R1
	ADC R11, R1
	ADC R12, R1
	ADC R13, R1
	ADC R14, R1

	ADD R2, L0		// L0,L1,L2,L3,0,...,0,CARRY2
	ADC R3, L1
	ADC R4, L2
	ADC R5, L3
	ADC R6, R1
	ADC R7, R1
	ADC R8, R1
	ADC R9, R1
	ADC R10, R1
	ADC R11, R1
	ADC R12, R1
	ADC R13, R1
	ADC R14, R1

	ADD R2, L0		// L0,L1,L2,L3,0,...,0,CARRY3
	ADC R3, L1
	ADC R4, L2
	ADC R5, L3
	ADC R6, R1
	ADC R7, R1
	ADC R8, R1
	ADC R9, R1
	ADC R10, R1
	ADC R11, R1
	ADC R12, R1
	ADC R13, R1
	ADC R14, R1

	ADD R2, L0		// L0,L1,L2,L3,0,...,0,CARRY4
	ADC R3, L1
	ADC R4, L2
	ADC R5, L3
	ADC R6, R1
	ADC R7, R1
	ADC R8, R1
	ADC R9, R1
	ADC R10, R1
	ADC R11, R1
	ADC R12, R1
	ADC R13, R1
	ADC R14, R1

	LDD R0, Z+4		// L4,L5,L6,L7,0,...,0,CARRY5
	ADD R2, R0
	LDD R0, Z+5
	ADC R3, R0
	LDD R0, Z+6
	ADC R4, R0
	LDD R0, Z+7
	ADC R5, R0
	ADC R6, R1
	ADC R7, R1
	ADC R8, R1
	ADC R9, R1
	ADC R10, R1
	ADC R11, R1
	ADC R12, R1
	ADC R13, R1
	ADC R14, R1

	LDD R0, Z+4		// L4,L5,L6,L7,0,...,0,CARRY6
	ADD R2, R0
	LDD R0, Z+5
	ADC R3, R0
	LDD R0, Z+6
	ADC R4, R0
	LDD R0, Z+7
	ADC R5, R0
	ADC R6, R1
	ADC R7, R1
	ADC R8, R1
	ADC R9, R1
	ADC R10, R1
	ADC R11, R1
	ADC R12, R1
	ADC R13, R1
	ADC R14, R1

	LDD R0, Z+20	// L20,L21,L22,L23,0,...,0,BURROW1
	SUB R2, R0
	LDD R0, Z+21
	SBC R3, R0
	LDD R0, Z+22
	SBC	R4, R0
	LDD R0, Z+23
	SBC R5, R0
	SBC R6, R1
	SBC R7, R1
	SBC R8, R1
	SBC R9, R1
	SBC R10, R1
	SBC R11, R1
	SBC R12, R1
	SBC R13, R1
	ADC R15, R1	

	LDD R0, Z+16	// L16,L17,L18,L19,0,...,0,BURROW2
	SUB R2, R0
	LDD R0, Z+17
	SBC R3, R0
	LDD R0, Z+18
	SBC	R4, R0
	LDD R0, Z+19
	SBC R5, R0
	SBC R6, R1
	SBC R7, R1
	SBC R8, R1
	SBC R9, R1
	SBC R10, R1
	SBC R11, R1
	SBC R12, R1
	SBC R13, R1
	ADC R15, R1	

	LDD R0, Z+12	// L12,L13,L14,L15,0,...,0,BURROW3
	SUB R2, R0
	LDD R0, Z+13
	SBC R3, R0
	LDD R0, Z+14
	SBC	R4, R0
	LDD R0, Z+15
	SBC R5, R0
	SBC R6, R1
	SBC R7, R1
	SBC R8, R1
	SBC R9, R1
	SBC R10, R1
	SBC R11, R1
	SBC R12, R1
	SBC R13, R1
	ADC R15, R1	

	MOVW R30, R20	// Z = H, 0,0,0,0,H24,...,H31,CARRY7
	LDD R0, Z+24
	ADD R6, R0
	LDD R0, Z+25
	ADC R7, R0
	LDD R0, Z+26
	ADC R8, R0
	LDD R0, Z+27
	ADC R9, R0
	LDD R0, Z+28
	ADC R10, R0
	LDD R0, Z+29
	ADC R11, R0
	LDD R0, Z+30
	ADC R12, R0
	LDD R0, Z+31
	ADC R13, R0
	ADC R14, R1		

	LDD R0, Z+24	// H24,...,H35,CARRY8
	ADD R2, R0
	LDD R0, Z+25
	ADC R3, R0
	LDD R0, Z+26
	ADC R4, R0
	LDD R0, Z+27
	ADC R5, R0
	LDD R0, Z+28
	ADC R6, R0
	LDD R0, Z+29
	ADC R7, R0
	LDD R0, Z+30
	ADC R8, R0
	LDD R0, Z+31
	ADC R9, R0
	LDD R0, Z+32
	ADC R10, R0
	LDD R0, Z+33
	ADC R11, R0
	LDD R0, Z+34
	ADC R12, R0
	LDD R0, Z+35
	ADC R13, R0
	ADC R14, R1

	LDD R0, Z+32	// H32,...,H43,BURROW4
	SUB R2, R0
	LDD R0, Z+33
	SBC R3, R0
	LDD R0, Z+34
	SBC R4, R0
	LDD R0, Z+35
	SBC R5, R0
	LDD R0, Z+36
	SBC R6, R0
	LDD R0, Z+37
	SBC R7, R0
	LDD R0, Z+38
	SBC R8, R0
	LDD R0, Z+39
	SBC R9, R0
	LDD R0, Z+40
	SBC R10, R0
	LDD R0, Z+41
	SBC R11, R0
	LDD R0, Z+42
	SBC R12, R0
	LDD R0, Z+43
	SBC R13, R0
	ADC R15, R1

	MOVW R30, R26	// Z = M
	LDD R0, Z+12	// M12,...,M23,BURROW5
	SUB R2, R0
	LDD R0, Z+13
	SBC R3, R0
	LDD R0, Z+14
	SBC R4, R0
	LDD R0, Z+15
	SBC R5, R0
	LDD R0, Z+16
	SBC R6, R0
	LDD R0, Z+17
	SBC R7, R0
	LDD R0, Z+18
	SBC R8, R0
	LDD R0, Z+19
	SBC R9, R0
	LDD R0, Z+20
	SBC R10, R0
	LDD R0, Z+21
	SBC R11, R0
	LDD R0, Z+22
	SBC R12, R0
	LDD R0, Z+23
	SBC R13, R0
	ADC R15, R1

	LDD R0, Y+48	// R0~R11 CARRY 처리
	ADD R2, R0
	ADC R3, R1
	ADC R4, R1
	ADC R5, R1
	ADC R6, R1
	ADC R7, R1
	ADC R8, R1
	ADC R9, R1
	ADC R10, R1
	ADC R11, R1
	ADC R12, R1
	ADC R13, R1
	ADC R14, R1

	LDD R0, Y+49	// R0~R11 BURROW 처리
	SUB R2, R0
	SBC R3, R1
	SBC R4, R1
	SBC R5, R1
	SBC R6, R1
	SBC R7, R1
	SBC R8, R1
	SBC R9, R1
	SBC R10, R1
	SBC R11, R1
	SBC R12, R1
	SBC R13, R1
	ADC R15, R1

	// R14, R15에 있는 CARRY/BURROW 처리해주기
	STD Y+12, R2
	STD Y+13, R3
	STD Y+14, R4
	STD Y+15, R5
	STD Y+16, R6
	STD Y+17, R7
	STD Y+18, R8
	STD Y+19, R9
	STD Y+20, R10
	STD Y+21, R11
	STD Y+22, R12
	STD Y+23, R13
	LDD R0, Y+50	// R[50]에 있는 CARRY 가져오기
	ADD R14, R0
	STD Y+50, R14	// CARRY 저장
	LDD R0, Y+51	// R[51]에 있는 BURROW 가져오기
	ADD R15, R0
	STD Y+51, R15	// BURROW 저장

	POP_REGS
	
RET