from sympy import *

n = symbols('n', integer=True)
x_1 = IndexedBase('x_1')
x_2 = IndexedBase('x_2')
a = symbols('a')
b = symbols('b')

def partA():
    def H(x):
        return n*x

    y_1 = H(x_1[n])
    y_2 = H(x_2[n])

    leftside = a*y_1 + b*y_2
    rightside = H(a*x_1[n] + b*x_2[n])

    print("Leftside is: ", leftside)
    print("Rightside is: ", rightside)

    if(leftside.equals(rightside)):
        print("The given system is a Linear system")
    else:
        print("The given system is a Non-Linear system")

def partB():
    def H(x):
        return x**2

    y_1 = H(x_1[n])
    y_2 = H(x_2[n])

    leftside = a * y_1 + b * y_2
    rightside = H(a * x_1[n] + b * x_2[n])

    print("Leftside is: ", leftside)
    print("Rightside is: ", rightside)

    if (leftside.equals(rightside)):
        print("The given system is a Linear system")
    else:
        print("The given system is a Non-Linear system")


print("------------ PART A -------------")
partA()
print("------------ PART B -------------")
partB()