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
    ext = .Call("r_so_SO_read", name)
    so = so_SO$new(cobj=ext)
    return(so)
}

so_SO_write <- function(self, filename, pretty) {
    .Call("r_so_SO_write", self, filename, pretty)
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

so_SO$methods(
    write = function(filename, pretty=TRUE) {
        fail = so_SO_write(.self$.cobj, filename, ifelse(pretty, 1L, 0L))
        if (fail != 0) {
            stop(paste0("Could not write to file ", filename))
        }
    }
)
