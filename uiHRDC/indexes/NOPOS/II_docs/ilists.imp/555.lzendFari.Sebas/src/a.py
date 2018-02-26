import array
#from struct import *
import sys
#3arr = unpack_from("i",open(sys.argv[1]).read())

arr=array.array("i")
cont = open(sys.argv[1]).read()
print len(cont)
arr.fromstring(cont)
arr = list(arr)
delta = []
for i in xrange(1,len(arr)):
	d = arr[i]-arr[i-1]
	if d<0:
		d=0
	delta.append(d)
print delta[:200]
delta.sort()
print delta[-1], delta[-2]


