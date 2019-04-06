// Author: zeqiang-lai

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

/**
 * return '0' if a==b, otherwise '1'.
 */
inline char XOR(char a, char b)
{
    return a == b ? '0' : '1';
}

/**
 * computer remainder of the input bit string.
 * @return string, remainder.
 */
string crc_remainder(string input_bitstring, string poly)
{
    int len_input = input_bitstring.length();
    int len_poly = poly.length();
    string padding(len_poly-1, '0');
    string padded_input = input_bitstring.append(padding);

    // long division
    for(int i=0; i<len_input; ++i) {
        if(padded_input[i] == '0')
            continue;
        for(int j=i; j<i+len_poly; ++j)
            padded_input[j] = XOR(padded_input[j], poly[j-i]);
    }
    // extract remainder
    return padded_input.substr(len_input, len_poly-1);
}

string crc_check(string input_bitstring, string poly)
{
    int len_poly = poly.length();
    int len_input = input_bitstring.length() - len_poly;
    string padded_input = input_bitstring;

    // long division
    for(int i=0; i<len_input; ++i) {
        if(padded_input[i] == '0')
            continue;
        for(int j=i; j<i+len_poly; ++j)
            padded_input[j] = XOR(padded_input[j], poly[j-i]);
    }
    return padded_input;
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

int main(int argc, char* argv[])
{
    string ini_path = "crc.ini";
    if(argc == 2)
        ini_path = string(argv[2]);

    string send_string;
    string received_string;
    string poly;

    if(read_configuration(ini_path, poly, send_string, received_string) != 0)
        return -1;

    string crc_code_send = crc_remainder(send_string, poly);
    string crc_code_receive = crc_remainder(send_string, poly);
    string check_remainder = crc_check(send_string+crc_code_receive, poly);
    size_t pos1 = check_remainder.find('1');

    cout << "Message to be sent:         " << send_string << endl;
    cout << "CRC-Code:                   " << crc_code_send << endl;
    cout << "Message with checksum(crc): " << send_string+crc_code_send << endl;

    cout << "----------------------------------------------------" << endl;
    cout << "Message received:           " << received_string << endl;
    cout << "CRC-Code:                   " << crc_code_receive << endl;
    cout << "Message with checksum(crc): " << send_string+crc_code_receive << endl;
    cout << "Remainder:                  " << check_remainder << endl;
    if(check_remainder.find('1') == string::npos) 
        cout << "Valid:                      " << "true" << endl;
    else
        cout << "Valid:                      " << "false" << endl;
    return 0;
}