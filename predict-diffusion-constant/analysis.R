# get dataframe into desired form
# (https://www.engineeringtoolbox.com/air-diffusion-coefficient-gas-mixture-temperature-d_2010.html)
library(magrittr)

dat <- t(read.table("diffusion.dat"))
cnames <-
  dat["Species",] # save chemical formulas to be used as column names
dat <- matrix(as.double(dat), ncol = 8)

# clean data
dat <- dat[,-8]  # remove SF6
cnames <- cnames[-8] # remove SF6 from cnames
dat <- dat[-1,]   # remove first row (chemical names)
dat <- dat[-1,]  # remove first row (0 deg C data)

# swap co2 and h2o columns in dat and cnames
dat <- dat[, c(1, 2, 3, 6, 5, 4, 7)]
cnames = cnames[c(1, 2, 3, 6, 5, 4, 7)]

# label dataframe
temp <-
  c(20, 100, 200, 300, 400) # create vector to hold temp values
colnames(dat) <- cnames
rownames(dat) <- temp

# Create Feature Matrix
# Features: MM (molar mass), Vol (molecular volume), Dipole (dipole moment)

# fdat is the feature matrix for the 7 molecules
fdat <- matrix(ncol = 3, nrow = 7)
fdat <- data.frame(fdat, row.names = cnames)
colnames(fdat) <- c("MM", "Vol", "Dipole")

# add molar mass (g/mol)
fdat["MM"] <- c(39.95, 16.04, 28.01, 18.02, 2.02, 44.02, 4.00)

# van der waals b coefficients (L/mol)
# (https://chem.libretexts.org/Ancillary_Materials/Reference/Reference_Tables/Atomic_and_Molecular_Properties/A8%3A_van_der_Waal%27s_Constants_for_Real_Gases)
fdat["Vol"] <- c(0.03201, 0.04301, 0.03948, 0.03049, 0.02651, 0.04286, 0.0238)

# add dipole moment (Debye) (https://cccbdb.nist.gov/diplistx.asp)
fdat["Dipole"] <- c(0, 0, 0.112, 1.855, 0, 0, 0)

# transform MM feature to be 1 / sqrt(MM)
fdat["MM"] <- 1 / sqrt(fdat["MM"])
fdat["Vol"] <- 1 / fdat["Vol"]^(1/3)

# check rms of each feature; consider altering to make rms similar size
# regularize rmses
rms <- function(x)
  sqrt(sum(x * x) / length(x))

# print out rms of each feature
 for (i in 1:3) {
   res <- rms(fdat[, i])
   print(paste0(colnames(fdat)[i], " = ", res))
 }

# regression model yhat = t(x) %*% beta + nu
# x is feature vector, beta is coefficients

ls1 <-
  lsfit(
    fdat[1:5, ],
    t(dat[, 1:5]),
    wt = NULL,
    intercept = TRUE,
    tolerance = 1e-07
  )

# check model result on co2 and He
fco2 <- as.numeric(cbind(1, fdat["CO2", ]))
fhe <- as.numeric(cbind(1, fdat["He", ]))

model1 <- t(ls1$coefficients)
rownames(model1) <- temp

pred_co2 <- model1 %*% fco2
pred_he <- model1 %*% fhe
actual_co2 <- dat[, "CO2"]
actual_he <- dat[, "He"]

library("pracma")

# plot diffusion constant versus temp for the two species we'll predict, CO2 and He

plot_data <- data.frame(cbind(temp,actual_co2,pred_co2,actual_he,pred_he))
colnames(plot_data) <- c("Temp", "CO2 actual", "CO2 predicted", "He actual", "He predicted")

library("ggplot2")
colors <- c("CO2 actual" = "darkblue", "CO2 predicted" = "steelblue", 
            "He actual" = "darkred", 
            "He predicted" = "lightcoral")
ggplot(data=plot_data) + aes(x=Temp) + 
  geom_point(aes(y=`CO2 actual`,color ="CO2 actual")) + 
  geom_line(aes(y=`CO2 actual`,color="CO2 actual")) +
  geom_point(aes(y=`CO2 predicted`,color ="CO2 predicted")) + 
  geom_line(aes(y=`CO2 predicted`,color="CO2 predicted"),linetype="twodash") +
  geom_point(aes(y=`He actual`,color ="He actual")) + 
  geom_line(aes(y=`He actual`,color="He actual")) +
  geom_point(aes(y=`He predicted`,color ="He predicted")) + 
  geom_line(aes(y=`He predicted`,color="He predicted"),linetype="twodash") + 
  labs(x="Temp (C)", y=bquote('Diffusivity'~(cm^2 / s)), color="Legend") +
  scale_color_manual(values=colors) +
  theme(legend.position = c(0.2, 0.75)) +
  ggtitle("Predicting Diffusion Constants with a Linear Model") + 
  theme(plot.title = element_text(hjust = 0.5,size=10)) +
  theme(axis.title = element_text(size=10)) +
  theme(legend.text = element_text(size=8)) +
  theme(legend.title = element_text(size=9))
  
  cat("\nExperimental Diffusion Constants at Different Temps:\n")
  print(dat)
  
  cat("\nFeature Matrix:\n")
  print(fdat)

  cat("\nLinear Model:\n")
  print(model1)
  
  cat("\nThe (+/-) signs of the model coefficients show that diffusion constants\n
        are positively correlated with 1/sqrt(Molar Mass) and negatively\n
        correlated with Molecular Volume and Dipole Moment. This makes sense\n
        physically because larger molecules should bump into other molecules\n
        more and molecules with a dipole moment should interact more strongly with\n
        molecules around them which would slow them down.\n")
  
  # calculate diffusion limit of gases in air at these temperatures
  # diffusion limit is obtained from plugging Molar Mass = 1, Volume = 0, 
  # and Dipole Moment = 1 into the model.
  
  predDiffusionLimit = model1 %*% c(1, 1/sqrt(1), 0, 0)
  
  cat("\nThe predicted diffusion limit for a gas at each temperature:\n")
  colnames(predDiffusionLimit) <- "cm^2 / 2"
  print(predDiffusionLimit)
  
  ggsave(filename="model_predictions.jpg", height=4.3, width=4,units="in", dpi=200)