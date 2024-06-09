import numpy as np

# sample data
x = np.array([66.0, 71.5, 75.4, 80.7, 81.0, 75.8, 70.8])
y = np.array([70.0, 75.0, 79.9, 85.0, 85.2, 80.0, 75.0])

n = len(x)
x_mean = np.mean(x)
y_mean = np.mean(y)
xy_mean = np.mean(x * y)
xx_mean = np.mean(x * x)

# least square method
a = (xy_mean - x_mean * y_mean) / (xx_mean - x_mean * x_mean)
b = y_mean - a * x_mean

x_real = 77.8

print(f"a = {a}")
print(f"b = {b}")

y_real = a * x_real + b

print(f"y_real = {y_real}") 
