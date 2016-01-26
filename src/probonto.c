/* libsoc - Library to handle standardised output files
 * Copyright (C) 2015 Rikard Nordgren
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * his library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <pharmml/probonto.h>

#define NUMDISTRONAMETYPES 86 
static char *distro_name_types[] = {
    "", "Bernoulli1", "BetaBinomial1", "Binomial1", "CategoricalOrdered1", "CategoricalNonordered1", "ConwayMaxwellPoisson1", "DoublePoisson1",
    "GeneralizedNegativeBinomial1", "GeneralizedPoisson1", "GeneralizedPoisson2", "GeneralizedPoisson3", "Geometric1", "Hypergeometric1",
    "InverseBinomial1", "NegativeBinomial1", "NegativeBinomial2", "NegativeBinomial3", "NegativeBinomial4", "NegativeBinomial5",
    "Poisson1", "UniformDiscrete1", "UniformDiscrete2", "ZeroInflatedNegativeBinomial1", "ZeroInflatedGeneralizedPoisson1", "ZeroInflatedPoisson1",
    "Beta1", "BirnbaumSaunders1", "Cauchy1", "ChiSquared1", "Erlang1", "Exponential1", "Exponential2", "F1", "F2", "Gamma1", "Gamma2",
    "GeneralizedGamma1", "GeneralizedGamma2", "GeneralizedGamma3", "Gompertz1", "Gumbel1", "HalfNormal1", "InverseGamma1", "InverseGaussian1",
    "Laplace1", "Laplace2", "Logistic1", "Logistic2", "LogLogistic1", "LogLogistic2", "LogNormal1", "LogNormal2", "LogNormal3", "LogNormal4",
    "LogNormal5", "LogNormal6", "LogUniform1", "Nakagami1", "Normal1", "Normal2", "Normal3", "NormalInverseGamma1", "Pareto1", "Rayleigh1",
    "StandardNormal1", "StandardUniform1", "StudentT1", "StudentT2", "Triangular1", "TruncatedNormal1", "Uniform1", "Weibull1", "Weibull2",
    "Multinomial1", "Dirichlet1", "InverseWishart1", "MultivariateNormal1", "MultivariateNormal2", "MultivariateStudentT1", "MultivariateStudentT2",
    "Wishart1", "Wishart2", "MixtureDistribution", "RandomSample", "StatisticalSample", "UnknownSample"
};

const char *pharmml_DistroNameType_to_string(pharmml_DistroNameType value)
{
    if (value < NUMDISTRONAMETYPES) {
        return distro_name_types[value];
    } else {
        return NULL;
    }
}

pharmml_DistroNameType pharmml_string_to_DistroNameType(const char *str)
{
    for (int i = 0; i < NUMDISTRONAMETYPES; i++) {
        if (strcmp(str, distro_name_types[i]) == 0) {
            return i;
        }
    }

    return 0;
}
