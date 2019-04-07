def XOR(a,b):
    if cmp(a,b):
        return '1'
    else:
        return '0'
def crc_remainder(input_string,poly):
    len_input = len(input_string)
    len_poly = len(poly)
    padding = '0'*(len_poly-1)
    padded_input = input_string+padding
    for i in range(0,len_input,1):
        if padded_input[i] == '0':
            continue
        else:
            for j in range(i,i+len_poly,1):
                l = list(padded_input)
                l[j] = XOR(l[j], poly[j - i])
                padded_input=''.join(l)
    return padded_input[len_input:len_poly+len_input]

def crc_check(input_string,poly):
    len_input = len(input_string)
    len_poly = len(poly)
    padded_input = input_string
    for i in range(0, len_input, 1):
        if padded_input[i] == '0':
            continue
        else:
            for j in range(i, i + len_poly, 1):
                l = list(padded_input)
                l[j] = XOR(l[j], poly[j - i])
                padded_input = ''.join(l)
    return padded_input

def read_configuration(file_path):
    f=open(file_path,'r')
    list=f.readlines()
    return list
    f.close()

def contain(check_remainder):
    if check_remainder.find('1'):
        print "Valid:                                      true"
    else:
        print "Valid:                                      false"


list_1 =read_configuration('crc.ini')
send_string = ''.join(list_1[4]).replace("\n","")
poly = ''.join(list_1[3]).replace("\n","")
received_string = list_1[5].replace("\n","")
crc_code_send = crc_remainder(send_string,poly)
crc_code_receive = crc_remainder(send_string,poly)
check_remainder = crc_check(send_string+crc_code_receive,poly)


print "Message to be sent:               "+send_string
print "CRC-Code:                             "+crc_code_send
print "Message with checksum(crc): "+send_string+crc_code_send
print "----------------------------------------------------"
print "Message received:                  "+received_string
print "CRC-Code:                              "+crc_code_receive
print "Message with checksum(crc): "+send_string+crc_code_receive
print "Remainder:                             "+check_remainder
contain(check_remainder)
