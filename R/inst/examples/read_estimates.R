#!/usr/bin/Rscript

# This example reads in and presents some results from an estimation

library(methods)
library(libsoc)

file <- system.file("extdata", "pheno.SO.xml",  package="libsoc")
so <- so_SO_read(file)
estimates <- so$SOBlock[[1]]$Estimation$PopulationEstimates$MLE
cat("\n")
cat("Parameter Estimates:\n")
print(estimates)
cat("\n")

cat("Covariance Matrix:\n")
cov_matrix <- so$SOBlock[[1]]$Estimation$PrecisionPopulationEstimates$MLE$CovarianceMatrix
print(cov_matrix)
cat("\n")

cat("Standard error:\n")
se <- so$SOBlock[[1]]$Estimation$PrecisionPopulationEstimates$MLE$StandardError
print(se)
cat("\n")

cat("Relative standard error:\n")
rse <- so$SOBlock[[1]]$Estimation$PrecisionPopulationEstimates$MLE$StandardError
print(rse)
cat("\n")

cat("OFV:\n")
ofv <- so$SOBlock[[1]]$Estimation$OFMeasures$Deviance
print(ofv)
cat("\n")
