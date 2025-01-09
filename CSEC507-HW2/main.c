#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROUNDS 16
#define POW2(x) (1 << (x))

typedef unsigned long bit64;

short int IParr[64] = 
                {58,50,42,34,26,18,10,2,
                60,52,44,36,28,20,12,4,
                62,54,46,38,30,22,14,6,
                64,56,48,40,32,24,16,8,
                57,49,41,33,25,17,9,1,
                59,51,43,35,27,19,11,3,
                61,53,45,37,29,21,13,5,
                63,55,47,39,31,23,15,7};

short int PCone_leftArr[28] = 
            {57, 49, 41, 33, 25, 17, 9,
            1, 58, 50, 42, 34, 26, 18,
            10, 2, 59, 51, 43, 35, 27,
            19, 11, 3, 60, 52, 44, 36};

short int PCone_rightArr[28] = 
            {63, 55, 47, 39, 31, 23, 15,
            7, 62, 54, 46, 38, 30, 22,
            14, 6, 61, 53, 45, 37, 29,
            21, 13, 5, 28, 20, 12, 4,};

short int sbox[8][4][16] = { 
        // SBOX 1
        {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
         {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
         {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
         {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
        // SBOX 2
        {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
         {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
         {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
         {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
        // SBOX 3
        {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
         {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
         {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
         {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
         // SBOX 4
        {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
         {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
         {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
         {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
         // SBOX 5
        {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
         {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
         {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
         {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
         // SBOX 6
        {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
         {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
         {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
         {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
         // SBOX 7
        {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
         {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
         {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
         {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
         // SBOX 8
        {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
         {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
         {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
         {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}} 
         };

short int Expansion[48] =
            {32, 1, 2, 3, 4, 5,
            4, 5, 6, 7, 8, 9,
            8, 9, 10, 11, 12, 13,
            12, 13, 14, 15, 16, 17,
            16, 17, 18, 19, 20, 21,
            20, 21, 22, 23, 24, 25,
            24, 25, 26, 27, 28, 29,
            28, 29, 30, 31, 32, 1};

short int PC2_Arr[48] =
            {14,17,11,24,1,5,
            3,28,15,6,21,10,
            23,19,12,4,26,8,
            16,7,27,20,13,2,
            41,52,31,37,47,55,
            30,40,51,45,33,48,
            44,49,39,56,34,53,
            46,42,50,36,29,32};

short int permutation[32] =
            {16,7,20,21,29,12,28,17,
            1,15,23,26,5,18,31,10,
            2,8,24,14,32,27,3,9,
            19,13,30,6,22,11,4,25};

short int FPArr[64] = {40, 8, 48, 16, 56, 24, 64, 32,
              39, 7, 47, 15, 55, 23, 63, 31,
              38, 6, 46, 14, 54, 22, 62, 30,
              37, 5, 45, 13, 53, 21, 61, 29,
              36, 4, 44, 12, 52, 20, 60, 28,
              35, 3, 43, 11, 51, 19, 59, 27,
              34, 2, 42, 10, 50, 18, 58, 26,
              33, 1, 41, 9, 49, 17, 57, 25};

short int rotations[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

bit64 keys[16] = {0}, part2_OUTPUT, IV = 0;;

bit64 generic_permute( short int * arr, int maxVal, int arrSize, short int outputSize, bit64 block)
{
    bit64 res = 0, bit = 0;
    int index = outputSize-1;
    for(int i = 0; i < arrSize ; i++)
    {
        bit = ( block >> ( maxVal - arr[i] ) ) & 0x01;
        res = res ^ ( bit << index--);
    }
    return res;
}

void print_bit64(bit64 x)
{
    printf("%016lx", x);
}

bit64 circularRotate(bit64 oldKey, short int times)
{
    bit64 newKey;

    newKey = (oldKey << times) ^ ( oldKey >> ( 28 - times ) );
    newKey = newKey & 0xFFFFFFF;

    return newKey;
}

bit64 apply_PC2(bit64 leftKey, bit64 rightKey)
{   
    bit64 bit = 0, res = 0;
    short int index = 47;
    for(int i = 0; i < 48 ; i++)
    {
        if( PC2_Arr[i] > 28 ) // GET FROM RIGTH
        {
            bit = ( rightKey >> ( 56 - PC2_Arr[i]) ) & 0x01;
        }
        else // GET FROM LEFT
        {
            bit = ( leftKey >> ( 28 - PC2_Arr[i]) ) & 0x01;
        } 
        res = res ^ ( bit << ( 47 - i ));
    }
    return res;
}

bit64 SBOX(short unsigned int input, int ind)
{
    int innerIndex = (input & 0x01) + ( (input >> 5 ) & 0x01)*2;

    int innerinnerIndex = ( (input >> 1 ) & 0x01) + ( (input >> 2 ) & 0x01)*2 + ( (input >> 3 ) & 0x01)*4 +  (  (input >> 4 ) & 0x01)*8;
    return sbox[ind][ innerIndex ][ innerinnerIndex ];
}

bit64 apply_SBOX(bit64 block)
{
    bit64 res = 0, output = 0;
    int index = 0;
    for(int i = 0; i < 8 ; i++)
    {
        short unsigned int input = (( block >> ( 6*i ) ) & 0x3F);

        output = SBOX(input, 7-i);

        res = res ^ ( output << index);
        index += 4;
    }
    return res;
}


bit64 apply_round(bit64 block, int i , bit64 roundKey)
{   
    block = generic_permute(Expansion,32,48,48,block);

    block = (block ^ roundKey) & 0xFFFFFFFFFFFF;

    block = apply_SBOX(block);

    block = generic_permute(permutation,32,32,32,block);
    
    return block;
}


bit64 handle_block(bit64 block)
{
    block = generic_permute(IParr,64,64, 64, block);

    for(int i = 0; i < ROUNDS ; i++)
    {
        bit64 rightPart =  (  block << 32 ) >> 32; 
        bit64 leftPart =  ( block >> 32);
        
        bit64 roundResult = apply_round(rightPart,i,keys[i]);

        leftPart = roundResult ^ leftPart;

        if( i == 15)
        {
            block = ( leftPart << 32) ^  rightPart;
            printf("ROUND: %d - ", i);
            print_bit64(block);
            printf("\n");
            break;
        }

        block = (  rightPart  << 32 ) ^ leftPart;

        printf("ROUND: %d - ",i);
        print_bit64(block);
        printf("\n");

    }   
    block =  generic_permute(FPArr, 64, 64, 64, block);
    printf("RESULT AFTER FP:");
    print_bit64(block);
    printf("\n");
    return block;
}

void generate_keys(bit64 key)
{
    bit64 leftKey = generic_permute( PCone_leftArr, 64, 28, 28, key ), rightKey = generic_permute(PCone_rightArr, 64, 28, 28, key );
    for(int i = 0; i < ROUNDS ; i++)
    {
        leftKey = circularRotate(leftKey, rotations[i]);
        rightKey = circularRotate(rightKey, rotations[i]);

        bit64 roundKey = apply_PC2(leftKey, rightKey);

        keys[i] = roundKey;

        printf("ROUND %d: Key: ", i+1);
        print_bit64(roundKey);
        printf("\n");
    }
}

void DES(bit64 *data, size_t size )
{   
    bit64 result = IV;
    for(size_t i = 0; i < size; i++ )
    {
        result = result ^ data[i];
        result =  handle_block(result);
    }
}

void generate_IV()
{
    // I seed the random generator with the current time
    // Otherwise, it generates the same IV for each run
    srand(time(NULL)); 

    for(int i = 0; i < 16 ; i++)
    {   
        IV = IV ^ ( (   (bit64)(rand() % 16)  ) << ( i * 4 ) );
    }

    printf("IV:");
    print_bit64(IV);
    printf("\n");
}

// Alkim Dogan 41 6c 6b 69 6d 20 44 6f || 67 61 6e 80 00 00 00 00

bit64 Alkim_Dogan[2] = {0x416c6b696d20446f, 0x67616e8000000000};
bit64 * big_data;

void part1()
{
    bit64 block[1] = {0x4e6f772069732074};
    printf("PART1\n");
    DES(block,1);
    printf("\n");
}

void part2()
{
    printf("PART2\n");
    generate_IV();
    DES(Alkim_Dogan, 2);
    printf("\n");
}

void part3()
{
    size_t size = POW2(23);
    big_data = malloc(sizeof(bit64) * size);

    printf("Size Of the data: %zu\n", size);
    DES(big_data, size);
}

int main()
{   
    bit64 key = 0x0123456789abcdef;

    printf("KEY-SCHEDULE\n");
    generate_keys(key);
    printf("\n");

    // PART1
    part1();

    printf("\n"); printf("\n"); printf("\n");

    // PART2
    part2();

    // PART3
    //part3();
}