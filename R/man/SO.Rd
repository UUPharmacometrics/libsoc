\name{so_SO}
\alias{so_SO}
\title{so_SO reference class}
\description{
	Reference Class for the SO element of a PharmML-SO data structure
}
\section{Methods}{
so_SO$new() - Create a new empty so_SO object\cr
so_SO$write(filename, pretty=TRUE) - Write an SO to file. Set pretty to FALSE to not get pretty printed xml\cr
so_SO$add_SOBlock(object) - Add a SOBlock\cr
so_SO$remove_SOBlock(object, i) - Remove the SOBlock having index i\cr
so_SO$all_population_estimates() - Get a data.frame with the population estimates from all SOBlocks\cr
so_SO$all_standard_errors() - Get a data.frame with the standard errors from all SOBlocks\cr
so_SO$variability_type(parameter_names) - Given an array of parameter names return an array with the variability type of the parameters\cr
    Types are: structParameter, parameterVariability and residualError\cr
so_SO$correlation_parameters(parameter_names) - Given an array of parameter names return an array of whether each parameter is a correlation or not\cr
so_SO$random_variable_from_variability_parameter(parmeter_names) - Given an array of parameter names return an array of names of the corresponding random variable\cr
}
\section{Fields}{
$Description - A character string\cr
$PharmMLRef - A \link{so_PharmMLRef} object\cr
$SOBlock - A list of \link{so_SOBlock} objects\cr
$id - A character string attribute\cr
$metadataFile - A character string attribute\cr
}
\keyword{so_SO}
