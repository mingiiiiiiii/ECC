#include "ECC384_origin.h"

// #define P384 12
// 0xFFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF 00000000 00000000 FFFFFFFF

void SetBignum(Bignum *X, uint32_t inputdata[]) {
    int cnt_i;
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        X->a[cnt_i] = inputdata[(P384 - 1) - cnt_i];     // 순서 거꾸로 넣어주기
    }
    X->e = 0;
}

void PrintBignum(Bignum* X, const char* name) { // 확인용 출력함수
    int cnt_i;
    printf("Bignumber %s = ", name);
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        printf("%08X ", X->a[(P384 - 1) - cnt_i]); // 출력할땐 원래 순서대로 출력 (저장할때 반대로 저장함)
    }
    printf("\n");
    printf("e = %d\n\n", X->e);
}

int Compare(Bignum* X, Bignum* Y) {
    int cnt_i;
    for (cnt_i = 7; cnt_i >= 0; cnt_i--) {
        if (X->a[cnt_i] > Y->a[cnt_i]) {
            return 1;   // X가 클때
        }
        else if (X->a[cnt_i] < Y->a[cnt_i]) {
            return -1;  // Y가 클때
        }
    }
    return 0;   // 둘이 같을 때
}

// zero 이면 1 return
int IsZero(Bignum* X) {
    int cnt_i;
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        if (X->a[cnt_i] != 0x00000000) {
            return 0;
        }
    }
    return 1;
}

//Bignumber 덧셈 구현
void Bn_Addition(Bignum* X, Bignum* Y, Bignum* Z) {
    int cnt_i;
    int carry = 0;
    uint32_t tmp2 = 0x00;
    Bignum tmp = { {0x00}, 0x00 };
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        tmp2 = X->a[cnt_i] + Y->a[cnt_i];
        tmp.a[cnt_i] = X->a[cnt_i] + Y->a[cnt_i] + carry;
        
        if (tmp2 < X->a[cnt_i]) {
            carry = 1;
        }
        else if (tmp.a[cnt_i] < carry) {
            carry = 1;
        }
        else {
            carry = 0;
        }
    }

    if (carry == 1) {
        tmp.e = carry;
    }
    
    memcpy(Z, &tmp, sizeof(Bignum));
}

// Bignumber 뺄셈 구현
void Bn_Subtraction(Bignum* X, Bignum* Y, Bignum* Z) {
    int cnt_i;
    int burrow = 0;
    Bignum tmp = { {0x00}, 0x00 };

    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        tmp.a[cnt_i] = X->a[cnt_i] - Y->a[cnt_i] - burrow;
        if (X->a[cnt_i] < Y->a[cnt_i]) {
            burrow = 1;
        }
        else if ((X->a[cnt_i] == Y->a[cnt_i]) && (burrow == 1)) {
            burrow = 1;
        }
        else {
            burrow = 0;
        }
    }

    if (burrow == 1) {
        tmp.e = burrow;
    }

    memcpy(Z, &tmp, sizeof(Bignum));
}

// Bignumber 덧셈 후 e가 1인 경우 mod 취해주기 ( Z = Z - prime )
void Fp_Addition(Bignum* X, Bignum* Y, Bignum* Z, Bignum* prime) {
    Bn_Addition(X, Y, Z);
    if (Z->e == 1) {
        Bn_Subtraction(Z, prime, Z);
        Z->e = 0;           // 감산 진행하였으니 0으로 해주기 
    }
}

// 메모리에 넣는 Ver
// void Fp_Addition(Bignum* X, Bignum* Y, Bignum* Z, Bignum* prime) {
//     Bn_Addition(X, Y, Z);
//     Bignum tmp = { {0x00}, 0x00 };
//     if (Z->e == 1) {
//         Bn_Subtraction(Z, prime, &tmp);
//         tmp.e = 0;           // 감산 진행하였으니 0으로 해주기 
//         memcpy(Z, &tmp, sizeof(Bignum));
//     }
// }

// Bignumber 뺄셈 후 e가 1인 경우 mod 취해주기 ( Z <- Z + prime )
void Fp_Subtraction(Bignum* X, Bignum* Y, Bignum* Z, Bignum* prime) {
    Bn_Subtraction(X, Y, Z);
    if (Z->e == 1) {
        Bn_Addition(Z, prime, Z);
        Z->e = 0;           // 감산 진행하였으니 0으로 해주기 
    }
}

// 메모리에 넣는 Ver
// void Fp_Subtraction(Bignum* X, Bignum* Y, Bignum* Z, Bignum* prime) {
//     Bn_Subtraction(X, Y, Z);
//     Bignum tmp = { {0x00}, 0x00 };
//     if (Z->e == 1) {
//         Bn_Addition(Z, prime, &tmp);
//         tmp.e = 0;           // 감산 진행하였으니 0으로 해주기 
//         memcpy(Z, &tmp, sizeof(Bignum));
//     }
// }
// tmp 에 하면 왜 틀릴까 ? tmp의 e 값을 0으로 초기화 해줘야했음~!~!~

// 수정해야함 8 --> 12로 ... 16을 24로...
void OS_Multiplication(Bignum *X, Bignum *Y, Bignum *Z, Bignum *prime, uint32_t *test) {
    uint32_t C[24] = { 0x00 };
    uint32_t U = 0x00;
    uint32_t V = 0x00;
    uint64_t UV = 0x00;
    int cnt_i, cnt_j;
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        U = 0x00;
        for (cnt_j = 0; cnt_j < P384; cnt_j++) {
            UV = C[cnt_i + cnt_j] + ((uint64_t)X->a[cnt_i] * (uint64_t)Y->a[cnt_j]) + U;
            U = UV >> 32;
            V = UV & 0x00000000FFFFFFFF;
            C[cnt_i + cnt_j] = V;
        }
        C[cnt_i + P384] = U;
    }
    memcpy(test, C, sizeof(uint32_t) * (2 * P384)); // mul 결과값 C[24] 저장
    // for (cnt_i = 15; cnt_i >= 0; cnt_i--) {
    //     printf("%08X", test[cnt_i]);
    // }
    FastReduction_P384(Z, C, prime);
}

