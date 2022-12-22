library(matrixcalc)
library(pracma)

measure_time = function(f) {
  if (!is.function(f)) {
    warning("argument should be a fxn!")
    return(vector(mode = "integer"))
  }
  start.time = Sys.time()
  f()
  end.time = Sys.time()
  time.taken = end.time - start.time
  time.taken
}

# prepare for matrix exponential

# timing results matrix
results = matrix(data = 0, ncol = 4, nrow = 10)

for (core_idx in 1:7) {
  Ncores = core_idx * 2
  library(doParallel)
  registerDoParallel(cores = Ncores)
  library(foreach)
  I = eye(4)
  res = -1
  
  A = matrix(data=rnorm(16),ncol=4)
  krange = rep(1e4, 1e4)
  fn_par = function() {
    foreach(k = krange) %dopar% {
      res = matrix.power(A + I * (1 / k), k)
    }
  }
  
  fn_seq = function() {
    foreach(k = krange) %do% {
      res = matrix.power(A + I * (1 / k), k)
    }
  }
  colnames(results) = c("cores", "Seq", "Parallel", "Factor Increase")
  if (core_idx == 1) {
    time1 = measure_time(fn_seq) # repeating seq trial each time not necessary
  }
  results[core_idx, 2] = time1
  time2 = measure_time(fn_par)
  results[core_idx, 3] = time2
  results[core_idx, 1] = Ncores
  results[core_idx, 4] = as.double(time1) / as.double(time2)
}

df = data.frame(results)
library(ggplot2)
colors <- c("Seq" = "blue", "Parallel" = "red")

p = ggplot(df[1:7, ], aes(x = cores)) +
  geom_line(aes(y = Seq , color = "Seq")) +
  geom_line(aes(y = Parallel , color = "Parallel")) +
  labs(x = "number of cores", y = "time in seconds", color = "Legend") +
  scale_color_manual(values = colors) +
  scale_x_continuous(breaks = seq(2,16,2)) +
  ggtitle("matrix.power(4x4 matrix, 1e4) done 10,000 times")
print(p)

# Results show that the timing improves as we move from 1 to 8 cores because
# there are 8 true cores on the machine. After that we are using multiple
# threads on each core and the timing gets worse.
