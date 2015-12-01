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

so_Table_free <- function(self) {
    .Call("r_so_Table_free", self)
}

so_Matrix_free <- function(self) {
    .Call("r_so_Matrix_free", self)
}

so_SO$methods(
    write = function(filename, pretty=TRUE) {
        invisible(so_SO_write(.self$.cobj, filename, ifelse(pretty, 1L, 0L)))
    }
)
