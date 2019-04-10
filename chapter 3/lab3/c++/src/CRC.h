#pragma once
#include<string>
using namespace std;


inline char XOR(char a, char b)
{
	return a == b ? '0' : '1';
}

class CRC
{
public:
	CRC() {
		poly = "10001000000100001";
	}
	string crc_remainder(string input_bitstring)
	{
		int len_input = input_bitstring.length();
		int len_poly = poly.length();
		string padding(len_poly - 1, '0');
		string padded_input = input_bitstring.append(padding);

		for (int i = 0; i < len_input; ++i) {
			if (padded_input[i] == '0')
				continue;
			for (int j = i; j < i + len_poly; ++j)
				padded_input[j] =XOR(padded_input[j], poly[j - i]);
		}
		return padded_input.substr(len_input, len_poly - 1);
	}
	string crc_check(string input_bitstring)
	{
		int len_poly = poly.length();
		int len_input = input_bitstring.length() - len_poly;
		string padded_input = input_bitstring;

		for (int i = 0; i < len_input; ++i) {
			if (padded_input[i] == '0')
				continue;
			for (int j = i; j < i + len_poly; ++j)
				padded_input[j] = XOR(padded_input[j], poly[j - i]);
		}
		return padded_input;
	}
private:
	string poly;
};