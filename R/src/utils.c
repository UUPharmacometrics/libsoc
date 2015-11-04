#include <Rinternals.h>

SEXP isnull(SEXP pointer) {
    return ScalarLogical(!R_ExternalPtrAddr(pointer));
}
