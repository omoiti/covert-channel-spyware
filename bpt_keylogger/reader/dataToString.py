from __future__ import print_function

import csv
import matplotlib.pyplot as plt
import math
import numpy as np
import bitarray
import binascii
import matplotlib.pyplot as plt
import charDict
import difflib
import warnings
import os
warnings.filterwarnings("ignore")

def dataToString():
	#Get filtered data
	data = np.loadtxt('filtereddata.csv',
	                 unpack=True,
	                 delimiter = ',')

	#get value of first number (either a high or low)
	val=data[0]


	one=max(data)
	zero=min(data)

	#Get rid of beginning noise
	i = 0
	while(i < len(data) and data[i] == one):
		i+=1
	data = data[i:len(data)]

	#loop until the second value (either a high or low) is reached. Store indices and values in array
	i=1
	indexArray=[]
	dataArray=[]
	timeArray=[]
	while i < len(data)-1:
		if(data[i] != data[i+1]):
			if(len(indexArray) > 0):
				if((i - indexArray[-1]) > 100):					
					timeArray.append(i-indexArray[-1])
					indexArray.append(i)
					dataArray.append(data[i])
			else:
				if((i) > 100):
					indexArray.append(i)
					timeArray.append(i)
					dataArray.append(data[i])
		i+=1

	
	#Find minimum value in array to find duration to transmit one bit
	if len(timeArray) == 0:
		return
	oneBitTime=min(timeArray)
	
	# print oneBitTime
	#Create array of bits
	k=0
	arrayOfBits=[]
	while k < len(dataArray):		
		if(timeArray[k]>oneBitTime):
			#this rounds to the nearest multiple for the value of one bit, then divides by that value to obtain the correct number of bits
			amountOfBits=int((timeArray[k]-timeArray[k]%oneBitTime)/oneBitTime)				

			if(dataArray[k]==one):
				arrayOfBits+=amountOfBits*['1']
			else:
				arrayOfBits+=amountOfBits*['0']
		else:
			if(dataArray[k]==one):
				arrayOfBits.append('1')
			else:
				arrayOfBits.append('0')
		#print(amountOfBits)
		k=k+1

	#loop through arrayOfBits and create bytes of characters to store in charBytes array 
	charBytes=[]

	#find first 0 (start of transmission)
	if '0' in arrayOfBits:
		startIndex=arrayOfBits.index('0')
	else:
		#print('dataToString.py: No 0 bits found')
		return

	while(len(arrayOfBits) % 8):
		arrayOfBits.append('1')

	read = ''
	results = open("results.txt", "a+")
	k = startIndex
	while k + 8 <= len(arrayOfBits):		
		char = ''.join(arrayOfBits[k:k+8])
		if char in charDict.binCharDict:
			print(charDict.binCharDict[char], end='')
			results.write(charDict.binCharDict[char])
			read = read + charDict.binCharDict[char]
		else:
			#This gets the closest matching bit sequence in the dictionary
			closest = difflib.get_close_matches(char, charDict.binCharDict.keys(), 3)
			print(charDict.binCharDict[closest[0]], end='')
			results.write(charDict.binCharDict[closest[0]])
			read = read + charDict.binCharDict[closest[0]]
		k+=8
	results.write("\n")
	results.close()
	print()


