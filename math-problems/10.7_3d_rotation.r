library(pracma)

# Problem 10.6
# Two rotations of 3-vector x.
# x --rot1--> z --rot2--> y

# Question 1: Find the 3x3 matrix, A to satisfy y = Ax
# Question 2: Verify that A is orthogonal

.rotMatBasic = function(theta) {
  # function takes theta in radians
  # returns a 2d rotation matrix for the angle theta
  res = matrix(data=c(cos(theta),sin(theta),-sin(theta),cos(theta)),ncol=2)
  return(res)
}

rotMat = function(theta,en) {
  theta=deg2rad(theta)
  res = .rotMatBasic(theta)
  if (en == 3) {
    res = rbind(res,c(0,0))
    res = cbind(res,c(0,0,1))
    return(res)
  }
  if (en == 1) {
    res = rbind(c(0,0),res)
    res = cbind(c(1,0,0),res)
    return(res)
  }
  if (en == 2) {
    warning("code for case en == 2 not written yet")
    return(-1)
  }
}

# first rotation is 40 degrees about the e3 axis
U = rotMat(40,3)
# second rotation is 20 degrees about the e1 axis
V = rotMat(20,1)

# Answer 1: 
# We have y == V(Ux)
# Thus A = VU
A = V %*% U

# Answer 2:
# Compute t(A) %*% A and show that it is I or close to I
print( t(A) %*% A )
print( t(U) %*% U )
print( t(V) %*% V )

# Thus A, U, and V are are all orthogonal matrices
