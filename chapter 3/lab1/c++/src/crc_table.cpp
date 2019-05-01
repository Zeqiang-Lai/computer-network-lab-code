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

/**
 * computer remainder of the input bytes stream.
 * @return remainder.
 */
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

/**
 * check the validity of the input bit stream with crc code.
 * @return 1 if it is valid, otherwise 0.
 */
int crc_check(uint8_t msg_with_crc[], int n_bytes)
{
    return (crc_remainder(msg_with_crc, n_bytes) == 0);
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

// the code below is the same as the code in crc_bit.cpp.

/**
 * convert binary bit string into bytes stream.
 */
int to_bytes_stream(char const bit_string[], int n_char, uint8_t** bit_stream)
{
    int n_bytes = (n_char+7) / 8;
    *bit_stream = new uint8_t[n_bytes];

    // pad zero at the tail
    // uint8_t tmp_byte;
    // for(int byte=0; byte<n_bytes; ++byte)
    // {
    //     tmp_byte = 0;
    //     for(int bit=0; bit<8; ++bit) {
    //         if(bit_string[(byte+1)*8-bit-1] == '1')
    //             tmp_byte |= 1 << bit;
    //     }
    //     (*bit_stream)[byte] = tmp_byte;
    // }

    // pad zero at the front
    uint8_t tmp_byte;
    for(int byte=n_bytes-1; byte>=0; --byte)
    {
        tmp_byte = 0;
        for(int bit=0; bit<8; ++bit) {
            if(bit_string[n_char-1-(bit+(n_bytes-byte-1)*8)] == '1')
                tmp_byte |= 1 << bit;
        }
        (*bit_stream)[byte] = tmp_byte;
    }

    return n_bytes;
}

/**
 * convert bytes stream into binary bit string.
 */
int to_char_stream(uint8_t const bit_stream[], int n_bytes, char** bit_string)
{
    int n_char = n_bytes*8;
    *bit_string = new char[n_char+1];

    int idx = 0;
    for(int byte=0; byte<n_bytes; ++byte){
        for(int bit=7; bit >=0; bit--) {
            // check if the 'bit'th bit in bit_stream[byte] is 1.
            if(bit_stream[byte] & (1 << bit)) {
                (*bit_string)[idx] = '1';
                idx++;
            } else {
                (*bit_string)[idx] = '0';
                idx++;
            }
        }
    }
    (*bit_string)[idx] = '\0';
    return n_char;
}

int read_configuration(string file_path, string& poly, string& send_string, string& received_string)
{
    // open configuration file
    ifstream inifile(file_path);
    if(inifile.fail()) {
        cerr << "Error: configuration file not found." << endl;
        inifile.close();
        return -1;
    }

    // Parsing, only line 4-6 are used.
    int num_line = 0;
    string line;
    while(getline(inifile, line)) {
        num_line ++;
        switch(num_line) {
            case 4:
                poly = line;
                break;
            case 5:
                send_string = line;
                break;
            case 6:
                received_string = line;
                break;
            default:
                break;
        }
    }
    inifile.close();
    return 0;
}

void show_send_string_result(string send_string)
{
    printf("################# Send String #####################\n");
    uint8_t* msg;
    int len_msg = to_bytes_stream(send_string.c_str(), send_string.length(), &msg);

    //-----------------Computer CRC Code---------------//
    crc crc_code;
    crc_code = crc_remainder((uint8_t*)msg, len_msg);

    printf("Message: %s\n", send_string.c_str());
    printf("Message in Hex: ");
    for(int i=0; i<len_msg; ++i)
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

    char* bit_string;
    to_char_stream(msg_with_crc, len_msg+sizeof(crc), &bit_string);
    printf("Message with CRC Code in Binary: ");
    printf("%s", bit_string);
    printf("\n");
}


void show_receive_string_result(string receive_string)
{
    printf("################# Receive String #####################\n");
    printf("Message: %s\n", receive_string.c_str());
    if(receive_string.length() <= WIDTH) {
        printf("Invalid receive string: too short, at least 33 bit(32 bit crc code).\n");
        return;
    }

    uint8_t* msg;
    int len_msg = to_bytes_stream(receive_string.c_str(), receive_string.length(), &msg);
    printf("Message in Hex: ");
    for(int i=0; i<len_msg; ++i)
        printf("%x", msg[i]);
    printf("\n");

    crc crc_code = crc_remainder((uint8_t*)msg, len_msg);
    if(crc_code == 0)
        printf("Receive string is valid!\n");
    else
        printf("Invalid Receive string. CRC code=%d\n", crc_code);
}

int main(int argc, char* argv[])
{
    build_crc_table();
    //---------------Read Configuration-------------------//
    string ini_path = "crc.ini";
    if(argc == 2)
        ini_path = string(argv[1]);

    string send_string;
    string received_string;
    string poly;

    if(read_configuration(ini_path, poly, send_string, received_string) != 0)
        return -1;
    
    //--------------show result------------------------//
    show_send_string_result(send_string);
    show_receive_string_result(received_string);

    return 0;
}