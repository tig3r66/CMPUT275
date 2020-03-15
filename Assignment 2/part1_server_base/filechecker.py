import sys

def manhattan(x1, y1, x2, y2):
	return abs(x1-x2) + abs(y1-y2)

fin1 = open(sys.argv[1], "r")
fin2 = open(sys.argv[2], "r")

fin1.read(1)
counter1 = int(fin1.readline().strip())
fin2.read(1)
counter2 = int(fin2.readline().strip())

# if (counter1 != counter2):
	# raise Exception("number of lines do not match")

i = 0
sum1 = 0
sum2 = 0
while (i < counter1):
	line = fin1.readline()
	if (i == 0):
		prevLat1 = int(line[2:9])
		prevLon1 = int(line[10:19])
		prevLat2 = int(line[2:9])
		prevLon2 = int(line[10:19])
		i += 1
	else:
		currentLat1 = int(line[2:9])	
		currentLon1 = int(line[10:19])	
		currentLat2 = int(line[2:9])
		currentLon2 = int(line[10:19])
		sum1 +=  manhattan(prevLon1, prevLat1, currentLon1, currentLat1)
		sum2 += manhattan(prevLon2, prevLat2, currentLon2, currentLat2)
		i += 1

print(f'Sum 1: {sum1}\nSum 2: {sum2}')
if (sum1 == sum2):
	print("The sums are equivalent.")

fin1.close()
fin2.close()
