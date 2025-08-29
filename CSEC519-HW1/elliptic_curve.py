def gcd_extended(a, b, x, y):
	if a == 0: 
		x[0] = 0
		y[0] = 1
		return b 

	x1, y1 = [0], [0]
	gcd = gcd_extended(b % a, a, x1, y1)

	x[0] = y1[0] - (b // a) * x1[0] 
	y[0] = x1[0] 
	return gcd 

def find_gcd(a, b):
	x, y = [1], [1]
	return gcd_extended(a, b, x, y)

def get_inverse(A, M):
	x, y = [1], [1]
	g = gcd_extended(A, M, x, y)

	if g == 1:
		return (x[0] % M + M) % M

	return None


def get_all_points(field_size, a, b):
	points = []
	for i in range(field_size):
		rhs = (i * i * i + a * i + b) % field_size 
		for j in range(field_size):
			if ( j * j) % field_size == rhs:
				points.append((i,j))
				if j != 0:
					points.append((i, field_size-j))
				break
	points.extend([(float("inf"),float("inf"))])
	return points


def get_double(p, field_size, a, b):

	x,y = p[0], p[1]

	if y == 0:
		return None

	payda = get_inverse(2 * y, field_size)
	x3 = ( 3 * x * x  + a ) * payda
	x3 *= x3
	x3 = (x3 - 2 * x ) % field_size

	y3 = (3 * x * x + a ) * payda * (x - x3) - y  
	y3 = y3 % field_size

	return x3,y3

def get_sum(p1, p2, field_size, a , b):
	x1, y1, x2, y2 = p1[0], p1[1], p2[0], p2[1]

	if x1 == x2:
		return None 

	pay = (y2 - y1) % field_size
	payda = (x2 - x1) % field_size

	payda = get_inverse(payda, field_size)

	if payda is None:
		return None 

	x3 = pay * payda 
	x3 = (x3 * x3 - x1 - x2) % field_size

	y3 = (pay * payda * ( x1 - x3 ) - y1) % field_size

	return x3, y3


def order(p, field_size, a, b):
	
	cnt = 1
	var = get_double(p, field_size, a, b)

	if var is None:
		return cnt  

	cnt += 1  

	while True:
		#print(f'var: {var[0]},{var[1]}')
		var = get_sum(p, var, field_size, a , b)

		if var is None:
			break

		cnt += 1

	cnt += 1 
	return cnt 



def find_generator(all_points, field_size, a, b):
	for p in all_points:
		if p[0] == float("inf"):
			continue
		if order(p, field_size, a, b) == len(all_points):
			print(f'({p[0]},{p[1]}) is a generator')
		print('------------')

def main():
	field_size = 13
	a = 1
	b = 2
	all_points = get_all_points(field_size, a, b)

	print(all_points)

	print(f'# Of Points :{len(all_points)}')

	find_generator(all_points, field_size, a, b)


if __name__ == '__main__':
	main()