/*
void PS_Multiplication(Bignum* X, Bignum* Y, Bignum* Z, Bignum* prime, uint32_t* test) {
    uint32_t C[16] = { 0x00 };
    uint32_t U = 0x00;
    uint32_t V = 0x00;
    uint32_t R0 = 0x00;
    uint32_t R1 = 0x00;
    uint32_t R2 = 0x00;
    uint32_t tmp = 0x00;

    uint64_t UV = 0x00;
    int cnt_i, cnt_k;
    int carry = 0;
    
    for (cnt_k = 0; cnt_k < 15; cnt_k++) {
        if (cnt_k < 8) {
            for (cnt_i = 0; cnt_i < cnt_k + 1; cnt_i++) {
                UV = (uint64_t)X->a[cnt_i] * (uint64_t)Y->a[cnt_k - cnt_i];
                U = UV >> 32;
                V = UV & 0x00000000FFFFFFFF;

                tmp = V + R0;
                if (tmp < V) {
                    carry = 1;
                }
                R0 = tmp;

                tmp = U + R1;
                R1 = tmp + carry;
                if (tmp < U) {
                    carry = 1;
                }
                else if (R1 < carry) {
                    carry = 1;
                }
                else {
                    carry = 0;
                }

                R2 = R2 + carry;

                carry = 0;  // 다음을 위해 carry 초기화
            }
        }
        else{
            for (cnt_i = cnt_k - 7; cnt_i < 8; cnt_i++) {
                UV = (uint64_t)X->a[cnt_i] * (uint64_t)Y->a[cnt_k - cnt_i];
                U = UV >> 32;
                V = UV & 0x00000000FFFFFFFF;

                tmp = V + R0;
                if (tmp < V) {
                    carry = 1;
                }
                R0 = tmp;

                tmp = U + R1;
                R1 = tmp + carry;
                if (tmp < U) {
                    carry = 1;
                }
                else if (R1 < carry) {
                    carry = 1;
                }
                else {
                    carry = 0;
                }

                R2 = R2 + carry;

                carry = 0;  // 다음을 위해 carry 초기화
            }
        }
        C[cnt_k] = R0;
        R0 = R1;
        R1 = R2;
        R2 = 0x00;
    }
    C[15] = R0;

    memcpy(test, C, sizeof(uint32_t) * 16); // mul 결과값 C[16] 저장
    FastReduction_P256(Z, C, prime);
}
*/

// Z <- X^2 
void Fp_Square(Bignum* X, Bignum* Z, Bignum* prime, uint32_t test[]) {
    uint32_t C[24] = { 0x00 };
    uint32_t U = 0x00;
    uint32_t V = 0x00;
    uint32_t R0 = 0x00;
    uint32_t R1 = 0x00;
    uint32_t R2 = 0x00;
    uint32_t tmp = 0x00;

    uint64_t UV = 0x00;
    uint64_t tmp2 = 0x00;
    int cnt_i, cnt_k;
    int carry = 0;

    for (cnt_k = 0; cnt_k < P384 - 1; cnt_k++) {
        if (cnt_k < P384) {
            for (cnt_i = 0; cnt_i <= cnt_k - cnt_i; cnt_i++) {
                UV = (uint64_t)X->a[cnt_i] * (uint64_t)X->a[cnt_k - cnt_i];
                if (cnt_i < cnt_k - cnt_i) {
                    tmp2 = UV + UV;
                    if (tmp2 < UV) {
                        carry = 1;
                    }
                    UV = tmp2;   // carry 처리 후 UV <- 2*UV 처리 (tmp2 = UV +UV)
                    R2 = R2 + carry;
                    carry = 0;  // 다음 계산을 위해 carry 초기화
                }
                
                U = UV >> 32;
                V = UV & 0x00000000FFFFFFFF;

                tmp = V + R0;
                if (tmp < V) {
                    carry = 1;
                }
                R0 = tmp;

                tmp = U + R1;
                R1 = tmp + carry;
                if (tmp < U) {
                    carry = 1;
                }
                else if (R1 < carry) {
                    carry = 1;
                }
                else {
                    carry = 0;
                }

                R2 = R2 + carry;

                carry = 0;  // 다음을 위해 carry 초기화   
            }
        }
        else {
            for (cnt_i = cnt_k - (P384 - 1); cnt_i <= cnt_k - cnt_i; cnt_i++) {
                UV = (uint64_t)X->a[cnt_i] * (uint64_t)X->a[cnt_k - cnt_i];
                if (cnt_i < cnt_k - cnt_i) {
                    tmp2 = UV + UV;
                    if (tmp2 < UV) {
                        carry = 1;
                    }
                    UV = tmp2;   // carry 처리 후 UV <- 2*UV 처리 (tmp2 = UV +UV)
                    R2 = R2 + carry;
                    carry = 0;  // 다음 계산을 위해 carry 초기화
                }
                
                U = UV >> 32;
                V = UV & 0x00000000FFFFFFFF;

                tmp = V + R0;
                if (tmp < V) {
                    carry = 1;
                }
                R0 = tmp;

                tmp = U + R1;
                R1 = tmp + carry;
                if (tmp < U) {
                    carry = 1;
                }
                else if (R1 < carry) {
                    carry = 1;
                }
                else {
                    carry = 0;
                }

                R2 = R2 + carry;

                carry = 0;  // 다음을 위해 carry 초기화    
            }
        }
        C[cnt_k] = R0;
        R0 = R1;
        R1 = R2;
        R2 = 0x00;
    }
    C[2 * P384 - 1] = R0;

    memcpy(test, C, sizeof(uint32_t) * (2 * P384)); // mul 결과값 C[16] 저장
    FastReduction_P384(Z, C, prime);
}

