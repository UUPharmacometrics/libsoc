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
\author{
Rikard Nordgren

Maintainer: Rikard Nordgren <rikard.nordgren@farmbio.uu.se>
}
\details{
\tabular{ll}{
Package: \tab libsoc\cr
Type: \tab Package\cr
Version: \tab 0.6.2\cr
Date: \tab 2018-01-12\cr
License: \tab LGPL-3\cr
}
Depends on libxml2, libiconv and zlib. More information and source code for these libraries can be found on the sites linked below.
\tabular{ll}{
libxml2 \tab http://xmlsoft.org \cr
libiconv \tab https://www.gnu.org/software/libiconv \cr
zlib \tab http://www.zlib.net \cr
}
You can view the license for libsoc itself and libiconv with 
\code{file.show(system.file("licenses", "COPYING-LIB", package="libsoc"))}
and libxml2 with \code{file.show(system.file("licenses", "COPYING-LIBXML2", package="libsoc"))}
}
\keyword{package}
\examples{
# Read in parameter estimates
file <- system.file("extdata", "pheno.SO.xml",  package="libsoc")
so <- so_SO_read(file)
estimates <- so$SOBlock[[1]]$Estimation$PopulationEstimates$MLE

# For more examples see the examples directory
}
