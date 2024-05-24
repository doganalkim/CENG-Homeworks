import numpy as np
import matplotlib.pyplot as plt

N = 10**3
def x(n): # defining x[n]
    if n < 0:
        return (1/2)**(-n)
    return 1/2**(n)

w_axis = np.linspace(-20, +20, 600)
def X(w): # defining x[e^(jw)]
    return np.sum([x(n) * np.exp(1j * w * -n) for n in range(-N,N+1)])

result = [X(w) for w in w_axis]

magnitudes = []
phases = []

for i in result:
    re = i.real
    im = i.imag
    magnitude = np.sqrt(re**2 + im**2)
    magnitudes.append(magnitude)
    # print(f"i:{i} - real:{re} - imaginary:{im} - magnitude is:{magnitude}") // debugging
    if re == 0:
        if im == 0:
            phases.append(0)
        elif im > 0:
            phases.append(np.pi / 2)
        else:
            phases.append( -np.pi / 2)
    else:
        phase = np.arctan(im/re)
        phases.append(round(phase,3))

fig = plt.figure(figsize=(12, 5))

w1 = w_axis
w2 = w_axis

plt.subplot(1, 2, 1)
plt.plot(w1, magnitudes, color='r',  label='Magnitude')
plt.xlabel('w-axis')
plt.ylabel('X(e^(jw))-axis')
plt.title('Magnitude')
plt.grid(True)
plt.legend()

plt.subplot(1, 2, 2)
plt.plot(w2, phases, color='b', linestyle='-',  label='Phase')
plt.xlabel('w-axis')
plt.ylabel('X(e^(jw))-axis')
plt.title('Phase')
plt.grid(True)
plt.legend()

plt.tight_layout()
plt.show()