/*
void FastReduction_P256(Bignum* X, uint32_t C[], Bignum* prime) {     // X에 C 대입 
    int cnt_i;

    Bignum S1 = { {0x00}, 0x00 };
    Bignum S2 = { {0x00}, 0x00 };
    Bignum S3 = { {0x00}, 0x00 };
    Bignum S4 = { {0x00}, 0x00 };
    Bignum S5 = { {0x00}, 0x00 };
    Bignum S6 = { {0x00}, 0x00 };
    Bignum S7 = { {0x00}, 0x00 };
    Bignum S8 = { {0x00}, 0x00 };
    Bignum S9 = { {0x00}, 0x00 };

    Bignum tmp1 = { {0x00}, 0x00 };
    //Bignum tmp2 = { {0x00}, 0x00 };
    
    uint32_t S1_arr[8] = { C[7], C[6], C[5], C[4], C[3], C[2], C[1], C[0] };
    uint32_t S2_arr[8] = { C[15], C[14], C[13], C[12], C[11], 0x00, 0x00, 0x00 };
    uint32_t S3_arr[8] = { 0x00, C[15], C[14], C[13], C[12], 0x00, 0x00, 0x00 };
    uint32_t S4_arr[8] = { C[15], C[14], 0x00, 0x00, 0x00, C[10], C[9], C[8] };
    uint32_t S5_arr[8] = { C[8], C[13], C[15], C[14], C[13], C[11], C[10], C[9] };
    uint32_t S6_arr[8] = { C[10], C[8], 0x00, 0x00, 0x00, C[13], C[12], C[11] };
    uint32_t S7_arr[8] = { C[11], C[9], 0x00, 0x00, C[15],  C[14], C[13], C[12] };
    uint32_t S8_arr[8] = { C[12], 0x00, C[10], C[9], C[8], C[15], C[14], C[13] };
    uint32_t S9_arr[8] = { C[13], 0x00, C[11], C[10], C[9], 0x00, C[15], C[14] };

    SetBignum(&S1, S1_arr);
    SetBignum(&S2, S2_arr);
    SetBignum(&S3, S3_arr);
    SetBignum(&S4, S4_arr);
    SetBignum(&S5, S5_arr);
    SetBignum(&S6, S6_arr);
    SetBignum(&S7, S7_arr);
    SetBignum(&S8, S8_arr);
    SetBignum(&S9, S9_arr);

    // tmp1 한개로 하는건 왜 안될까 ???  --> add,sub에서 중간 tmp 안써서
    Fp_Addition(&S1, &S2, &tmp1, prime);
    Fp_Subtraction(&tmp1, &S6, &tmp1, prime);
    Fp_Addition(&tmp1, &S2, &tmp1, prime);
    Fp_Subtraction(&tmp1, &S7, &tmp1, prime);
    Fp_Addition(&tmp1, &S3, &tmp1, prime);
    Fp_Subtraction(&tmp1, &S8, &tmp1, prime);
    Fp_Addition(&tmp1, &S3, &tmp1, prime);  
    Fp_Subtraction(&tmp1, &S9, &tmp1, prime);
    Fp_Addition(&tmp1, &S4, &tmp1, prime);
    Fp_Addition(&tmp1, &S5, &tmp1, prime);
    
    // Fp_Addition(&S1, &S2, &tmp1, prime);
    // Fp_Subtraction(&tmp1, &S6, &tmp2, prime);
    // Fp_Addition(&tmp2, &S2, &tmp1, prime);
    // Fp_Subtraction(&tmp1, &S7, &tmp2, prime);
    // Fp_Addition(&tmp2, &S3, &tmp1, prime);
    // Fp_Subtraction(&tmp1, &S8, &tmp2, prime);
    // Fp_Addition(&tmp2, &S3, &tmp1, prime);  
    // Fp_Subtraction(&tmp1, &S9, &tmp2, prime);
    // Fp_Addition(&tmp2, &S4, &tmp1, prime);
    // Fp_Addition(&tmp1, &S5, &tmp2, prime);
    // //PrintBignum(&tmp, "tmp");

    memcpy(X, &tmp1, sizeof(Bignum));
    //PrintBignum(X, "X");
}
*/

