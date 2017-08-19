#!/usr/bin/env python3

import sys
import argparse
import struct
import itertools
import cProfile

def main():
    #Parse command line arguments for appropriate inputs
    parser = argparse.ArgumentParser()
    parser.add_argument('--encode', action='store_true')
    parser.add_argument('--decode', action='store_true')
    parser.add_argument('--infile', type=str, help = 'Name of input file')
    parser.add_argument('--outfile', type=str, help = 'Name of output file')

    #Create a list of args
    args = parser.parse_args()
    #Assign infile and outfile name to variables
    infile_name = args.infile
    outfile_name = args.outfile

    #Return 1 if encode and 2 if decode
    encode_or_decode = read_magic_number(infile_name)

    #Perform appropriate functions if encode or decode
    try:
        if args.encode or encode_or_decode == 1:
            read_return = read_infile(infile_name, args)
            whole_file = read_return[0]
            blocksize = read_return[1]
            encoding = move_to_front(whole_file)
            rl_result = run_length(encoding)
            ascii_list = into_ascii(rl_result)
            write_outfile(outfile_name, blocksize, ascii_list, args)
        elif args.decode or encode_or_decode == 2:
            read_return = read_infile(infile_name, args)
            whole_file = read_return[0]
            blocksize = read_return[1]
            reverse_rl = from_ascii(whole_file)
            reverse_mtf = run_length_decode(reverse_rl)
            result = mtf_decode(reverse_mtf)
            write_outfile(outfile_name, blocksize, result, args)
    except FileNotFoundError:
        print("Error: File not found")
        exit()

######################################################
#               ....Encode Functions....             #
######################################################

#Convert the run length encoding into ascii representation
def into_ascii(rl_result):
    ascii_list = []
    for item in rl_result:
        if isinstance(item, int): #If item is an int then add 128
            ascii_list.append(chr(item+128)) #Add to list
        elif not isinstance(item, int):
            ascii_list.append(item) #If char then add to list
    ascii_list = "".join(ascii_list)
    return ascii_list

#Perform the move to front encoding
def move_to_front(whole_file):
    #s = "abcabcaaaaaaaab"
    list_after_encoding = []
    index = 0
    encoding = []
    #For each item in the text string
    for char in whole_file: #If not in list then add to list_after_encoding
        if not (char in list_after_encoding):
            index = index+1
            list_after_encoding.append(char)
            encoding.append(index)
            encoding.append(char)
        else: #If already in list then add position to encoding
            position = list_after_encoding.index(char)
            encoding.append(position+1)
            list_after_encoding.pop(position)
            list_after_encoding.insert(0, char)

    return encoding

#Remove redundancies by converting strings of 1 to 0 followed by num of 1's
def run_length(encoding):
    #Split list into groups of same item
    #print(encoding)
    rl_list = [list(g) for (_,g) in itertools.groupby(encoding)]
    #print(rl_list)
    index = 0
    #Look through and find groups of more than 2 1's
    for item in rl_list:
        index = index+1
    #Change to zero and number of ones
        if (item[0] == 1) and (len(item) > 2):
            item = [0,len(item)]
            rl_list[index-1] = item
    #Combine result into result list
    rl_result = sum(rl_list, [])

    return rl_result

######################################################
#               ....Decode Functions....             #
######################################################

#Reverse the mover to front encoding
def mtf_decode(reverse_mtf):
    list_after_decoding = []
    index = 0
    decoding = []
    for item in reverse_mtf:
        if index < len(reverse_mtf)-1:
            if (type(reverse_mtf[index]==int)) and (type(reverse_mtf[index+1]) == str):
                decoding.append(reverse_mtf[index+1])
                list_after_decoding.append(reverse_mtf[index+1])
            elif (type(reverse_mtf[index+1]==str)) and (type(reverse_mtf[index]) == int):
                list_after_decoding.append(decoding[item-1])
                temp = decoding.pop(item-1)
                decoding.insert(0, temp)
        index = index+1

    #list_after_decoding = list_after_decoding[:-1]
    try:
        list_after_decoding.append(decoding[item-1])
    except TypeError:
        pass

    result = "".join(list_after_decoding)

    return result

