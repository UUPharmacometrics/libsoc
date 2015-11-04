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

#SOBlock_acc <- function(value)
#{
#	if (!isnull(.self$.cobj)) {
#		if (missing(value)) {
#			n = so_SO_get_number_of_SOBlock(.self$.cobj)
#			if (n > 0) {
#				a = list()
#				for (i in seq.int(1, n)) {
#					child = so_SO_get_SOBlock(.self$.cobj, i - 1L)
#					a[i] = so_SOBlock$new(cobj=child)
#				}
#				return(a)
#			}
#		} else {
#		}
#	}
#}

# Double from gen-SO.R
so_SO = setRefClass("so_SO",
	fields=list(
		SOBlock = SOBlock_acc,
		.cobj = "externalptr"
	),
	methods=list(
		initialize = function(cobj) {
			if (missing(cobj)) {
				.cobj <<- so_SO_new()
			} else {
				.cobj <<- cobj
			}
		},
		finalize = function() {
		    so_SO_unref(.self$.cobj)
		},
        write = function(filename, pretty=1L) {
          invisible(so_SO_write(.self$.cobj, filename, pretty))
        },
        add_SOBlock = function(value) {
            invisible(so_SO_add_SOBlock(.self$.cobj, value$.cobj))
        }
	)
)