void FastReduction_P384(Bignum *X, uint32_t C[], Bignum *prime) {     // X에 C 대입 
    int cnt_i;

    Bignum S1 = { {0x00}, 0x00 };
    Bignum S2 = { {0x00}, 0x00 };
    Bignum S3 = { {0x00}, 0x00 };
    Bignum S4 = { {0x00}, 0x00 };
    Bignum S5 = { {0x00}, 0x00 };
    Bignum S6 = { {0x00}, 0x00 };
    Bignum S7 = { {0x00}, 0x00 };
    Bignum S8 = { {0x00}, 0x00 };
    Bignum S9 = { {0x00}, 0x00 };
    Bignum S10 = { {0x00}, 0x00 };
    
    Bignum tmp1 = { {0x00}, 0x00 };
    //Bignum tmp2 = { {0x00}, 0x00 };
    
    uint32_t S1_arr[P384] = { C[11], C[10], C[9], C[8], C[7], C[6], C[5], C[4], C[3], C[2], C[1], C[0] };
    uint32_t S2_arr[P384] = { 0x00, 0x00, 0x00, 0x00, 0x00, C[23], C[22], C[21], 0x00, 0x00, 0x00, 0x00 };
    uint32_t S3_arr[P384] = { C[23], C[22], C[21], C[20], C[19], C[18], C[17], C[16], C[15], C[14], C[13], C[12] };
    uint32_t S4_arr[P384] = { C[20], C[19], C[18], C[17], C[16], C[15], C[14], C[13], C[12], C[23], C[22], C[21] };
    uint32_t S5_arr[P384] = { C[19], C[18], C[17], C[16], C[15], C[14], C[13], C[12], C[20], 0x00, C[23], 0x00 };
    uint32_t S6_arr[P384] = { 0x00, 0x00, 0x00, 0x00, C[23], C[22], C[21], C[20], 0x00, 0x00, 0x00, 0x00 };
    uint32_t S7_arr[P384] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, C[23], C[22], C[21], 0x00, 0x00, C[20] };
    uint32_t S8_arr[P384] = { C[22], C[21], C[20], C[19], C[18], C[17], C[16], C[15], C[14], C[13], C[12], C[23] };
    uint32_t S9_arr[P384] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, C[23], C[22], C[21], C[20], 0x00 };
    uint32_t S10_arr[P384] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, C[23], C[22], 0x00, 0x00, 0x00 };
    
    SetBignum(&S1, S1_arr);
    SetBignum(&S2, S2_arr);
    SetBignum(&S3, S3_arr);
    SetBignum(&S4, S4_arr);
    SetBignum(&S5, S5_arr);
    SetBignum(&S6, S6_arr);
    SetBignum(&S7, S7_arr);
    SetBignum(&S8, S8_arr);
    SetBignum(&S9, S9_arr);
    SetBignum(&S10, S10_arr);

    Fp_Addition(&S1, &S2, &tmp1, prime);
    Fp_Subtraction(&tmp1, &S8, &tmp1, prime);
    Fp_Addition(&tmp1, &S2, &tmp1, prime);
    Fp_Subtraction(&tmp1, &S9, &tmp1, prime);
    Fp_Addition(&tmp1, &S3, &tmp1, prime);
    Fp_Subtraction(&tmp1, &S10, &tmp1, prime);
    Fp_Addition(&tmp1, &S4, &tmp1, prime);  
    Fp_Addition(&tmp1, &S5, &tmp1, prime);
    Fp_Addition(&tmp1, &S6, &tmp1, prime);
    Fp_Addition(&tmp1, &S7, &tmp1, prime);

    memcpy(X, &tmp1, sizeof(Bignum));
    //PrintBignum(X, "X");
}

// Z의 역원 = T
void Fp_InverseFLT(Bignum* Z, Bignum* T, Bignum* prime) {
    Bignum Z3 = { {0x00}, 0x00 };
    Bignum Z15 = { {0x00}, 0x00 };
    Bignum T0 = { {0x00}, 0x00 };
    Bignum T1 = { {0x00}, 0x00 };
    Bignum T2 = { {0x00}, 0x00 };
    Bignum T3 = { {0x00}, 0x00 };
    Bignum tmp = { {0x00}, 0x00 };
    Bignum tmp2 = { {0x00}, 0x00 };

    uint32_t buf[16] = { 0x00 };

    int cnt_i;
    // Z3 계산
    Fp_Square(Z, &tmp, prime, buf);
    OS_Multiplication(&tmp, Z, &Z3, prime, buf);
    //PrintBignum(&Z3, "Z3");

    // Z15 계산
    Fp_Square(&Z3, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf); // -> Fp_Square(&tmp, &tmp, ...) 해도 될까???
    OS_Multiplication(&tmp2, &Z3, &Z15, prime, buf);
    //PrintBignum(&Z15, "Z15");

    // T0 계산
    Fp_Square(&Z15, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    OS_Multiplication(&tmp2, &Z3, &T0, prime, buf);
    //PrintBignum(&T0, "T0");

    // T1 계산
    Fp_Square(&T0, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    OS_Multiplication(&tmp2, &T0, &T1, prime, buf);
    //PrintBignum(&T1, "T1");

    // T2 계산
    Fp_Square(&T1, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf); // tmp = T1^2^12
    OS_Multiplication(&tmp, &T1, &tmp2, prime, buf);

    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    OS_Multiplication(&tmp2, &T0, &T2, prime, buf);
    //PrintBignum(&T2, "T2");

    // T3 계산
    Fp_Square(&T2, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    OS_Multiplication(&tmp, &Z3, &T3, prime, buf);
    //PrintBignum(&T3, "T3");

    // T4 계산
    Fp_Square(&T3, &tmp2, prime, buf);
    for (cnt_i = 0; cnt_i < 15; cnt_i++) {
        Fp_Square(&tmp2, &tmp, prime, buf);
        Fp_Square(&tmp, &tmp2, prime, buf);
    }
    Fp_Square(&tmp2, &tmp, prime, buf);
    OS_Multiplication(&tmp, Z, &tmp2, prime, buf);

    for (cnt_i = 0; cnt_i < 48; cnt_i++) {
        Fp_Square(&tmp2, &tmp, prime, buf);
        Fp_Square(&tmp, &tmp2, prime, buf);
    }
    // tmp2 = T4    
    //PrintBignum(&tmp2, "T4");

    // T5 계산
    for (cnt_i = 0; cnt_i < 16; cnt_i++) {
        Fp_Square(&tmp2, &tmp, prime, buf);
        Fp_Square(&tmp, &tmp2, prime, buf);
    }
    OS_Multiplication(&tmp2, &T3, &tmp, prime, buf);
    for (cnt_i = 0; cnt_i < 16; cnt_i++) {
        Fp_Square(&tmp, &tmp2, prime, buf);
        Fp_Square(&tmp2, &tmp, prime, buf);
    }
    OS_Multiplication(&tmp, &T3, &tmp2, prime, buf);
    // tmp2 = T5
    //PrintBignum(&tmp2, "T5");

    // T 계산
    for (cnt_i = 0; cnt_i < 15; cnt_i++) {
        Fp_Square(&tmp2, &tmp, prime, buf);
        Fp_Square(&tmp, &tmp2, prime, buf);
    }
    OS_Multiplication(&tmp2, &T2, &tmp, prime, buf);
    Fp_Square(&tmp, &tmp2, prime, buf);
    Fp_Square(&tmp2, &tmp, prime, buf);
    OS_Multiplication(&tmp, Z, T, prime, buf);
    //PrintBignum(T, "T");
    
    //memcpy(T, &tmp, sizeof(Bignum));
}

void SetEccnum(ECCnum* A, uint32_t inputdata_X[], uint32_t inputdata_Y[]) {
    int cnt_i;
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        A->X.a[cnt_i] = inputdata_X[(P384 - 1) - cnt_i];
        A->Y.a[cnt_i] = inputdata_Y[(P384 - 1) - cnt_i];
    }
    A->infinitypoint = 0;
}

void PrintECCnum(ECCnum* A, const char* name) { // 확인용 출력함수
    int cnt_i;
    printf("ECCnumber %s \n", name);
    
    printf("X = ");
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        printf("%08X ", A->X.a[(P384 - 1) - cnt_i]); // 출력할땐 원래 순서대로 출력 (저장할때 반대로 저장함)
    }
    printf("\n");
    
    printf("Y = ");
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        printf("%08X ", A->Y.a[(P384 - 1) - cnt_i]); // 출력할땐 원래 순서대로 출력 (저장할때 반대로 저장함)
    }
    printf("\n");
    printf("\n");
}

