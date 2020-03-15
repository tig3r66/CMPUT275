import sys


def manhattan(x1, y1, x2, y2):
	return abs(x1-x2) + abs(y1-y2)


def get_sum(sum, in_file):
	i = 0
	for line in in_file:
		if i < 1:
			temp_coord = line.strip().split()
			i += 1
		else:
			new_coord = line.strip().split()
			try:
				sum += manhattan(int(temp_coord[1]), int(temp_coord[2]),
					int(new_coord[1]), int(new_coord[2]))
				temp_coord = new_coord
			except IndexError:
				pass
	return sum


if __name__ == '__main__':
	fin1 = open(sys.argv[1], "r")
	fin2 = open(sys.argv[2], "r")

	fin1.read(1)
	counter1 = int(fin1.readline().strip())
	fin2.read(1)
	counter2 = int(fin2.readline().strip())

	sum1, sum2 = 0, 0
	sum1 = get_sum(sum1, fin1)
	sum2 = get_sum(sum2, fin2)

	print(f'{sys.argv[1]}: {sum1}\n{sys.argv[2]}: {sum2}')
	if (sum1 == sum2):
		print("EQUIVALENT")
	else:
		print("INCORRECT OUTPUT")

	fin1.close()
	fin2.close()
