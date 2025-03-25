#include <bits/stdc++.h>
#include <immintrin.h>
#include <emmintrin.h>    // SSE2
#include <tmmintrin.h>    // SSSE3（需要支持 _mm_shuffle_epi8）
using namespace std;

uint8_t state[32], subBytes1[32], subBytes2[32], matrix[32], final[32];  // 用 uint8_t 低 4 位存
__m128i n;
__m128i Input[9], Output[9];
uint8_t *ad, *m, *c;
size_t ad_len, m_len;
size_t c_len;
__m128i Z[3], K[2], M[3], C[3], AD[3];
__m128i temp[9];

int perm[32] = {
    0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23, 8, 24, 9, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31
};
int MDS[16][16] = {
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1},
    {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1},
    {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0},
    {0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
    {1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}
};
int mds[16][16] = {
    {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0},
    {1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1},
    {1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1},
    {0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
    {0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0},
    {1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1},
    {1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1},
    {0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0}
};
int sbox[16] = {
    0x1, 0x0, 0x2, 0x4, 0x3, 0x8, 0x6, 0xd, 0x9, 0xa, 0xb, 0xe, 0xf, 0xc, 0x7, 0x5
};

__m128i reverse_bytes(__m128i x) {
    // 构造一个逆序的 shuffle mask，值为 {15, 14, …, 0}
    const __m128i mask = _mm_setr_epi8(
            15, 14, 13, 12, 11, 10, 9, 8,
            7, 6, 5, 4, 3, 2, 1, 0
    );
    return _mm_shuffle_epi8(x, mask);
}

__m128i reverse_bits_in_bytes(__m128i x) {
    // 查找表，用于 4 位翻转，例如：0x0 -> 0x0, 0x1 -> 0x8, 0x2 -> 0x4, ..., 0xF -> 0xF
    const __m128i lut = _mm_setr_epi8(
            0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
            0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF
    );

    // 拆分低 4 位和高 4 位
    __m128i low_nibble = _mm_and_si128(x, _mm_set1_epi8(0x0F));
    __m128i high_nibble = _mm_and_si128(_mm_srli_epi16(x, 4), _mm_set1_epi8(0x0F));

    // 分别查表翻转
    low_nibble = _mm_shuffle_epi8(lut, low_nibble);
    high_nibble = _mm_shuffle_epi8(lut, high_nibble);

    // 合并：将低位查表结果左移 4 位，再 OR 上高位查表结果
    return _mm_or_si128(_mm_slli_epi16(low_nibble, 4), high_nibble);
}

__m128i reverse_128bits(__m128i x) {
    // 第一步：字节翻转
    __m128i rev_bytes = reverse_bytes(x);
    // 第二步：每个字节内部位翻转
    return reverse_bits_in_bytes(rev_bytes);
}


void print_m128i(__m128i vec) {
    unsigned char bytes[16];
    _mm_storeu_si128((__m128i*)bytes, vec);
    cout << "0x";
    for (int i = 15; i >= 0; i --) {
        printf("%02X", bytes[i]);
        if (i % 4 == 0 && i) cout << ", 0x";
    }
    printf("\n");
}
void m128_to_nibbles(uint8_t *dst, __m128i vec) {
    // 提取每个字节的低 4 位
    __m128i low = _mm_and_si128(vec, _mm_set1_epi8(0x0F));
    // 右移 4 位，提取每个字节的高 4 位（移到低位后，再屏蔽高位，确保只保留 4 位）
    __m128i high = _mm_srli_epi16(vec, 4);
    high = _mm_and_si128(high, _mm_set1_epi8(0x0F));

    // 使用 _mm_unpacklo_epi8 和 _mm_unpackhi_epi8 交叉合并，
    // 将每个字节的低 nibble 和高 nibble分别放到单独的字节中，共得到 32 字节
    __m128i low_high0 = _mm_unpacklo_epi8(low, high);
    __m128i low_high1 = _mm_unpackhi_epi8(low, high);

    // 将结果存储到 dst 中（注意使用非对齐存储）
    _mm_storeu_si128((__m128i*)dst, low_high0);
    _mm_storeu_si128((__m128i*)(dst + 16), low_high1);
}

// 将 32 个 nibble（存于 uint8_t 数组中，每个 nibble 存在低 4 位）重构为 __m128i
// 输入 p 指向 32 字节的 nibble 数组，返回值为 16 字节的 __m128i 向量
__m128i nibbles_to_m128(const uint8_t *p) {
    // 分别加载前 16 字节和后 16 字节（共 32 字节）
    __m128i in0 = _mm_loadu_si128((const __m128i*)p);
    __m128i in1 = _mm_loadu_si128((const __m128i*)(p + 16));

    // 确保每个字节仅保留低 4 位
    __m128i mask = _mm_set1_epi8(0x0F);
    in0 = _mm_and_si128(in0, mask);
    in1 = _mm_and_si128(in1, mask);

    // 构造 shuffle mask 用于提取偶数（低 nibble）和奇数位置（高 nibble）
    const __m128i shuf_even = _mm_setr_epi8(0, 2, 4, 6, 8, 10, 12, 14,
                                              -1, -1, -1, -1, -1, -1, -1, -1);
    const __m128i shuf_odd  = _mm_setr_epi8(1, 3, 5, 7, 9, 11, 13, 15,
                                              -1, -1, -1, -1, -1, -1, -1, -1);

    // 对 in0 进行处理：偶数位置作为低 nibble，奇数位置作为高 nibble
    __m128i even0 = _mm_shuffle_epi8(in0, shuf_even);  // 低 nibble
    __m128i odd0  = _mm_shuffle_epi8(in0, shuf_odd);    // 高 nibble
    odd0 = _mm_slli_epi16(odd0, 4);                     // 将高 nibble左移 4 位
    __m128i packed0 = _mm_or_si128(odd0, even0);         // 合并为 8 字节

    // 对 in1 同样处理
    __m128i even1 = _mm_shuffle_epi8(in1, shuf_even);
    __m128i odd1  = _mm_shuffle_epi8(in1, shuf_odd);
    odd1 = _mm_slli_epi16(odd1, 4);
    __m128i packed1 = _mm_or_si128(odd1, even1);

    // 将 packed0（8 字节）和 packed1（8 字节）合并为一个 16 字节的 __m128i 向量
    __m128i result = _mm_unpacklo_epi64(packed0, packed1);
    return result;
}
void debugRoundFunc(uint8_t *a, string name) {
    cout << name << ": ";
    for (int i = 0; i < 32; i ++) {
        printf("0x%x ", a[i]);
    } cout << endl;
}
// 31 -> 16 15 -> 0
void roundFunction(__m128i &input, __m128i &output) {
//    print_m128i(input);
    m128_to_nibbles(state, input);
    reverse(state, state + 32);
//    debugRoundFunc(state, "State");

    for (int i = 0; i < 32; i++) {
        subBytes1[i] = sbox[state[i]];
    }
//    debugRoundFunc(subBytes1, "Sbox1");
    for (int i = 0; i < 16; i ++) {
        matrix[i] = 0; matrix[i + 16] = 0;
        for (int j = 0; j < 16; j ++) {
            if (MDS[i][j] == 0) ;
            else {
                matrix[i] ^= subBytes1[j];
                matrix[i + 16] ^= subBytes1[j + 16];
            }
        }
    }

//    debugRoundFunc(matrix, "Matrix");

    for (int i = 0; i < 32; i++) {
        subBytes2[i] = sbox[matrix[i]];
    }

//    debugRoundFunc(subBytes2, "Sbox2");

    for (int i = 0; i < 32; i++) {
        final[perm[i]] = subBytes2[i];
    }

//    debugRoundFunc(final, "Final");
    reverse(final, final + 32);

    output = nibbles_to_m128(final);

//    print_m128i(output);
}

// [0] 代表最低位
uint8_t *padding(uint8_t *data, size_t &length) {
    if (length == 0) {
        return data;
    }
    if (length % 48 == 0) {
        return data;
    }
    uint8_t *buffer;
    buffer = (uint8_t *)malloc(48 * ((length + 47) / 48) * sizeof (uint8_t));
    int idx = 0;
    int temp_length = length;
    while (temp_length >= 48) {
        memcpy(buffer + idx * 48, data + idx * 48, 48 * sizeof (uint8_t));
        temp_length -= 48;
        idx ++;
    }
    memcpy(buffer + idx * 48, data + idx * 48, temp_length * sizeof (uint8_t));
    reverse(buffer, buffer + idx * 48 + temp_length);
    buffer[idx * 48 + temp_length] = 1;
    int i = idx * 48 + temp_length + 1;
    while (i < (idx + 1) * 48) buffer[i ++] = 0;
    // for (int i = 0; i < 48 * ((length + 47) / 48); i ++) {
    //     printf("0x%X ", buffer[i]);
    //     if ((i + 1) % 16 == 0) {
    //         cout << endl;
    //     }
    // }
    reverse(buffer, buffer + (idx + 1) * 48);
    length = 48 * ((length + 47) / 48);
    // cout << length << " " << 48 * ((length + 47) / 48) << endl;
    return buffer;
}
__m128i add(__m128i a, __m128i b) {
    // 分解为 4 个 32 位元素
    uint32_t a_data[4], b_data[4], result[4];
    _mm_storeu_si128((__m128i*)a_data, a);
    _mm_storeu_si128((__m128i*)b_data, b);
    
    uint64_t carry = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t sum = (uint64_t)a_data[i] + (uint64_t)b_data[i] + carry;
        result[i] = sum & 0xFFFFFFFF; // 取低 32 位
        carry = sum >> 32;           // 记录进位
    }
    
    // 返回结果（忽略最终进位）
    return _mm_loadu_si128((__m128i*)result);
}
void R(__m128i *input, __m128i *output, __m128i &X0, __m128i &X1, __m128i &X2) {
    __m128i tmp[9];
    for (int i = 0; i < 9; i ++) {
        roundFunction(input[i], tmp[i]);
    }
    output[0] = _mm_xor_si128(tmp[8], X0);
    output[1] = _mm_xor_si128(tmp[0], input[3]);
    output[2] = _mm_xor_si128(tmp[1], input[6]);
    output[3] = _mm_xor_si128(tmp[2], X1);
    output[4] = _mm_xor_si128(tmp[3], input[4]);
    output[5] = _mm_xor_si128(tmp[4], X2);
    output[6] = _mm_xor_si128(tmp[5], input[8]);
    output[7] = _mm_xor_si128(tmp[6], input[2]);
    output[8] = _mm_xor_si128(tmp[7], input[0]);
    for (int i = 0; i < 9; i ++) {
        input[i] = output[i];
    }
}
void Initialization() {
    Z[0] = _mm_set_epi32(0x428a2f98, 0xd728ae22, 0x71374491, 0x23ef65cd);
    Z[1] = _mm_set_epi32(0xb5c0fbcf, 0xec4d3b2f, 0xe9b5dba5, 0x8189dbbc);
    Z[2] = _mm_set_epi32(0x71374491, 0x23ef65cd, 0x428a2f98, 0xd728ae22);
    K[0] = _mm_set_epi32(0x16402245, 0x96795a4c, 0x54550546, 0x722fc76b);
    K[1] = _mm_set_epi32(0x16d3059d, 0xfc040666, 0x57a839d2, 0xd5be827b);
    n = _mm_set_epi32(0x51af4471, 0xe8bcf6a2, 0x704d7116, 0x3021f4fd);
    M[0] = _mm_set_epi32 (0x84070aa5, 0xe8afb486, 0xda0561b6, 0xa1b88164);
    M[1] = _mm_set_epi32 (0x78b67f4b, 0xe34cc1f3, 0x4a02ecce, 0x30813270);
    M[2] = _mm_set_epi32 (0x9556a6ca, 0x986a3d6d, 0x7f9c5bb4, 0x0c99aa61);
    AD[0] = _mm_set_epi32(0xe0984999, 0x61971de2, 0x2fec2235, 0xb24c1309);
    AD[1] = _mm_set_epi32(0xbab0da98, 0xc3a386da, 0xa98d918b, 0x8cd88d5d);
    AD[2] = _mm_set_epi32(0x2fbe54a2, 0xc06d135b, 0xf0fdc7cc, 0x81f47625);
    m_len = 48;
    ad_len = 48;
    c_len = 48;
    // 初始化 state
    Input[0] = Z[1];
    Input[1] = K[0];
//    Input[2] = add(n, K[0]);
    Input[2] = _mm_xor_si128(K[0], n);
    Input[3] = _mm_setzero_si128();
    Input[4] = Z[0];
    Input[5] = _mm_setzero_si128();
    Input[6] = n;
    Input[7] = K[1];
    Input[8] = Z[2];
    for (int r = 0; r < 20; r ++) {
        R(Input, temp, Z[0], Z[1], Z[2]);
    }
    Output[0] = _mm_xor_si128(temp[0], K[0]); Output[1] = _mm_xor_si128(temp[1], K[0]); Output[2] = _mm_xor_si128(temp[2], K[0]);
    Output[3] = _mm_xor_si128(temp[3], K[0]); Output[4] = _mm_xor_si128(temp[4], K[1]); Output[5] = _mm_xor_si128(temp[5], K[0]);
    Output[6] = _mm_xor_si128(temp[6], K[1]); Output[7] = _mm_xor_si128(temp[7], K[1]); Output[8] = _mm_xor_si128(temp[8], K[1]);
    for (int i = 0; i < 9; i ++) {
        Input[i] = Output[i];
    }
}

