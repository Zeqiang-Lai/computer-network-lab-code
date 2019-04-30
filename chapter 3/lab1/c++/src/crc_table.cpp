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

crc crc_table[256];

crc crc_remainder(uint8_t msg[], int n_bytes)
{
    crc remainder = 0;
    int index;
    for (int byte = 0; byte < n_bytes; ++byte)
    {
        remainder ^= (msg[byte] << (WIDTH - 8));
        index = remainder >> (WIDTH-8);
        remainder = (remainder << 8) ^ crc_table[index];
    }
    return remainder;
}

void build_crc_table()
{
    crc remainder;
    for(int dividend = 0; dividend < 256; ++dividend)
    {
        remainder = dividend << (WIDTH - 8);
        for(uint8_t bit = 8; bit > 0; --bit)
        {		
            if (remainder & TOPBIT)
                remainder = (remainder << 1) ^ POLYNOMIAL;
            else
                remainder = (remainder << 1);
        }
        crc_table[dividend] = remainder;
    }
}

int main()
{
    build_crc_table();
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