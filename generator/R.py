#!/usr/bin/python3

# Generate R bindings to libsoc
import os
import sys
import common
from structure import structure

def print_new(name):
    print("SEXP r_so_", name, "_new()", sep='')
    print("{")
    print("\tso_", name, " *obj = so_", name, "_new();", sep='')
    print("\treturn R_MakeExternalPtr(obj, R_NilValue, R_NilValue);")
    print("}")

def print_free(name):
    print("SEXP r_so_", name, "_free(SEXP self)", sep='')
    print("{")
    print("\tso_", name, "_free(R_ExternalPtrAddr(self));", sep='')
    print("\treturn R_NilValue;")
    print("}")

def print_ref_unref(name):
    print("SEXP r_so_", name, "_ref(SEXP self)", sep='')
    print("{")
    print("\tso_", name, "_ref(R_ExternalPtrAddr(self));", sep='')
    print("\treturn R_NilValue;")
    print("}")
    print()
    print("SEXP r_so_", name, "_unref(SEXP self)", sep='')
    print("{")
    print("\tso_", name, "_unref(R_ExternalPtrAddr(self));", sep='')
    print("\treturn R_NilValue;")
    print("}")

def print_attribute_getter(name, attr):
    print("SEXP ", common.create_get_name(name, attr, prefix="r_so"), "(SEXP self)", sep='')
    print("{")
    print("\tchar *string = ", common.create_get_name(name, attr, prefix="so"), "(R_ExternalPtrAddr(self));", sep='')
    print()
    print("\tSEXP r_string;")
    print()
    print("\tif (string) {")
    print("\t\tr_string = PROTECT(NEW_STRING(1));")
    print("\t\tSET_STRING_ELT(r_string, 0, mkChar(string));")
    print()
    print("\t\tUNPROTECT(1);")
    print("\t} else {")
    print("\t\tr_string = R_NilValue;")
    print("\t}")
    print("\treturn r_string;")
    print("}")

def print_attribute_setter(name, attr):
    print("SEXP " + common.create_set_name(name, attr, prefix="r_so") + "(SEXP self, SEXP string)", sep='')
    print("{")
    print("\tchar *c_string = (char *) CHAR(STRING_ELT(string, 0));")
    print("\t", common.create_set_name(name, attr, prefix="so"), "(R_ExternalPtrAddr(self), c_string);", sep="")
    print()
    print("\treturn R_NilValue;")
    print("}")

def print_get_number_of(name, child, cls):
    print("SEXP r_so_", name, "_get_number_of_", child, "(SEXP self)", sep='')
    print("{")
    print("\tint number = so_", name, "_get_number_of_", child, "(R_ExternalPtrAddr(self));", sep='')
    print("\tSEXP r_int = PROTECT(NEW_INTEGER(1));")
    print("\tINTEGER(r_int)[0] = number;")
    print("\tUNPROTECT(1);")
    print("\treturn r_int;")
    print("}")

def print_add_child(name, child, cls):
    print("SEXP r_so_", name, "_add_", child, "(SEXP self, SEXP child)", sep='')
    print("{")
    print("\tso_", name, "_add_", child, "(R_ExternalPtrAddr(self), R_ExternalPtrAddr(child));", sep='')
    print("\treturn(R_NilValue);")
    print("}")

