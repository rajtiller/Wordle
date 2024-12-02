#!/usr/bin/env python3
inp = ''
mode = input("Letter to number (l) or Number to letter (n)? ")
letter_dict = {'G':3,'g':3,'Y':1,'y':1,'B':0,'b':0}
number_dict = {3:'G',1:'Y',0:'B'}
if mode == 'l':
    while (inp != 'q' and inp != 'Q'):
        to_print = ""
        inp = input("String of letters (B/G/Y): ")
        try:
            to_print+=letter_dict[inp[4]]*256
            to_print+=letter_dict[inp[3]]*64
            to_print+=letter_dict[inp[2]]*16
            to_print+=letter_dict[inp[1]]*4
            to_print+=letter_dict[inp[0]]*1
            print("Conversion:",to_print,"\n")
        except:
            continue
else:
    while (inp != 'q' and inp != 'Q'):
        to_print = ""
        inp = input("Number: ")
        try:
            inp = int(inp)
            to_print+=number_dict[int(inp/256 % 4)]
            to_print+=number_dict[int(inp/64 % 4)]
            to_print+=number_dict[int(inp/16 % 4)]
            to_print+=number_dict[int(inp/4 % 4)]
            to_print+=number_dict[int(inp/1 % 4)]
            print("Conversion:",to_print,"\n")
        except:
            continue
