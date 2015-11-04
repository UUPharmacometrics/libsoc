/* libsoc - Library to handle standardised output files
 * Copyright (C) 2015 Rikard Nordgren
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * his library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 */

#include <stdlib.h>
#include <memory.h>
#include <libxml/SAX.h>

#include <so.h>
#include <so/private/SO.h>
#include <so/private/SOBlock.h>
#include <so/private/util.h>

void so_SOBlock_add_rawresults_datafile(so_SOBlock *self, char *description, char *path, char *oid)
{
    if (!self->RawResults) {
        so_SOBlock_create_RawResults(self);
    }

    so_RawResults *rr = self->RawResults;

    so_DataFile *df = so_RawResults_create_DataFile(rr);

    so_DataFile_set_oid(df, oid);
    if (description) {
        so_estring *description_estring = so_DataFile_create_Description(df);
        so_estring_set_name(description_estring, description); 
    }
    if (path) {
        so_estring *path_estring = so_DataFile_create_path(df);
        so_estring_set_name(path_estring, path);
    }
}
