import numpy as np
import matplotlib.pyplot as plt

def x(t):
    return (np.cos(np.pi / 3 * t) + 2*np.cos(np.pi * t + np.pi/2))

t = np.linspace(0, 6, 6000)

def a(k):
    return 1/6 * np.trapz(x(t)* np.exp( -1j * k * np.pi / 3 * t) ,t)

x_axis = [i for i in range(-20,21)]
y_axis = []



for i in x_axis:
    y_axis.append(a(i))

y_axis_mag = []
y_axis_phase = []

for i in y_axis:
    mag = np.sqrt(i.real**2 + i.imag**2)
    if round(mag,3) == 0:
        y_axis_phase.append(0)
    else:
        phas = np.arctan( i.imag / i.real  )
        y_axis_phase.append(phas / np.pi)
    y_axis_mag.append(mag)


print("The period is 6, w_0 is pi/3")
print("The Fourier series representation is -j * e ^{-j*pi*t} + 0.5 * e ^{-j*pi*t / 3} + 0.5 *e ^{j*pi*t/3} + j * e^{j*pi*t}")
# -3 -> -j, -1 -> 0.5 , 1 -> 0.5, 3 -> j

x1 =  x_axis
y1 = y_axis_mag


x2 = x_axis
y2 = y_axis_phase

fig = plt.figure(figsize=(12, 5))


plt.subplot(1, 2, 1) 
plt.plot(x1, y1, marker='o', color='r', linestyle='',  label='Plot 1') 
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.title('Magnitude ')
plt.grid(True)
plt.legend()


plt.subplot(1, 2, 2)
plt.plot(x2, y2, marker='s', color='b', linestyle='',  label='Plot 2') 
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.title('Phase')
plt.grid(True)
plt.legend()

plt.tight_layout() 
plt.show()
