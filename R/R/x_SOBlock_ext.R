so_SOBlock_add_message <- function(type, toolname, name, content, severity) {
    if (isnull(.self$.cobj)) {
        stop(".cobj is NULL")
    }
    if (!is(type, "character")) {
        stop("type must be a character string")
    }
    if (!is(toolname, "character")) {
        stop("toolname must be a character string")
    }
    if (!is(name, "character")) {
        stop("name must be a character string")
    }
    if (!is(content, "character")) {
        stop("content must be a character string")
    }
    if (is(severity, "numeric")) {
        severity = as.integer(severity)
    }
    if (!is(severity, "integer")) {
        stop("severity must be integer or numeric")
    }
    invisible(.Call("R_so_SOBlock_add_message", .self$.cobj, type, toolname, name, content, severity))
}

so_SOBlock$methods(add_message = so_SOBlock_add_message)

so_SOBlock_add_rawresults_datafile <- function(description, path, oid) {
    if (isnull(.self$.cobj)) {
        stop(".cobj is NULL")
    }
    if (!is(description, "character")) {
        stop("type must be a character string")
    }
    if (!is(path, "character")) {
        stop("toolname must be a character string")
    }
    if (!is(oid, "character")) {
        stop("name must be a character string")
    }
    invisible(.Call("R_so_SOBlock_add_rawresults_datafile", .self$.cobj, description, path, oid))
}

so_SOBlock$methods(add_rawresults_datafile = so_SOBlock_add_rawresults_datafile)

so_SOBlock_add_rawresults_graphicsfile <- function(description, path, oid) {
    if (isnull(.self$.cobj)) {
        stop(".cobj is NULL")
    }
    if (!is(description, "character")) {
        stop("type must be a character string")
    }
    if (!is(path, "character")) {
        stop("toolname must be a character string")
    }
    if (!is(oid, "character")) {
        stop("name must be a character string")
    }
    invisible(.Call("R_so_SOBlock_add_rawresults_graphicsfile", .self$.cobj, description, path, oid))
}

so_SOBlock$methods(add_rawresults_graphicsfile = so_SOBlock_add_rawresults_graphicsfile)
