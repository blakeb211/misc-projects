import numpy as np
import pandas as pd
import numpy.linalg as lin
from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt

##################################################
# Parametric equation of a line through a point.
##################################################

# Find the parametric equation of a line L in R^4 passing
# through the point P(1; 2; 3; 4) and in the direction
# of u = (5; 6; -7; 8).
# Also, find the point Q on L when t = 1.

p = np.array([1,2,3,-4])
u = np.array([5,6,-7,8])

L = lambda p,u,t: p + t*u

print("Q = ", L(p,u,1))

##################################################
# Derivative of 3d curve in R^n is a tangent vector
##################################################

F = lambda t : (np.sin(t), np.cos(t), t)
V = lambda t : (np.cos(t), -np.sin(t), 1)
lenV = np.sqrt(2) # cos^2(t) + sin^2(t) + 1

tangent_unit_vec = lambda t: V(t)/lenV

fig = plt.figure()
ax = plt.axes(projection='3d')

fun3 = list(map(F, np.arange(0.0,6.28,0.1)))
from swampy import structshape as ss
print("type fun3:", ss.structshape(fun3))
xline = [i[0] for i in fun3]
yline = [i[1] for i in fun3]
zline = [i[2] for i in fun3]
print("type xline:", ss.structshape(xline))
ax.plot3D(xline, yline, zline, 'gray')

tan_vec3 = list(map(tangent_unit_vec, np.arange(0.0,6.28,0.1)))

from swampy import structshape as ss
print("type fun3:", ss.structshape(fun3))
xline = [i[0] for i in tan_vec3]
yline = [i[1] for i in tan_vec3]
zline = [i[2] for i in tan_vec3]
print("type xline:", ss.structshape(xline))
ax.plot3D(xline, yline, zline, 'blue')
plt.show()