def print_get_child(name, child):
    print("SEXP ", common.create_get_name(name, child['name'], prefix="r_so"), "(SEXP self", sep ='', end='')
    if child.get("array", False):
        print(", SEXP index", end='')
    print(")")
    print("{")
    if child['type'] == "type_string":
        this_type = "char"
    elif child['type'] == "type_real":
        this_type = "double"
    elif child['type'] == "type_int":
        this_type = "int"
    else:
        this_type = "so_" + child['type']
    print("\t", this_type, " *child = ", common.create_get_name(name, child['name'], prefix="so"), "(R_ExternalPtrAddr(self)", sep='', end='')
    if child.get("array", False):
        print(", INTEGER(index)[0]", end='')
    print(");")
    if child['type'] == 'Table':
        print("\tSEXP result = table2df(child);")
    elif child['type'] == 'estring':
        print("\tSEXP result = estring2character(child);")
    elif child['type'] == 'type_string':
        print("\tif (!child) {")
        print("\t\treturn R_NilValue;")
        print("\t}")
        print("\tSEXP result;")
        print("\tPROTECT(result = NEW_STRING(1));")
        print("\tSET_STRING_ELT(result, 0, mkChar(child));")
        print("\tUNPROTECT(1);")
    elif child['type'] == 'type_real':
        print("\tif (!child) {")
        print("\t\treturn R_NilValue;")
        print("\t}")
        print("\tSEXP result;")
        print("\tPROTECT(result = NEW_NUMERIC(1));")
        print("\tREAL(result)[0] = *child;")
        print("\tUNPROTECT(1);")
    elif child['type'] == 'type_int':
        print("\tif (!child) {")
        print("\t\treturn R_NilValue;")
        print("\t}")
        print("\tSEXP result;")
        print("\tPROTECT(result = NEW_INTEGER(1));")
        print("\tINTEGER(result)[0] = *child;")
        print("\tUNPROTECT(1);")
    elif child['type'] == 'Matrix':
        print("\tSEXP result = matrix2Rmatrix(child);")
    else:
        print("\tSEXP result = R_MakeExternalPtr(child, R_NilValue, R_NilValue);")
    print("\treturn result;")
    print("}")

def print_set_child(name, child):
    print("SEXP ", common.create_set_name(name, child['name'], prefix="r_so"), "(SEXP self, SEXP child)", sep='')
    print("{")
    if child['type'] == "Table":
        print("\tso_Table *table = df2table(child, \"", child['name'], "\");", sep='')
        print("\tso_Table_free(so_", name, "_get_", child['name'], "(R_ExternalPtrAddr(self)));", sep='')
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), table);", sep='')
    elif child['type'] == "estring":
        prefix = ''
        if child.get('prefix', False):
            prefix = child['prefix'] + ":"
        print("\tso_estring *estring = character2estring(child, \"", prefix, child['name'], "\");", sep='')
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), estring);", sep='')
    elif child['type'] == 'type_string':
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), (char *) CHAR(STRING_ELT(child, 0)));", sep='')
    elif child['type'] == 'type_real':
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), REAL(child));", sep='')
    elif child['type'] == 'type_int':
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), INTEGER(child));", sep='')
    elif child['type'] == 'Matrix':
        print("\tso_Matrix *matrix = Rmatrix2matrix(child, \"", child['name'], "\");", sep='')
        print("\tso_Matrix_free(so_", name, "_get_", child['name'], "(R_ExternalPtrAddr(self)));", sep='')
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), matrix);", sep='')
    else:
        print("\tso_", name, "_set_", child['name'], "(R_ExternalPtrAddr(self), R_ExternalPtrAddr(child));", sep='')
    print("\treturn R_NilValue;")
    print("}")

def print_create_child(name, child, cls):
    if cls != 'type_string' and cls != 'type_real' and cls != 'type_int':
        print("SEXP r_so_", name, "_create_", child, "(SEXP self)", sep='')
        print("{")
        print("\tso_", cls, " *child = so_", name, "_create_", child, "(R_ExternalPtrAddr(self));", sep='')
        print("\treturn R_MakeExternalPtr(child, R_NilValue, R_NilValue);")
        print("}")

def print_includes():
    print("#include <R.h>")
    print("#include <Rdefines.h>")
    print("#include <so.h>")
    print("#include \"soc.h\"")

