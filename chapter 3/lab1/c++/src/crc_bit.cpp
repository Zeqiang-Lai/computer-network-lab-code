// Author: zeqiang-lai

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

typedef uint16_t crc;
#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0x1021
// 1 0001 0000 0010 0001

/**
 * computer remainder of the input bytes stream.
 * @return remainder.
 */
crc crc_remainder(uint8_t msg[], int n_bytes)
{
    crc remainder = 0;	
    for (int byte = 0; byte < n_bytes; ++byte)
    {
        remainder ^= (msg[byte] << (WIDTH - 8));
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            if (remainder & TOPBIT)
                remainder = (remainder << 1) ^ POLYNOMIAL;
            else
                remainder = (remainder << 1);
        }
    }
    return remainder;
}

/**
 * check crc code of input bit string.
 * @return string, remainder of input bit string. if it contains no '1', 
 * it means that the input bit string is valid.
 */
int crc_check(uint8_t msg_with_crc[], int n_bytes)
{
    return (crc_remainder(msg_with_crc, n_bytes) == 0);
}


int main()
{
    //-----------------Computer CRC Code---------------//
    char msg[] = "hello";
    int len_msg = strlen(msg);

    crc crc_code;
    crc_code = crc_remainder((uint8_t*)msg, len_msg);

    printf("Message: %s\n", msg);
    printf("Message in Hex: ");
    for(int i=0; i<strlen(msg); ++i)
        printf("%x", msg[i]);
    printf("\n");
    printf("CRC Code: %x\n", crc_code);

    //--------------Pad the crc code------------------//
    // be careful about the bytes sequence.
    uint8_t* msg_with_crc = new uint8_t[len_msg+sizeof(crc)];
    for(int i=0; i<len_msg; ++i)
        msg_with_crc[i] = msg[i];
    for(int i=0; i<sizeof(crc); ++i)
        msg_with_crc[i+len_msg] = (crc_code >> ((sizeof(crc)-i-1)*8)) % 0x100;
    
    printf("Message with CRC Code in Hex: ");
    for(int i=0; i<len_msg+sizeof(crc); ++i)
        printf("%x", msg_with_crc[i]);
    printf("\n");
       
    //---------------Check--------------------------//
    crc check_code = crc_remainder(msg_with_crc, len_msg+sizeof(crc));
    printf("Check code: %x\n", check_code);

    printf("Flip first byte\n");
    msg_with_crc[0] = ~msg_with_crc[0];

    check_code = crc_remainder(msg_with_crc, len_msg+sizeof(crc));
    printf("Check code: %x\n", check_code);
    return 0;
}