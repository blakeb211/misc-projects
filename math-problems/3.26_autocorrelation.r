stdev <- function(x) {
  mu <- mean(x)
  res <- ((x - mu) %*% (x - mu)) / length(x)
  sqrt(res)
}

x <- rep(c(1, -1), 4)
#a <- seq(0,4*pi,length=100)
#x <- sin(a)
print(x)

if (length(x) < 2 || length(x) %% 2 != 0 || stdev(x) == 0) {
  print("error: for n-vector x, need n > 1 and even, with stdev != 0")
  q()
}

mu <- mean(x)

# z-scoring - z is the standardized version of x
z <- c(1 / stdev(x))*(x-c(mu))
N <- length(z)

ac <- function(z, tau) {
  sum <- 0
  for(i in seq(1,N-tau)) {
    sum <- sum + z[i]*z[i+tau]
  }
  sum / N
  # my way of normalizing it so that the peaks don't diminish with higher tau values.
  # sum / (N-tau) 
}

ac_results <- c(rep(0,N))

for(i in seq(0,N-1)) ac_results[i+1] <- ac(z,i)

plot(seq(0,N-1),ac_results,xlab = "tau", ylab ="autocorrelation")

mat <- matrix(data = c(seq(0,N-1),ac_results),nrow=8,ncol=2,byrow=FALSE)
colnames(mat) <- c("tau","acf")

print(mat)


