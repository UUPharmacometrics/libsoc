# libsoc - Library to handle standardised output files
# Copyright (C) 2015 Rikard Nordgren
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
# 
# his library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.

.self <- NULL       # Work around to silent notes for R CMD check

isnull <- function(pointer) {
    stopifnot(is(pointer, "externalptr"))
    .Call("isnull", pointer)
}

so_SO_read <- function(name) {
    ext = .Call("r_so_SO_read", path.expand(name))
    so = so_SO$new(cobj=ext)
    return(so)
}

so_SO_write <- function(self, filename, pretty) {
    .Call("r_so_SO_write", self, filename, pretty)
}

so_SO_all_population_estimates <- function(self) {
    .Call("r_so_SO_all_population_estimates", self)
}

so_SO_all_standard_errors <- function(self) {
    .Call("r_so_SO_all_standard_errors", self)
}

so_SO_is_structural_parameter <- function(self, name) {
    .Call("r_so_SO_is_structural_parameter", self, name)
}

so_SO_is_ruv_parameter <- function(self, name) {
    .Call("r_so_SO_is_ruv_parameter", self, name)
}

so_SO_random_variable_from_variability_parameter <- function(self, name) {
    .Call("r_so_SO_random_variable_from_variability_parameter", self, name)
}

so_SO_is_correlation_parameter <- function(self, name) {
    .Call("r_so_SO_is_correlation_parameter", self, name)
}

so_Table_ref <- function(self) {
    .Call("r_so_Table_ref", self)
}

so_Table_free <- function(self) {
    .Call("r_so_Table_free", self)
}

so_Matrix_free <- function(self) {
    .Call("r_so_Matrix_free", self)
}

id_column <- function(table) {
    colTypes <- attributes(table)$columnType
    if (is.null(colTypes)) {
        stop("Could not find columnTypes attribute. Did this data.frame not come from an SO object?")
    }
    for (i in 1:length(colTypes)) {
        if ("id" %in% colTypes[[i]]) {
            return(i)
        }
    }
    stop("Could not find ID column")
}

id_column_name <- function(table) {
    names(table)[id_column(table)]
}

idv_column <- function(table) {
    colTypes <- attributes(table)$columnType
    if (is.null(colTypes)) {
        stop("Could not find columnTypes attribute. Did this data.frame not come from an SO object?")
    }
    for (i in 1:length(colTypes)) {
        if ("idv" %in% colTypes[[i]]) {
            return(i)
        }
    }
    stop("Could not find idv column")
}

idv_column_name <- function(table) {
    names(table)[idv_column(table)]
}

dv_column <- function(table) {
    colTypes <- attributes(table)$columnType
    if (is.null(colTypes)) {
        stop("Could not find columnTypes attribute. Did this data.frame not come from an SO object?")
    }
    for (i in 1:length(colTypes)) {
        if ("dv" %in% colTypes[[i]]) {
            return(i)
        }
    }
    stop("Could not find dv column")
}

dv_column_name <- function(table) {
    names(table)[dv_column(table)]
}

variability_func <- function(symbol, self) {
    struct <- so_SO_is_structural_parameter(self, symbol)
    if (struct == 0) {
        "structParameter"
    } else if (struct == -1) {
        "unknown"
    } else if (so_SO_is_ruv_parameter(self, symbol) == 0) {
        "residualError"
    } else {
        "parameterVariability"
    }
}

random_variable_func <- function(symbol, self) {
    so_SO_random_variable_from_variability_parameter(self, symbol)
}

correlation_func <- function(symbol, self) {
    corr <- so_SO_is_correlation_parameter(self, symbol)
    if (corr == 0) {
        TRUE
    } else if (corr == 1) {
        FALSE
    } else {
        NA
    }
}

so_SO$methods(list(
    write = function(filename, pretty=TRUE) {
        fail = so_SO_write(.self$.cobj, filename, ifelse(pretty, 1L, 0L))
        if (fail != 0) {
            stop(paste0("Could not write to file ", filename))
        }
    },
    all_population_estimates = function() {
        so_SO_all_population_estimates(.self$.cobj)
    },
    all_standard_errors = function() {
        so_SO_all_standard_errors(.self$.cobj)
    },
    variability_type = function(symbols) {
        sapply(symbols, variability_func, .self$.cobj)
    },
    correlation_parameters = function(symbols) {
        sapply(symbols, correlation_func, .self$.cobj)
    },
    random_variable_from_variability_parameter = function(symbols) {
        sapply(symbols, random_variable_func, .self$.cobj)
    }

    )
)