// R = P + Q 
void ECADD(ECCnum* P, ECCnum* Q, ECCnum* R, Bignum* prime) {
    Bignum tmp1 = { {0x00}, 0x00 };
    Bignum tmp2 = { {0x00}, 0x00 };
    Bignum tmp3 = { {0x00}, 0x00 };

    Bignum X_tmp = { 0x00 };
    Bignum Y_tmp = { 0x00 };

    uint32_t buf[2 * P384] = { 0x00 };

    Fp_Subtraction(&(Q->Y), &(P->Y), &tmp1, prime);
    Fp_Subtraction(&(Q->X), &(P->X), &tmp2, prime);
    Fp_InverseFLT(&tmp2, &tmp3, prime);
    OS_Multiplication(&tmp1, &tmp3, &tmp2, prime, buf); // tmp2 = y2-y1/x2-x1

    Fp_Square(&tmp2, &tmp1, prime, buf);
    Fp_Subtraction(&tmp1, &(P->X), &tmp3, prime);
    Fp_Subtraction(&tmp3, &(Q->X), &tmp1, prime);
    memcpy(&X_tmp, &tmp1, sizeof(Bignum));

    Fp_Subtraction(&(P->X), &(X_tmp), &tmp3, prime);
    OS_Multiplication(&tmp2, &tmp3, &tmp1, prime, buf);
    Fp_Subtraction(&tmp1, &(P->Y), &tmp3, prime);
    memcpy(&Y_tmp, &tmp3, sizeof(Bignum));

    memcpy(&(R->X), &X_tmp, sizeof(Bignum));
    memcpy(&(R->Y), &Y_tmp, sizeof(Bignum));
}

// R = 2P
void ECDBL(ECCnum* P, ECCnum* R, Bignum* a, Bignum* prime) {
    Bignum tmp1 = { {0x00}, 0x00 };
    Bignum tmp2 = { {0x00}, 0x00 };
    Bignum tmp3 = { {0x00}, 0x00 };
    Bignum tmp4 = { {0x00}, 0x00 };

    Bignum X_tmp = { 0x00 };
    Bignum Y_tmp = { 0x00 };

    uint32_t buf[2 * P384] = { 0x00 };

    Fp_Addition(&(P->X), &(P->X), &tmp1, prime);
    memcpy(&tmp4, &tmp1, sizeof(Bignum));       // tmp4 = 2 * X1
    Fp_Addition(&tmp1, &(P->X), &tmp2, prime);
    OS_Multiplication(&tmp2, &(P->X), &tmp1, prime, buf);
    Fp_Addition(&tmp1, a, &tmp2, prime);
    Fp_Addition(&(P->Y), &(P->Y), &tmp1, prime);
    Fp_InverseFLT(&tmp1, &tmp3, prime);
    OS_Multiplication(&tmp2, &tmp3, &tmp1, prime, buf); // tmp1 = 람다

    Fp_Square(&tmp1, &tmp2, prime, buf);
    //Fp_Addition(&(P->X), &(P->X), &tmp3, prime);
    Fp_Subtraction(&tmp2, &tmp4, &X_tmp, prime);

    Fp_Subtraction(&(P->X), &X_tmp, &tmp2, prime);
    OS_Multiplication(&tmp1, &tmp2, &tmp3, prime, buf);
    Fp_Subtraction(&tmp3, &(P->Y), &Y_tmp, prime);
    
    memcpy(&(R->X), &X_tmp, sizeof(Bignum));
    memcpy(&(R->Y), &Y_tmp, sizeof(Bignum));
}

