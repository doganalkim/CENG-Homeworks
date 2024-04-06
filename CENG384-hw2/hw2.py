import matplotlib.pyplot


def x(n):
    return 1 if n == 1 else 0

def y(n):
    if n < 0:
        return 0
    return 1/4*y(n-1) + x(n)


x_axis = [0,1,2,3,4]

y_axis = []

for i in x_axis:
    y_axis.append(y(i))

matplotlib.pyplot.stem(x_axis,y_axis)
matplotlib.pyplot.grid(True)
matplotlib.pyplot.xticks(range(0,6))
matplotlib.pyplot.yticks([0,0.25,0.5,0.75,1,1.25])
matplotlib.pyplot.show()