// ad 处理过程
void ProcessAD() {
     for (int i = 0; i * 48 < ad_len; i ++) {       // Test Vector 里面最多只做一次
        R(Input, Output, AD[3 * i + 0], AD[3 * i + 1], AD[3 * i + 2]);
     }
}

// m 处理过程
// 如果 padding 补充了 p bit，生成的 Cipher 会进行截断
void Encryption() {
    for (int i = 0; i * 48 < m_len; i ++) {
        temp[0] = _mm_xor_si128(Input[0], Input[1]);
        temp[1] = _mm_xor_si128(Input[2], Input[6]);
        temp[2] = _mm_xor_si128(Input[3], Input[5]);
        roundFunction(temp[0], Output[0]);
        roundFunction(temp[1], Output[1]);
        roundFunction(temp[2], Output[2]);
        C[3 * i + 0] = _mm_xor_si128(Output[0], _mm_xor_si128(Input[4], M[3 * i + 0]));
        C[3 * i + 1] = _mm_xor_si128(Output[1], _mm_xor_si128(Input[7], M[3 * i + 1]));
        C[3 * i + 2] = _mm_xor_si128(Output[2], _mm_xor_si128(Input[8], M[3 * i + 2]));
        R(Input, Output, M[3 * i + 0], M[3 * i + 1], M[3 * i + 2]);
        for (int j = 0; j < 9; j ++) {
            Input[j] = Output[j];
        }
    }
}
void Decryption() {
    for (int i = 0; i * 48 < m_len; i ++) {
        temp[0] = _mm_xor_si128(Input[0], Input[1]);
        temp[1] = _mm_xor_si128(Input[2], Input[6]);
        temp[2] = _mm_xor_si128(Input[3], Input[5]);
        roundFunction(temp[0], Output[0]);
        roundFunction(temp[1], Output[1]);
        roundFunction(temp[2], Output[2]);
        M[3 * i + 0] = _mm_xor_si128(Output[0], _mm_xor_si128(Input[4], C[3 * i + 0]));
        M[3 * i + 1] = _mm_xor_si128(Output[1], _mm_xor_si128(Input[7], C[3 * i + 1]));
        M[3 * i + 2] = _mm_xor_si128(Output[2], _mm_xor_si128(Input[8], C[3 * i + 2]));
        R(Input, Output, C[3 * i + 0], C[3 * i + 1], C[3 * i + 2]);
        for (int j = 0; j < 9; j ++) {
            Input[j] = Output[j];
        }
    }
}
void Finalization() {
    Output[0] = _mm_xor_si128(Output[0], K[0]); Output[1] = _mm_xor_si128(Output[1], K[0]); Output[2] = _mm_xor_si128(Output[2], K[1]);
    Output[3] = _mm_xor_si128(Output[3], K[1]); Output[4] = _mm_xor_si128(Output[4], K[0]); Output[5] = _mm_xor_si128(Output[5], K[0]);
    Output[6] = _mm_xor_si128(Output[6], K[1]); Output[7] = _mm_xor_si128(Output[7], K[0]); Output[8] = _mm_xor_si128(Output[8], K[1]);
    for (int i = 0; i < 9; i ++) {
        Input[i] = Output[i];
    }

    for (int r = 0; r < 20; r ++) {
        R(Input, Output, K[0], Z[0], K[1]);
    }
    Output[0] = _mm_xor_si128(Output[0], K[1]); Output[1] = _mm_xor_si128(Output[1], K[0]); Output[2] = _mm_xor_si128(Output[2], K[0]);
    Output[3] = _mm_xor_si128(Output[3], K[0]); Output[4] = _mm_xor_si128(Output[4], K[1]); Output[5] = _mm_xor_si128(Output[5], K[0]);
    Output[6] = _mm_xor_si128(Output[6], K[0]); Output[7] = _mm_xor_si128(Output[7], K[1]); Output[8] = _mm_xor_si128(Output[8], K[1]);
    for (int i = 0; i < 9; i ++) {
        Input[i] = Output[i];
    }
}
signed main() {
    Initialization();
//    for (int i = 0; i < 9; i ++) {
//        print_m128i(Input[i]);
//    }
    if (ad_len) {
        // ad = padding(ad, ad_len);
        ProcessAD();
    }

//    for (int i = 0; i < 9; i ++) {
//        print_m128i(Input[i]);
//    }

    if (m_len) {
        // m = padding(m, m_len);
        Encryption();
        // Truncate();
    }


    Finalization();
    for (int i = 0; i < 9; i ++) {
        print_m128i(Input[i]);
    }
     // Tag
     __m128i res = _mm_setzero_si128();
     for (int i = 0; i < 9; i ++) {
         res = _mm_xor_si128(res, Output[i]);
     }
     for (int i = 0; i < 3; i ++) {
         cout << "C[" << i << "]: ";
         print_m128i(C[i]);
     }
     cout << "Tag: ";
     print_m128i(res);



//     解密
    Initialization();
    M[0] = _mm_setzero_si128();
    M[1] = _mm_setzero_si128();
    M[2] = _mm_setzero_si128();
    // ad 处理过程
    if (ad_len) {
        // ad = padding(ad, ad_len);
        ProcessAD();
    }
    // m 处理过程
    // 如果 padding 补充了 p bit，生成的 Cipher 会进行截断
    if (m_len) {
        // c = padding(c, c_len);
        Decryption();
        // Truncate();
    }
    Finalization();
    // Tag
    res = _mm_setzero_si128();
    for (int i = 0; i < 9; i ++) {
        res = _mm_xor_si128(res, Output[i]);
    }
    for (int i = 0; i < 3; i ++) {
        cout << "M[" << i << "]: ";
        print_m128i(M[i]);
    }
    cout << "Tag: ";
    print_m128i(res);
}

//signed main() {
//    __m128i a = _mm_set_epi32(0x51af4471, 0xe8bcf6a2, 0x704d7116, 0x3021f4fd);
//    __m128i b;
//    print_m128i(a);
//    roundFunction(a, b);
//    print_m128i(b);
//}