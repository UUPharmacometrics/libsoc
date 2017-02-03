\name{libsoc}
\alias{libsoc}
\alias{libsoc-package}
\docType{package}
\title{
A package to handle PharmML standardized output (SO) XML files
}
\description{
A package to handle PharmML standardized output (SO) XML files.
SO files can be created, read, manipulated and written through a
data binding from the XML structure to a tree structure of R objects.
}
\details{
\tabular{ll}{
Package: \tab libsoc\cr
Type: \tab Package\cr
Version: \tab 0.5\cr
Date: \tab 2016-09-20\cr
License: \tab LGPL-3\cr
}
}
\author{
Rikard Nordgren

Maintainer: Rikard Nordgren <rikard.nordgren@farmbio.uu.se>
}
\keyword{package}
\examples{
# Read in parameter estimates
file <- system.file("extdata", "pheno.SO.xml",  package="libsoc")
so <- so_SO_read(file)
estimates <- so$SOBlock[[1]]$Estimation$PopulationEstimates$MLE

# For more examples see the examples directory
}
