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

#include <stdlib.h>
#include <memory.h>
#include <libxml/SAX.h>

#include <so.h>
#include <so/private/SO.h>
#include <so/private/SOBlock.h>
#include <so/Table.h>

void so_SOBlock_add_rawresults_datafile(so_SOBlock *self, char *description, char *path, char *oid)
{
    if (!self->RawResults) {
        so_SOBlock_create_RawResults(self);
    }

    so_RawResults *rr = self->RawResults;

    so_Table *df = so_RawResults_create_DataFile(rr);
    so_ExternalFile *ext = so_Table_create_ExternalFile(df);

    so_ExternalFile_set_oid(ext, oid);

    if (description) {
        so_ExternalFile_set_Description(ext, description);
    }
    if (path) {
        so_ExternalFile_set_path(ext, path);
    }
}

void so_SOBlock_add_rawresults_graphicsfile(so_SOBlock *self, char *description, char *path, char *oid)
{
    if (!self->RawResults) {
        so_SOBlock_create_RawResults(self);
    }

    so_RawResults *rr = self->RawResults;

    so_ExternalFile *df = so_RawResults_create_GraphicsFile(rr);

    so_ExternalFile_set_oid(df, oid);
    if (description) {
        so_ExternalFile_set_Description(df, description);
    }
    if (path) {
        so_ExternalFile_set_path(df, path);
    }
}

void so_SOBlock_add_message(so_SOBlock *self, char *type, char *toolname, char *name, char *content, int severity)
{
    if (!self->TaskInformation) {
        so_SOBlock_create_TaskInformation(self);
    }

    so_TaskInformation *ti = self->TaskInformation;

    so_Message *m = so_TaskInformation_create_Message(ti);

    so_Message_set_type(m, type);
    so_Message_set_Toolname(m, toolname);
    so_Message_set_Name(m, name);
    so_Message_set_Content(m, content);
    so_Message_set_Severity(m, &severity);
}
