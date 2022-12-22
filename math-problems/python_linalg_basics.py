import numpy as np
import numpy.linalg as lin

a = np.random.randint(1,4,size=9).reshape((3,3)) # convert 9-vector into 3x3 matrix
a = np.reshape(a,(3,3)) 
b = np.random.choice(np.random.binomial(n=4,p=.2,size=20),replace=False,size=9).reshape((3,3))
prod = b @ a  # matrix multiplication with @ symbol
prod2 = b * a # element wise multiplication with * symbol
prod3 = np.abs(prod2 - prod)
soln = np.ndarray(shape=(3,3),dtype=float)
if lin.det(prod3) != 0.0:
    soln = lin.inv(prod3)
    print(prod3)
    print(soln)
    print(prod3@soln)

from numpy.matlib import *
print("from numpy.matlib import *")
print("eye(3,3)", eye(3,3),sep="\n")
print("zeros(shape=(2,4))",zeros(shape=(2,4)),sep="\n")
print("randn(3,3)", randn(3,3),sep="\n")