#Convert the ascii string back into the run length version
def run_length_decode(reverse_rl):
    #print(reverse_rl)
    index = 0
    for item in reverse_rl:
        index = index+1
        if(item == 0):
            #print(index)
            num_to_add = reverse_rl.pop(index)
            reverse_rl.pop(index-1)
            for i in range(num_to_add):
                reverse_rl.insert(index-1, 1)
    #print(reverse_rl)
    return reverse_rl

#Get integers from file and convert the ascii values back into integers or characters
def from_ascii(whole_file):
    str1 = whole_file
    #str1 = "\x81a\x82b\x83c\x81\x82\x83\x83\x80\x87\x83"
    reverse_rl = []
    for item in str1:
        temp = ord(item)
        if temp > 127:
            temp = temp - 128
            reverse_rl.append(temp)
        elif temp <= 127:
            temp = chr(temp)
            reverse_rl.append(temp)
    #print(reverse_rl)
    return reverse_rl

######################################################
#            ....Read/Write Functions....            #
######################################################

#Read in the input file, return a tuple with file and blocksize
def read_infile(infile_name, args):
    #Read in the bytes to confirm filetype and get block size
    infile = open(infile_name, mode="rb")
    byte_list = []
    #Read in bytes an add to list
    for j in range(8):
        ch = infile.read(1)
        byte_list.append(ch)
    #Join lists to be byte string
    mag_num_in_bytes = b''.join(byte_list[0:4])
    size_in_bytes = b''.join(byte_list[4:8])
    #Convert byte strings to int and char string
    blocksize = int.from_bytes(size_in_bytes, byteorder="little")
    magic_number = bytes.decode(mag_num_in_bytes, 'latin-1')
    #Check if the header is valid for for ph1 file
    if (args.encode) and (magic_number != "\xab\xba\xbe\xef"):
        print("Invalid header at start of file")
        exit()
    #Check if the header is valid for a ph2 file
    if (args.decode) and (magic_number != "\xda\xaa\xaa\xad"):
        print("Invalid header at start of file")
        exit()

    infile.close()
    #Get the rest of the file after the header bytes
    infile = open(infile_name, encoding="latin-1", mode="r")
    whole_file = infile.read()
    whole_file = whole_file[8:]
    #print(whole_file)
    infile.close()

    return (whole_file, blocksize)

#Check magic number to determine file type to be read
#Return 1 if ph1 and 2 if ph2
def read_magic_number(infile_name):
    #Read in the bytes to confirm filetype and get block size
    infile = open(infile_name, mode="rb")
    byte_list = []
    #Read in bytes an add to list
    for j in range(4):
        ch = infile.read(1)
        byte_list.append(ch)
    #Join lists to be byte string
    mag_num_in_bytes = b''.join(byte_list[0:4])
    magic_number = bytes.decode(mag_num_in_bytes, 'latin-1')
    #Check if the header is valid for for ph1 file
    if (magic_number == "\xab\xba\xbe\xef"):
        return 1
    #Check if the header is valid for a ph2 file
    if (magic_number == "\xda\xaa\xaa\xad"):
        return 2

#Write the magic number and blocksize to start for ph1/ph2 and then rest
def write_outfile(outfile_name, blocksize, whole_file, args):
    outfile = open(outfile_name, encoding="latin-1", mode="w")
    #Write ph2 magic number
    if args.encode:
        outfile.write("\xda\xaa\xaa\xad")
    #Write ph1 magic number
    elif args.decode:
        outfile.write("\xab\xba\xbe\xef")
    #Convert the blocksize to bytes and write to file
    bytes = struct.pack("I", blocksize)
    chars = [chr(c) for c in bytes]
    s = "".join(chars)
    outfile.write(s)
    outfile.write(whole_file)
    outfile.close()


if __name__=='__main__':
    #cProfile.run('main()')
    main()