// L to R scalar multiplication
// R = k * P (k is scalar)
void ECLTR(ECCnum* P, Bignum* k, ECCnum* R, Bignum* a, Bignum* prime) {
    int check_bit = 0;
    int cnt_i, cnt_j;

    ECCnum tmp1 = { {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 };
    ECCnum tmp2 = { {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 };
    
    for (cnt_i = P384 - 1; cnt_i >= 0; cnt_i--) {
        for (cnt_j = 31; cnt_j >= 0; cnt_j--) {
            if (check_bit == 1) {
                ECDBL(&tmp1, &tmp2, a, prime);  // Q <- 2Q
                if (((k->a[cnt_i] >> cnt_j) & 0x01) != 0) {  // k[i] == 1
                    ECADD(&tmp2, P, &tmp1, prime);
                    continue;
                }
                memcpy(&tmp1, &tmp2, sizeof(ECCnum));
            }
            else {  // check_bit가 1이 되면 else문 까지 더이상 오지 않음
                check_bit = ((k->a[cnt_i] >> cnt_j) & 0x01); 
                if (check_bit != 0) {
                    memcpy(&tmp1, P, sizeof(ECCnum));
                }
            }
        }
    }
    memcpy(R, &tmp1, sizeof(ECCnum));
}

// R to L scalar multiplication
// R = k * P (k is scalar)
void ECRTL(ECCnum* P, Bignum* k, ECCnum* R, Bignum* a, Bignum* prime) {
    int cnt_i, cnt_j;
    int check_bit = 0;

    ECCnum tmp1 = { {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 }; // ppt에서의 A 역할
    ECCnum tmp2 = { {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 }; // buffer 역할
    ECCnum tmp3 = { {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 }; // Q 역할

    memcpy(&tmp1, P, sizeof(ECCnum));   // A <- P, Q <- infinity

    // 무한원점과 ECADD를 하는 것은 금기? -> 처음에 이렇게해서 틀린듯
    // 무한원점은 타원 위에 있지 않은 점을 하나로 모아놓은 것이라 생각하기 (0,0)도 무한원
    // ECADD(&tmp3, &tmp1, &tmp2, prime);  // Q <- Q + A 이렇게 하면 tmp3가 무한점인 상태이기 때문에 조건문을 추가해서 멤카피 하는게 옳다.

    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        for (cnt_j = 0; cnt_j < 32; cnt_j++) {
            if (((k->a[cnt_i] >> cnt_j) & 0x01) != 0) {
                if (check_bit == 0) {
                    memcpy(&tmp3, &tmp1, sizeof(ECCnum));   // Q <- Q(infinity) + A 
                    check_bit = 1;
                }
                else {
                    ECADD(&tmp3, &tmp1, &tmp2, prime);  // Q <- Q + A , 연산값 tmp2에 넣었다가 tmp3에 넣기
                    memcpy(&tmp3, &tmp2, sizeof(ECCnum));
                }
            }
            ECDBL(&tmp1, &tmp2, a, prime);  // 2A를 tmp2에 저장하고 다시 tmp1에 넣어주기
            memcpy(&tmp1, &tmp2, sizeof(ECCnum));   // A <- 2A
        }
    }
    memcpy(R, &tmp3, sizeof(ECCnum));
}
// P * 0(infinity)에 대한 연산 결과가 0이 나올거 같은데 오류가 나면 여기일듯? 아니면 ECLTR일수도

void SetEccnum_J(ECCnum_J* A, uint32_t inputdata_X[], uint32_t inputdata_Y[], uint32_t inputdata_Z[]) {
    int cnt_i;
    for (cnt_i = 0; cnt_i < 8; cnt_i++) {
        A->X.a[cnt_i] = inputdata_X[7 - cnt_i];
        A->Y.a[cnt_i] = inputdata_Y[7 - cnt_i];
        A->Z.a[cnt_i] = inputdata_Z[7 - cnt_i];
    }
    A->infinitypoint = 0;
}

void PrintECCnum_J(ECCnum_J* A, const char* name) { // 확인용 출력함수
    int cnt_i;
    printf("ECCnumber %s \n", name);
    
    printf("X = ");
    for (cnt_i = 0; cnt_i < 8; cnt_i++) {
        printf("%08X ", A->X.a[7 - cnt_i]); // 출력할땐 원래 순서대로 출력 (저장할때 반대로 저장함)
    }
    printf("\n");
    
    printf("Y = ");
    for (cnt_i = 0; cnt_i < 8; cnt_i++) {
        printf("%08X ", A->Y.a[7 - cnt_i]); // 출력할땐 원래 순서대로 출력 (저장할때 반대로 저장함)
    }
    printf("\n");

    printf("Z = ");
    for (cnt_i = 0; cnt_i < 8; cnt_i++) {
        printf("%08X ", A->Z.a[7 - cnt_i]); // 출력할땐 원래 순서대로 출력 (저장할때 반대로 저장함)
    }
    printf("\n");
    printf("\n");
}

// A(X, Y) -> B(X, Y, 1)
void From_A_to_J(ECCnum* A, ECCnum_J* B) {
    Bignum tmp1 = { {0x00}, 0x00 };
    uint32_t arr1[8] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000,
                    0x00000000, 0x00000000, 0x00000000, 0x00000001 };
    SetBignum(&tmp1, arr1);
    memcpy(&(B->Z), &tmp1, sizeof(Bignum));
    memcpy(&(B->X), &(A->X), sizeof(B->X));
    memcpy(&(B->Y), &(A->Y), sizeof(B->Y));
}

// B(X, Y, Z) -> B2(X/Z^2, Y/Z^3, 1) -> A(X/Z^2, Y/Z^3)
void From_J_to_A(ECCnum_J* A, ECCnum* B, Bignum* prime) {
    Bignum tmp1 = { {0x00}, 0x00 };
    Bignum tmp2 = { {0x00}, 0x00 };
    Bignum tmp3 = { {0x00}, 0x00 };
    
    uint32_t buf[16] = { 0x00 };
    
    Fp_Square(&(A->Z), &tmp1, prime, buf);
    OS_Multiplication(&tmp1, &(A->Z), &tmp2, prime, buf); // tmp2 = Z^3
    Fp_InverseFLT(&tmp2, &tmp1, prime); // tmp1 = 1/Z^3
    OS_Multiplication(&(A->Y), &tmp1, &tmp2, prime, buf);   // Y * (1/Z^3) = affine Y 축
    memcpy(&(B->Y), &tmp2, sizeof(Bignum));

    OS_Multiplication(&(A->Z), &tmp1, &tmp2, prime, buf);   // tmp2 = 1/Z^2
    OS_Multiplication(&(A->X), &tmp2, &tmp3, prime, buf);   // X * (1/Z^2) = affine X 축
    memcpy(&(B->X), &tmp3, sizeof(Bignum));
}

void ECDBL_J(ECCnum_J* P, ECCnum_J* R, Bignum* a, Bignum* prime) {
    Bignum T1 = { {0x00}, 0x00 };
    Bignum T2 = { {0x00}, 0x00 };
    Bignum T3 = { {0x00}, 0x00 };
    Bignum X3 = { {0x00}, 0x00 };
    Bignum Y3 = { {0x00}, 0x00 };
    Bignum Z3 = { {0x00}, 0x00 };
    Bignum tmp1 = { {0x00}, 0x00 };
    Bignum tmp2 = { {0x00}, 0x00 };
    Bignum tmp3 = { {0x00}, 0x00 };

    uint32_t buf[16] = { 0x00 };

    int cnt_i, cnt_j;
    int tmp = 0x00;

    Fp_Square(&(P->Z), &T1, prime, buf); // 2
    Fp_Subtraction(&(P->X), &T1, &T2, prime); // 3
    Fp_Addition(&(P->X), &T1, &tmp1, prime);
    memcpy(&T1, &tmp1, sizeof(Bignum)); // 4
    OS_Multiplication(&T2, &T1, &tmp1, prime, buf);
    memcpy(&T2, &tmp1, sizeof(Bignum)); // 5
    Fp_Addition(&tmp1, &T2, &tmp2, prime);
    Fp_Addition(&tmp2, &tmp1, &T2, prime); // 6
    Fp_Addition(&(P->Y), &(P->Y), &Y3, prime); // 7
    OS_Multiplication(&Y3, &(P->Z), &Z3, prime, buf); // 8
    Fp_Square(&Y3, &tmp3, prime, buf);
    memcpy(&Y3, &tmp3, sizeof(Bignum)); // 9
    OS_Multiplication(&Y3, &(P->X), &T3, prime, buf); // 10
    Fp_Square(&Y3, &tmp3, prime, buf);
    memcpy(&Y3, &tmp3, sizeof(Bignum)); // 11
    //PrintBignum(&Y3, "11 Y3");
    if ((Y3.a[0] & 0x01) == 0) {
        for (cnt_i = 0; cnt_i < 8; cnt_i++) {
            Y3.a[cnt_i] = Y3.a[cnt_i] >> 1;
            if (cnt_i < 7) {
                tmp = Y3.a[cnt_i + 1] & 0x01;
                Y3.a[cnt_i] = Y3.a[cnt_i] | (tmp << 31);
            }
        }
    }
    else {
        Bn_Addition(&Y3, prime, &tmp1);
        for (cnt_i = 0; cnt_i < 8; cnt_i++) {
            tmp1.a[cnt_i] = tmp1.a[cnt_i] >> 1;
            if (cnt_i < 7) {
                tmp = tmp1.a[cnt_i + 1] & 0x01;
                tmp1.a[cnt_i] = tmp1.a[cnt_i] | (tmp << 31);
            }
        }
        if (tmp1.e == 1) {
                tmp1.a[7] = tmp1.a[7] | 0x80000000; // carry 처리 ( | 일까 ^ 일까????)
                tmp1.e = 0;
        }
        memcpy(&Y3, &tmp1, sizeof(Bignum));
    }   // 12
    //PrintBignum(&Y3, "12 Y3");
    Fp_Square(&T2, &X3, prime, buf); // 13
    //PrintBignum(&X3, "13 X3");
    memcpy(&tmp1, &T3, sizeof(Bignum)); // tmp1 <- T3
    Fp_Addition(&tmp1, &T3, &T1, prime); // 14
    Fp_Subtraction(&X3, &T1, &tmp1, prime);
    memcpy(&X3, &tmp1, sizeof(Bignum)); // 15
    Fp_Subtraction(&T3, &X3, &T1, prime);  // 16
    OS_Multiplication(&T1, &T2, &tmp1, prime, buf);
    memcpy(&T1, &tmp1, sizeof(Bignum)); // 17
    Fp_Subtraction(&T1, &Y3, &tmp1, prime);
    memcpy(&Y3, &tmp1, sizeof(Bignum)); // 18

    memcpy(&(R->X), &X3, sizeof(Bignum));
    memcpy(&(R->Y), &Y3, sizeof(Bignum));
    memcpy(&(R->Z), &Z3, sizeof(Bignum));
}

void ECADD_J(ECCnum_J* P, ECCnum* Q, ECCnum_J* R, Bignum* a, Bignum* prime) {
    Bignum T1 = { {0x00}, 0x00 };
    Bignum T2 = { {0x00}, 0x00 };
    Bignum T3 = { {0x00}, 0x00 };
    Bignum T4 = { {0x00}, 0x00 };
    Bignum X3 = { {0x00}, 0x00 };
    Bignum Y3 = { {0x00}, 0x00 };
    Bignum Z3 = { {0x00}, 0x00 };
    Bignum tmp1 = { {0x00}, 0x00 };
    Bignum tmp2 = { {0x00}, 0x00 };

    ECCnum_J tmp3 = { {{0x00}, 0x00}, {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 };
    ECCnum_J tmp4 = { {{0x00}, 0x00}, {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 };

    uint32_t buf[16] = { 0x00 };

    int cnt_i, cnt_j;
    int tmp = 0x00;
    if (Q->infinitypoint == 1) {    // Q is infinity point
        memcpy(R, P, sizeof(ECCnum_J));
        return;  
    }
    if (P->infinitypoint == 1) {    // P is infinity point
        From_A_to_J(Q, R);
        return;
    }

    Fp_Square(&(P->Z), &T1, prime, buf); // 3
    OS_Multiplication(&T1, &(P->Z), &T2, prime, buf); // 4
    OS_Multiplication(&T1, &(Q->X), &tmp1, prime, buf);
    memcpy(&T1, &tmp1, sizeof(Bignum)); // 5
    OS_Multiplication(&T2, &(Q->Y), &tmp2, prime, buf);
    memcpy(&T2, &tmp2, sizeof(Bignum)); // 6
    Fp_Subtraction(&T1, &(P->X), &tmp1, prime);
    memcpy(&T1, &tmp1, sizeof(Bignum)); // 7
    Fp_Subtraction(&T2, &(P->Y), &tmp2, prime);
    memcpy(&T2, &tmp2, sizeof(Bignum)); // 8
    if (IsZero(&T1) == 1) { // 9
        if (IsZero(&T2) == 1) { // 9.1
            From_A_to_J(Q, &tmp3);
            ECDBL_J(&tmp3, &tmp4, a, prime);
            memcpy(R, &tmp4, sizeof(ECCnum_J));
            return;
        }
        else { // 9.2
            printf("Return infinity\n");
            R->infinitypoint = 1;
            return;
        }
    }

    OS_Multiplication(&(P->Z), &T1, &Z3, prime, buf); // 10
    Fp_Square(&T1, &T3, prime, buf); // 11
    OS_Multiplication(&T3, &T1, &T4, prime, buf); // 12
    OS_Multiplication(&T3, &(P->X), &tmp1, prime, buf);     //  3.6  수정
    memcpy(&T3, &tmp1, sizeof(Bignum)); // 13
    Fp_Addition(&T3, &T3, &T1, prime); // 14                                문제의심
    Fp_Square(&T2, &X3, prime, buf); // 15
    Fp_Subtraction(&X3, &T1, &tmp1, prime);
    memcpy(&X3, &tmp1, sizeof(Bignum)); // 16
    Fp_Subtraction(&X3, &T4, &tmp1, prime);
    memcpy(&X3, &tmp1, sizeof(Bignum)); // 17
    Fp_Subtraction(&T3, &X3, &tmp1, prime);
    memcpy(&T3, &tmp1, sizeof(Bignum)); // 18
    OS_Multiplication(&T3, &T2, &tmp1, prime, buf);
    memcpy(&T3, &tmp1, sizeof(Bignum)); // 19
    OS_Multiplication(&T4, &(P->Y), &tmp1, prime, buf);
    memcpy(&T4, &tmp1, sizeof(Bignum)); // 20
    Fp_Subtraction(&T3, &T4, &Y3, prime); // 21
    
    memcpy(&(R->X), &X3, sizeof(Bignum));
    memcpy(&(R->Y), &Y3, sizeof(Bignum));
    memcpy(&(R->Z), &Z3, sizeof(Bignum));
}

void ECLTR_J(ECCnum_J* P, Bignum* k, ECCnum_J* R, Bignum* a, Bignum* prime) {
    int check_bit = 0;
    int cnt_i, cnt_j;

    ECCnum_J tmp1 = { {{0x00}, 0x00}, {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 };
    ECCnum_J tmp2 = { {{0x00}, 0x00}, {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 };
    ECCnum tmp3 = { {{0x00}, 0x00}, {{0x00}, 0x00}, 0x00 };

    From_J_to_A(P, &tmp3, prime);       // tmp3 = P 를 아핀변환한 값

    for (cnt_i = 7; cnt_i >= 0; cnt_i--) {
        for (cnt_j = 31; cnt_j >= 0; cnt_j--) {
            if (check_bit == 1) {
                ECDBL_J(&tmp1, &tmp2, a, prime);  // Q <- 2Q
                if (((k->a[cnt_i] >> cnt_j) & 0x01) != 0) {  // k[i] == 1
                    //From_J_to_A(P, &tmp3, prime);       // tmp3 = P 를 아핀변환한 값
                    ECADD_J(&tmp2, &tmp3, &tmp1, a, prime);
                    continue;
                }
                memcpy(&tmp1, &tmp2, sizeof(ECCnum_J));
            }
            else {  // check_bit가 1이 되면 else문 까지 더이상 오지 않음
                check_bit = ((k->a[cnt_i] >> cnt_j) & 0x01); 
                if (check_bit != 0) {
                    memcpy(&tmp1, P, sizeof(ECCnum_J));
                }
            }
        }
    }
    memcpy(R, &tmp1, sizeof(ECCnum_J));
}

void P384_Multiplication(Bignum *X, Bignum *Y, Bignum *Z, Bignum *prime, uint32_t *test) {
    uint32_t C[24] = { 0x00 };
    uint32_t U = 0x00;
    uint32_t V = 0x00;
    uint64_t UV = 0x00;
    int cnt_i, cnt_j;
    for (cnt_i = 0; cnt_i < P384; cnt_i++) {
        U = 0x00;
        for (cnt_j = 0; cnt_j < P384; cnt_j++) {
            UV = C[cnt_i + cnt_j] + ((uint64_t)X->a[cnt_i] * (uint64_t)Y->a[cnt_j]) + U;
            U = UV >> 32;
            V = UV & 0x00000000FFFFFFFF;
            C[cnt_i + cnt_j] = V;
        }
        C[cnt_i + P384] = U;
    }
    memcpy(test, C, sizeof(uint32_t) * (2 * P384)); // mul 결과값 C[16] 저장
    for (cnt_i = 23; cnt_i >= 0; cnt_i--) {
        printf("%08X", test[cnt_i]);
    }
    // FastReduction_P384(Z, C, prime);
}