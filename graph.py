import matplotlib.pyplot as plt
file = open('output.txt', 'r')
times = []
for val in file:
    times.append(float(val))
plt.plot([i for i in range(1, len(times) + 1)], times)
plt.show()
