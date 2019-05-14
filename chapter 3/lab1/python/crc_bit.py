import sys
import binascii
POLYNOMIAL = 0x1021
INITIAL_REMAINDER = 0xFFFF
FINAL_XOR_VALUE = 0x0000
WIDTH = 16
TOPBIT = (1 << (WIDTH - 1))
crcTable={}

def build_crc_table():
        SHIFT = WIDTH - 8
        for step in range(0, 256):
            remainder = step << SHIFT
            for bit in range(8, 0, -1):
                if remainder & TOPBIT:
                    remainder = ((remainder << 1) ) ^ POLYNOMIAL
                else:
                    remainder = remainder << 1
            crcTable[step] = remainder

def crc_remainder(message, nBytes):
    build_crc_table()
    remainder = 0
    byte = 0
    while byte < nBytes:
        remainder ^=(ord(message[byte])<<(WIDTH-8))
        index=remainder>>(WIDTH-8)
        remainder=(remainder<<8)^crcTable[index]
        byte+=1


    return hex(remainder)[2:]

def crc_check(msg_with_crc,n_bytes):
    return (crc_remainder(msg_with_crc,n_bytes)==0)

def to_bytes_stream(string):
    if len(string)%8!=0:
        for i in range(0,((int)(len(string)/8)+1)*8-len(string),1):
            string='0'+string
    zifu=[]
    for i in range(0,(int)(len(string)/8),1):
        zifu.append(chr(int((string[8*i:8*i+8]),2)))
    return zifu

def to_char_stream(list):
    list_3=[]
    for i in range(0,len(list),1):
        list_3.append(format(ord(list[i]),'b'))
    for i in range(0,len(list_3),1):
        if len(list_3[i])==8:
            print(list_3[i], end="")
        else:
            print(0,end="")
            print(list_3[i], end="")

    print(" ")

def read_configuration(file_path):
    f=open(file_path,'r')
    if f==-1:
        print("file open error!")
    list=f.readlines()
    return list
    f.close()

def show_send_string_result(sendstring):
    print("################# Send String #####################")
    print("Message: ", sendstring)
    send_string=to_bytes_stream(sendstring)
    crc_code=crc_remainder(send_string,len(send_string))
    print("Message in Hex: ",end=" ")
    for i in range(0,len(send_string),1):
        print(format(ord(send_string[i]),'x'),end="")
    print("")
    print("CRC Code: ", crc_code)
    msg_with_crc=[]
    print("Message with CRC Code in Hex: ",end="")
    for i in range(0, len(send_string), 1):
        msg_with_crc.append(send_string[i])
    for i in range(0, 4, 2):
        a = binascii.unhexlify(crc_remainder(send_string, len(send_string))[i:i + 2])
        msg_with_crc.append(a)
    for i in range(0,len(msg_with_crc),1):
        print(format(ord(msg_with_crc[i]),'x'),end="")
    print(" ")
    print("Message with CRC Code in Binary: ",end="")
    to_char_stream(msg_with_crc)



def show_receive_string_result(receivestring):
    print("################# Receive String #####################")
    print("Message: ", receivestring)
    receive_string=to_bytes_stream(receivestring)
    check_code=crc_remainder(receive_string,len(receive_string))
    print("Message in Hex: ",end=" ")
    for i in range(0,len(receive_string),1):
        print(format(ord(receive_string[i]),'x'),end="")
    print("")
    if check_code=='0':
        print("Receive string is valid!\n")
    else:
        print("Invalid Receive string. CRC code=",check_code)

file_path='crc.ini'
if len(sys.argv)==2:
    file_path = sys.argv[1]
list_1 =read_configuration(file_path)
send_string = ''.join(list_1[4]).replace("\n","")
poly = ''.join(list_1[3]).replace("\n","")
receive_string = list_1[5].replace("\n","")
message=[]
for i in range(0,len(send_string),1):
    message.append(send_string[i])
for i in range(0,4,2):
    a=binascii.unhexlify(crc_remainder(send_string,len(send_string))[i:i+2])
    message.append(a)
show_send_string_result(send_string)
show_receive_string_result(receive_string)