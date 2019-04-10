import sys
def XOR(a,b):
    if a==b:
        return '0'
    else:
        return '1'
def crc_remainder(input_string,poly):
    len_poly = len(poly)
    len_input = len(input_string)

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
    len_poly = len(poly)
    len_input = len(input_string)-len_poly+1

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
    if f==-1:
        print("file open error!")
    list=f.readlines()
    return list
    f.close()

def index_of_str(s1, s2):
   n= len(s1)
   for x in range(n):
      if s1[x]==s2:
         return x
   if s1[x]!=s2:
      return -1

def show_check_example(bit_string,poly):
    check_remainder=crc_check(bit_string,poly)
    print("Actual string received:           "+bit_string)
    print("Remainder:                        "+check_remainder)
    if index_of_str(check_remainder,"1")==-1:
        print("Valid:                            true")
    else:
        print("Valid:                            false")


file_path='crc.ini'
if len(sys.argv)==2:
    file_path = sys.argv[1]
list_1 =read_configuration(file_path)
send_string = ''.join(list_1[4]).replace("\n","")
poly = ''.join(list_1[3]).replace("\n","")
received_string = list_1[5].replace("\n","")
crc_code_send = crc_remainder(send_string,poly)
crc_code_receive = crc_remainder(send_string,poly)
actual_rec_str = received_string+crc_code_receive


print ("Message to be sent:               "+send_string)
print ("CRC-Code:                         "+crc_code_send)
print ("Message with checksum(crc):       "+send_string+crc_code_send)
print ("----------------------------------------------------")
print ("Message received:                 "+received_string)
print ("CRC-Code:                         "+crc_code_receive)
print ("Message with checksum(crc):       "+actual_rec_str)

print("True example:")
show_check_example(actual_rec_str,poly)
print("False example:")
ans = list(actual_rec_str)
ans[len(actual_rec_str)-1]=XOR(ans[len(actual_rec_str)-1],"1")
actual_rec_str=''.join(ans)
show_check_example(actual_rec_str,poly)
