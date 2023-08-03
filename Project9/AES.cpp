#include "AES.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <time.h>
using namespace std;
// S盒
unsigned char S[256] =
{
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

//密钥扩展
unsigned int W[44];

//列混淆时用到的正矩阵
extern unsigned char positive_matrix[4][4] =
{
    0x02, 0x03, 0x01, 0x01,
    0x01, 0x02, 0x03, 0x01,
    0x01, 0x01, 0x02, 0x03,
    0x03, 0x01, 0x01, 0x02
};

//明文
unsigned char P[MAX_LENGTH];
//解密之后的明文
unsigned char De_P[MAX_LENGTH];
//分组后的128明文
unsigned char P128[16];

//密文
unsigned char C[MAX_LENGTH];
//分组后的128密文
unsigned char C128[16];

//将128比特转换为状态矩阵
void array_to_mat(unsigned char p[], unsigned char state_mat[][4])
{
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            state_mat[row][col] = p[col * 4 + row];
        }
    }
}

//将状态矩阵转换为128密文
void mat_to_array(unsigned char state_mat[][4], unsigned char c[])
{
    int cnt = 0;
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            c[cnt++] = state_mat[row][col];
        }
    }
}

//将1个32位的密钥，转换为4个8位密钥,key8[0]为原来key32的高位
void key32_to_key8(unsigned int key32, unsigned char* key8)
{
    unsigned int mark = 0xff;
    int left_bit;
    for (int i = 0; i < 4; i++)
    {
        left_bit = (3 - i) * 8;
        key8[i] = ((mark << left_bit) & key32) >> left_bit;
    }
}

//将4个8位的密钥，转换为1个32位密钥
unsigned int key8_to_key32(unsigned char* key8)
{
    unsigned int temp_key = 0;
    for (int i = 0; i < 4; i++)
    {
        temp_key ^= ((unsigned int)key8[i] << ((3 - i) * 8));
        if (i == 3)
        {
            return temp_key;
        }
    }

}

//字节替换
unsigned char SubBytes(unsigned char input)
{
    unsigned char pre = 0xf0;
    unsigned char suf = 0x0f;
    unsigned int row = (input & pre) >> 4, col = input & suf;
    return S[row * 16UL + col];
}

//行位移
void ShiftRows(unsigned char state_mat[][4])
{
    unsigned char temp_row[4];
    for (int i = 1; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp_row[j] = state_mat[i][(j + 4 + i) % 4];
        }
        for (int j = 0; j < 4; j++)
        {
            state_mat[i][j] = temp_row[j];
        }
    }
}

//有限域上的乘法
unsigned char multi_finite_field(unsigned char a, unsigned char b)
{
    unsigned char ans = 0, v;
    for (int counter = 0; counter < 8; counter++)
    {
        if ((b & 0x01) != 0)
        {
            ans ^= a;
        }
        v = a >> 7;
        a <<= 1;
        if (v != 0)
        {
            a ^= 0x1b;
        }
        b >>= 1;
    }
    return ans;

}

//列混合
void MixColumns(unsigned char state_mat[][4])
{
    unsigned char ans_mat[4][4];
    memset(ans_mat, 0, sizeof(ans_mat));
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                ans_mat[i][j] ^= multi_finite_field(positive_matrix[i][k], state_mat[k][j]);
            }
        }
    }
    memcpy(state_mat, ans_mat, sizeof(ans_mat));
}

//轮密钥加，cnt标记这是第几轮循环，正确
void AddRoundKey(unsigned char state_mat[][4], int cnt)
{
    unsigned char key8[4];
    for (int col = 0; col < 4; col++)
    {
        key32_to_key8(W[4 * cnt + col], key8);
        for (int row = 0; row < 4; row++)
        {
            state_mat[row][col] ^= key8[row];
        }
    }
}