def print_wrapper_functions(name, struct):
    print("so_", name, "_new <- function() {", sep='')
    print("\t.Call(\"r_so_", name, "_new\")", sep='')
    print("}")
    print()
    print("so_", name, "_free <- function(self) {", sep='')
    print("\t.Call(\"r_so_", name, "_free\", self)", sep='')
    print("}")
    print()
    print("so_", name, "_ref <- function(self) {", sep='')
    print("\t.Call(\"r_so_", name, "_ref\", self)", sep='')
    print("}")
    print()
    print("so_", name, "_unref <- function(self) {", sep='')
    print("\t.Call(\"r_so_", name, "_unref\", self)", sep='')
    print("}")
    print()

    for attr in struct.get('attributes', []):
        print("so_", name, "_get_", attr, " <- function(self) {", sep='')
        print("\t.Call(\"r_so_", name, "_get_", attr, "\", self)", sep='')
        print("}")
        print()
        print("so_", name, "_set_", attr, " <- function(self, value) {", sep='')
        print("\t.Call(\"r_so_", name, "_set_", attr, "\", self, value)", sep='')
        print("}")
        print()

    if 'children' in struct:
        for child in struct['children']:
            print("so_", name, "_get_", child['name'], " <- function(self", sep='', end='')
            if child.get('array', False):
                print(", number", end='')
            print(") {")
            print("\t.Call(\"r_so_", name, "_get_", child['name'], "\", self", sep='', end='')
            if child.get('array', False):
                print(", number", end='')
            print(")")
            print("}")
            print()
            if child.get('array', False):
                print("so_", name, "_get_number_of_", child['name'], " <- function(self) {", sep='')
                print("\t.Call(\"r_so_", name, "_get_number_of_", child['name'], "\", self)", sep='')
                print("}")
                print("so_", name, "_add_", child['name'], " <- function(self, value) {", sep='')
                print("\t.Call(\"r_so_", name, "_add_", child['name'], "\", self, value)", sep='')
                print("}")
            else:
                print("so_", name, "_set_", child['name'], " <- function(self, value) {", sep='')
                print("\t.Call(\"r_so_", name, "_set_", child['name'], "\", self, value)", sep='')
                print("}")
            print()
            if child['type'] != "type_string" and child['type'] != "type_real" and child['type'] != "type_int":
                print("so_", name, "_create_", child['name'], " <- function(self) {", sep='')
                print("\t.Call(\"r_so_", name, "_create_", child['name'], "\", self, value)", sep='')
                print("}")
                print()

def print_accessors(name, struct):
    # attributes
    for attr in struct.get('attributes', []):
        print(attr, "_acc <- function(value)", sep='')
        print("{")
        print("\tif (!isnull(.self$.cobj)) {")
        print("\t\tif (missing(value)) {")
        print("\t\t\tso_", name, "_get_", attr, "(.self$.cobj)", sep='')
        print("\t\t} else {")
        print("\t\t\tso_", name, "_set_", attr, "(.self$.cobj, value)", sep='')
        print("\t\t}")
        print("\t}")
        print("}")
        print()

    # children
    if 'children' in struct:
        for child in struct['children']:
            print(child['name'], "_acc <- function(value)", sep='')
            print("{")
            print("\tif (!isnull(.self$.cobj)) {")
            print("\t\tif (missing(value)) {")

            if child.get('array', False):
                print("\t\t\tn = so_", name, "_get_number_of_", child['name'], "(.self$.cobj)", sep='')
                print("\t\t\tif (n > 0) {")
                print("\t\t\t\ta = list()")
                print("\t\t\t\tfor (i in seq.int(1, n)) {")
                print("\t\t\t\t\tchild = so_", name, "_get_", child['name'], "(.self$.cobj, i - 1L)", sep='')
                print("\t\t\t\t\ta[i] = so_", child['type'], "$new(cobj=child)", sep='')
                print("\t\t\t\t}")
                print("\t\t\t\treturn(a)")
                print("\t\t\t}")
            elif child['type'] == 'Table' or child['type'] == 'estring' or child['type'] == 'type_string' or child['type'] == 'type_real' or child['type'] == 'type_int' or child['type'] == 'Matrix':
                print("\t\t\tso_", name, "_get_", child['name'], "(.self$.cobj)", sep='')
            else:
                print("\t\t\tchild = so_", name, "_get_", child['name'], "(.self$.cobj)", sep='')
                print("\t\t\tif (!isnull(child)) {")
                print("\t\t\t\tso_", child['type'], "$new(cobj=child)", sep='')
                print("\t\t\t}")

            print("\t\t} else {")

            if child['type'] == 'Table' or child['type'] == 'estring' or child['type'] == 'Matrix' or child['type'] == 'type_string' or child['type'] == 'type_real' or child['type'] == 'type_int':
                print("\t\t\tso_", name, "_set_", child['name'], "(.self$.cobj, value)", sep='')
            elif child.get('array', False):
                pass
            else:
                print("\t\t\tso_", name, "_set_", child['name'], "(.self$.cobj, value$.cobj)", sep='')
                print("\t\t\tso_", child['type'], "_ref(value$.cobj)", sep='')

            print("\t\t}")
            print("\t}")
            print("}")
            print()

