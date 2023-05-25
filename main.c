/*
* ECC384.c
*
* Created: 2023-05-17 오후 10:59:46
* Author : mingi
*/

#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ECC384_origin.h"
//! int = 2bytes
//! 초기화 꼭 해주기
//! bignumber addition 16-bit 단위 짜봐

typedef unsigned char u8;

extern void adder(char* x, char* y, char* z);

extern void bignum_add(Bignum *a, Bignum *b, Bignum *c);

extern void rsr_mul(char *R, char *L, char *H, char *M);
extern void rsr_mul_1(char *R, char *L, char *H, char *M);
extern void rsr_mul_2(char *R, char *L, char *H, char *M);
extern void rsr_mul_3(char *R, char *L, char *H, char *M);
extern void rsr_mul_4(char *R, char *L, char *H, char *M);

//int main(void)
//{
	////u8 a[16] = {0};
	////u8 b[16] = {0};
	////u8 c[16] = {0};
	////
	////for(int cnt_i = 0 ;cnt_i < 16 ; cnt_i++)
	////{
	////a[cnt_i] = b[cnt_i] = cnt_i;
	////}
	////
	////
	////// adder(a, b, c); // c[i] = a[i] + b[i]
	////
	////Bignum aa;
	////aa.a[0] = 0x33;
	////aa.a[1] = 0x55;
	////aa.carry = 0x44;
	////
	////Bignum bb;
	////bb.a[0] = 4;
	////bb.a[1] = 5;
	////bb.carry = 6;
	////
	////Bignum cc;
	////cc.a[0] = 0;
	////cc.a[1] = 0;
	////cc.carry = 0;
	////
	////
	////bignum_add(&aa, &bb, &cc);
	////
	////c[0]++;	// 쓰레기
//}

int main(){
	char L[48] = {0x00};
	char H[48] = {0x00};
	char M[48] = {0x00};
	Bignum op_B = {{0x00}, 0x00};
	Bignum op_A = {{0x00}, 0x00};
	Bignum tmp = {{0x00}, 0x00};
	Bignum prime = {{0x00}, 0x00};
	uint32_t OSmul_tmp[24] = {0x00};
	uint32_t prime_array[P384] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF };
	SetBignum(&prime, prime_array);
	OS_Multiplication(&op_A, &op_B, &tmp, &prime, OSmul_tmp);

	for (int cnt_i = 0; cnt_i < 48; cnt_i++){
		L[cnt_i] = cnt_i % 8 + 1;
		H[cnt_i] = cnt_i % 8 + 1;
		M[cnt_i] = cnt_i % 8 + 1;
	}
	//for (int cnt_i = 0; cnt_i < 48; cnt_i++){
		//// L[cnt_i] = 0;
		//H[cnt_i] = 1;
		//// M[cnt_i] = 0;
	//}
	
	char R[56] = {0x00};	// 48개 워드 + 12개 당 carry, burrow 순차적으로 저장
	
	rsr_mul(R, L, H, M);	// l0 + h0 + l24 + h24

	// accum_rst_r(L, H, R_2, R_4);	// l12 + h12 + l36 + h36
	rsr_mul_1(R, L, H, M);
	rsr_mul_2(R, L, H, M);
	rsr_mul_3(R, L, H, M);
	rsr_mul_4(R, L, H, M);
}

