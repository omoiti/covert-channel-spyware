import csv
import matplotlib.pyplot as plt
import math
import numpy as np
import sys, os
#sys.path.append('/home/bilal/Dropbox/summer research/bpt')
from dataToString import dataToString
from scipy import stats
from bitstring import BitArray
import time
import stat

import warnings
warnings.filterwarnings("ignore")

N=15

def main():
	
	# Read csv values and store data in x

	x = np.loadtxt('output.csv',
	                 unpack=True,
	                 delimiter = ',')
	
	#Remove data items greater than 160 to 160. Removes some outlier points.
	x[x > 160] = 160

	average=np.mean(x)
	average=int(round(average))

	#Find mode above average
	modeHigh=stats.mode(x[x > average])
	if(len(modeHigh[0])==0):
		#print("just noise")
		return
	xHigh=int(modeHigh[0][0])

	#Find mode below average
	modeLow=stats.mode(x[x < average])
	if(len(modeLow[0])==0):
		#print("just noise")
		return
	xLow=int(modeLow[0][0])

	#For numbers less than the average, change them to the low value
	x[x<average]=xLow

	#Find the average value and change values to the mode closest to that value
	
	low_index=0
	high_index=N
	loop_idx = 0

	#Loop through array of data 
	while loop_idx <len(x):
		mode_array_portion=stats.mode(x[low_index:high_index])
		mode_num=int(mode_array_portion[0][0])

		if abs(mode_num-xHigh)<abs(mode_num-xLow):
			num=xHigh
		else:
			num=xLow
		x[low_index:high_index]=num
		loop_idx += N
		low_index=high_index
		high_index=high_index+N

	#Save filtered data to csv
	np.savetxt("filtereddata.csv", x, delimiter=",")		
	dataToString()

main()








