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
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <R.h>
#include <Rdefines.h>
#include <so.h>

SEXP R_so_SOBlock_add_message(SEXP self, SEXP type, SEXP toolname, SEXP name, SEXP content, SEXP severity)
{
    struct so_SOBlock *c_SOBlock = R_ExternalPtrAddr(self);
    char *c_type = (char *) CHAR(STRING_ELT(type, 0));
    char *c_toolname = (char *) CHAR(STRING_ELT(toolname, 0));
    char *c_name = (char *) CHAR(STRING_ELT(name, 0));
    char *c_content = (char *) CHAR(STRING_ELT(content, 0));
    int c_severity = INTEGER(severity)[0];

    so_SOBlock_add_message(c_SOBlock, c_type, c_toolname, c_name, c_content, c_severity);

    return R_NilValue;
}

SEXP R_so_SOBlock_add_rawresults_datafile(SEXP self, SEXP description, SEXP path, SEXP oid)
{
    struct so_SOBlock *c_SOBlock = R_ExternalPtrAddr(self);
    char *c_description = (char *) CHAR(STRING_ELT(description, 0));
    char *c_path = (char *) CHAR(STRING_ELT(path, 0));
    char *c_oid = (char *) CHAR(STRING_ELT(oid, 0));

    so_SOBlock_add_rawresults_datafile(c_SOBlock, c_description, c_path, c_oid);

    return R_NilValue;
}

SEXP R_so_SOBlock_add_rawresults_graphicsfile(SEXP self, SEXP description, SEXP path, SEXP oid)
{
    struct so_SOBlock *c_SOBlock = R_ExternalPtrAddr(self);
    char *c_description = (char *) CHAR(STRING_ELT(description, 0));
    char *c_path = (char *) CHAR(STRING_ELT(path, 0));
    char *c_oid = (char *) CHAR(STRING_ELT(oid, 0));

    so_SOBlock_add_rawresults_graphicsfile(c_SOBlock, c_description, c_path, c_oid);

    return R_NilValue;
}