def print_classes(name, struct):
    print("so_", name, " = setRefClass(\"so_", name, "\",", sep='')
    print("\tfields=list(")
    for attr in struct.get('attributes', []):
        print("\t\t", attr, " = ", attr, "_acc,", sep='')
    if 'children' in struct:
        for child in struct['children']:
            print("\t\t", child['name'], " = ", child['name'], "_acc,", sep='')
    print("\t\t.cobj = \"externalptr\"")
    print("\t),")
    print("\tmethods=list(")
    print("\t\tinitialize = function(cobj) {")
    print("\t\t\tif (missing(cobj)) {")
    print("\t\t\t\t.cobj <<- so_", name, "_new()", sep='')
    print("\t\t\t} else {")
    print("\t\t\t\t.cobj <<- cobj")
    print("\t\t\t}")
    print("\t\t},")
    print("\t\tfinalize = function() {")
    #print("\t\t\tso_", name, "_unref(.self$.cobj)", sep='')
    print("\t\t}", end='')
    if 'children' in struct:
        for child in struct['children']:
            if child.get('array', False):
                print(",")
                print("\t\tadd_", child['name'], " = function(value) {", sep='')
                print("\t\t\tso_", name, "_add_", child['name'], "(.self$.cobj, value$.cobj)", sep='')
                print("\t\t\tinvisible(so_", child['name'], "_ref(value$.cobj))", sep='')
                print("\t\t}", end='')
    print()
    print("\t)")
    print(")")
    ns = open("../NAMESPACE", "a")
    print("export(so_", name, ")", sep='', file=ns)


# Generate C part of binding
os.chdir("../R/src")

for name in structure:
    with open("gen-" + name + ".c", "w") as f:

        sys.stdout = f
        print_includes()
        print()
        print_new(name)
        print()
        print_free(name)
        print()
        print_ref_unref(name)
        print()

        if 'children' in structure[name]:
            for child in structure[name]['children']:
                print_get_child(name, child)
                print()
                if child.get('array', False):
                    print_get_number_of(name, child['name'], child['type'])
                    print_add_child(name, child['name'], child['type'])
                else:
                    print_set_child(name, child)
                    print()
                print_create_child(name, child['name'], child['type'])
                print()

        for attr in structure[name].get('attributes', []):
            print_attribute_getter(name, attr)
            print()
            print_attribute_setter(name, attr)
            print()


# Generate R part of binding
os.chdir("../R")

with open("../NAMESPACE", "w") as ns:
    print("useDynLib(soc)", file=ns)
    print("export(so_SO_read)", file=ns)
    print("export(so_SO_write)", file=ns)
    print("export(so_SO)", file=ns)

for name in structure:
    with open("gen-" + name + ".R", "w") as f:
        sys.stdout = f

        print_wrapper_functions(name, structure[name])
        print_accessors(name, structure[name])
        print_classes(name, structure[name])
