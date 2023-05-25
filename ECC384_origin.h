#include <stdio.h>
#include <stdint.h>
#include <string.h>

// uint32_t -> unsigned int : 4 Byte , 32 Bit -> 16진수 8개 묶음
#define P384 12

typedef struct {
    uint32_t a[P384];
    int e;
} Bignum;

typedef struct {
    Bignum X;
    Bignum Y;
    char infinitypoint;
} ECCnum;

typedef struct {
    Bignum X;
    Bignum Y;
    Bignum Z;
    char infinitypoint;
} ECCnum_J;

void SetBignum(Bignum* X, uint32_t inputdata[]);

void PrintBignum(Bignum* X, const char* name);

void Bn_Addition(Bignum* X, Bignum* Y, Bignum* Z);

void Bn_Subtraction(Bignum* X, Bignum* Y, Bignum* Z);

void Fp_Addition(Bignum* X, Bignum* Y, Bignum* Z, Bignum* Prime);

void Fp_Subtraction(Bignum* X, Bignum* Y, Bignum* Z, Bignum* Prime);

void OS_Multiplication(Bignum* X, Bignum* Y, Bignum* Z, Bignum* prime, uint32_t* test);

void PS_Multiplication(Bignum* X, Bignum* Y, Bignum* Z, Bignum* prime, uint32_t* test);

void Fp_Square(Bignum* X, Bignum* Z, Bignum* prime, uint32_t test[]);

void FastReduction_P384(Bignum* X, uint32_t C[], Bignum* prime);

void Fp_InverseFLT(Bignum* Z, Bignum* T, Bignum* prime);

void SetEccnum(ECCnum* A, uint32_t inputdata_X[], uint32_t inputdata_Y[]);

void PrintECCnum(ECCnum* A, const char* name);

void ECADD(ECCnum* P, ECCnum* Q, ECCnum* R, Bignum* prime);

void ECDBL(ECCnum* P, ECCnum* R, Bignum* a, Bignum* prime);

void ECLTR(ECCnum* P, Bignum* k, ECCnum* R, Bignum* a, Bignum* prime);

void ECRTL(ECCnum* P, Bignum* k, ECCnum* R, Bignum* a, Bignum* prime);

void SetEccnum_J(ECCnum_J* A, uint32_t inputdata_X[], uint32_t inputdata_Y[], uint32_t inputdata_Z[]);

void PrintECCnum_J(ECCnum_J* A, const char* name);

void From_A_to_J(ECCnum* A, ECCnum_J* B);

void From_J_to_A(ECCnum_J* A, ECCnum* B, Bignum* prime);

void ECDBL_J(ECCnum_J* P, ECCnum_J* R, Bignum* a, Bignum* prime);

void ECADD_J(ECCnum_J* P, ECCnum* Q, ECCnum_J* R, Bignum* a, Bignum* prime);

void ECLTR_J(ECCnum_J *P, Bignum *k, ECCnum_J *R, Bignum *a, Bignum *prime);

void P384_Multiplication(Bignum *X, Bignum *Y, Bignum *Z, Bignum *prime, uint32_t *test);