//密钥扩展时的T函数，cnt代表轮数
unsigned int T(unsigned int input, int cnt)
{
    unsigned char key8[4];
    key32_to_key8(input, key8);
    unsigned char temp[4];
    temp[0] = SubBytes(key8[1]);
    temp[1] = SubBytes(key8[2]);
    temp[2] = SubBytes(key8[3]);
    temp[3] = SubBytes(key8[0]);
    unsigned int ans = key8_to_key32(temp);
    ans = ans ^ rcon[cnt - 1];
    return ans;
}

//密钥扩展函数
void KeyExpansion(unsigned char* init_key)
{
    unsigned int temp_key = 0;
    for (int i = 0; i < 16; i++) // 对输入的8位初始密钥，转换到32位的W[0],W[1],W[2],W[3]中
    {
        temp_key ^= ((unsigned int)init_key[i] << ((3 - (i % 4)) * 8));
        if (i % 4 == 3)
        {
            W[i / 4] = temp_key;
            temp_key = 0;
        }
    }
    int cnt;
    for (int i = 1; i <= 10; i++) //10轮密钥扩展
    {
        cnt = i * 4; //cnt来当做更新W的下标
        while (1)
        {
            if (cnt % 4 != 0)
            {
                W[cnt] = W[cnt - 1] ^ W[cnt - 4];
            }
            else
            {
                W[cnt] = W[cnt - 4] ^ T(W[cnt - 1], i);
            }
            cnt++;
            if (cnt % 4 == 0)
            {
                break;
            }
        }
    }
}

//加密
void encryption()
{
    int len = 256 * 16;
    int group_cnt = (len + 15) / 16; //进行明文分组
    for (int group = 0; group < group_cnt; group++)
    {
        unsigned char state_mat[4][4];
        memcpy(P128, P + (group * 16), 16);
        array_to_mat(P128, state_mat);
        AddRoundKey(state_mat, 0);
        for (int i = 1; i <= 10; i++)
        {
            for (int row = 0; row < 4; row++) //字节替换
            {
                for (int col = 0; col < 4; col++)
                {
                    state_mat[row][col] = SubBytes(state_mat[row][col]);
                }
            }
            ShiftRows(state_mat);
            if (i != 10)
            {
                MixColumns(state_mat);
            }
            AddRoundKey(state_mat, i);

        }
        mat_to_array(state_mat, C128);
        memcpy(C + (group * 16), C128, 16);
    }
}

//初始密钥
unsigned char key[16];


int main()
{
    ios::sync_with_stdio(false);
    cout << "请输入初始密钥(按字节输入，共16个字节.如'00 01 ... 0d 0e 0f')：";
    unsigned int xx;//作为“中间人”接收用户输入单字节密钥
    for (int i = 0; i < 16; i++)  // 输入密钥
    {
        cin >> hex >> xx;
        key[i] = xx;
    }
    cout << "输入的密钥是：" << endl;
    for (int i = 0; i < 16; i++) // 输入密钥
    {
        printf("%02x  ", key[i]);
    }
    cout << "\n" << endl;
    KeyExpansion(key); //密钥扩展
    for (int p = 0; p < 1; p++) {
        cout << "输入的明文为:" << endl;
        for (int b = 0; b < 16 * 256; b++) {
            char c = b % 16;
            unsigned int d;
            d = b / 16;
            unsigned int cc;
            if (c == 0) {
                P[b] = d;
            }
            if (c > 0 && c < 16) {
                cc = rand() % 256;
                P[b] = cc;
            }
        }
        for (int i = 0; i < 256 * 16; i++)
        {
            printf("%02x  ", P[i]);
        }
        int len = 16 * 256;
        cout << endl;
        encryption(); //加密
        cout << endl;
        cout << "加密后的密文为:" << endl;
        for (int i = 0; i < 256 * 16; i++)
        {
            printf("%02x  ", C[i]);
        }
        cout << "\n" << endl;
    }


    return 0;
}