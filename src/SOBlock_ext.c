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

so_Table *so_SOBlock_all_simulated_profiles(so_SOBlock *self)
{
    so_Simulation *simulation = so_SOBlock_get_Simulation(self);
    if (!simulation) return NULL;
    int num_simulation_blocks = so_Simulation_get_number_of_SimulationBlock(simulation);
    if (!num_simulation_blocks) return NULL;

    so_Table *table = so_Table_new();

    if (!table) {
        return table;
    }

    int numcols = 0;
    int have_name = 0;

    // Find all blocks and profiles and create one column for each unique column
    for (int i = 0; i < num_simulation_blocks; i++) {
        so_SimulationBlock *block = so_Simulation_get_SimulationBlock(simulation, i);
        int num_simulation_profiles = so_SimulationBlock_get_number_of_SimulatedProfiles(block);
        for (int j = 0; j < num_simulation_profiles; j++) {
            so_SimulationSubType *subtype = so_SimulationBlock_get_SimulatedProfiles(block, j);
            if (so_SimulationSubType_get_name(subtype)) {  // Have a name?
                have_name = 1;
            }
            so_Table *current_table = so_SimulationSubType_get_base(subtype);
            int current_numcols = so_Table_get_number_of_columns(current_table);
            for (int col = 0; col < current_numcols; col++) {
                pharmml_valueType value_type = so_Table_get_valueType(current_table, col);
                pharmml_columnType *column_type = so_Table_get_columnType(current_table, col);
                int num_column_types = so_Table_get_num_columnTypes(current_table, col);
                char *columnId = so_Table_get_columnId(current_table, col);
                if (so_Table_get_index_from_name(table, columnId) == -1) {    // Do we not yet have this column?
                    so_Table_new_column_no_copy(table, columnId, column_type, num_column_types, value_type, NULL);
                    numcols++;
                }
            }
        }
    }
    pharmml_columnType undefined[] = { PHARMML_COLTYPE_UNDEFINED };
    if (have_name) {
        so_Table_new_column_no_copy(table, "name", undefined, 1, PHARMML_VALUETYPE_STRING, NULL);
    }
    so_Table_new_column_no_copy(table, "replicate", undefined, 1, PHARMML_VALUETYPE_INT, NULL);

    // Add data
    for (int i = 0; i < num_simulation_blocks; i++) {
        so_SimulationBlock *block = so_Simulation_get_SimulationBlock(simulation, i);
        int num_simulation_profiles = so_SimulationBlock_get_number_of_SimulatedProfiles(block);
        for (int j = 0; j < num_simulation_profiles; j++) {
            so_SimulationSubType *subtype = so_SimulationBlock_get_SimulatedProfiles(block, j);
            so_Table *current_table = so_SimulationSubType_get_base(subtype);
            int current_numrows = so_Table_get_number_of_rows(current_table); 

            // Loop through all cols except name and replicate
            for (int col = 0; col < numcols; col++) {
                so_Column *target_column = table->columns[col];
                char *columnId = so_Table_get_columnId(table, col);
                void *data = so_Table_get_column_from_name(current_table, columnId);
                pharmml_valueType value_type = so_Table_get_valueType(current_table, col);
                if (data) {      // Is this column available?
                    for (int row = 0; row < current_numrows; row++) {
                        if (value_type == PHARMML_VALUETYPE_REAL) {
                            double *real = (double *) data;     // FIXME: Need special merge function
                            so_Column_add_real(target_column, real[row]);
                        } else {    // FIXME: Assume string
                            char **string = (char **) data;
                            so_Column_add_string(target_column, string[row]);
                        }
                    }
                } else {
                    for (int row = 0; row < current_numrows; row++) {
                        so_Column_add_real(target_column, pharmml_na());
                    }
                } 
            }

            // Handle name column
            if (have_name) {
                so_Column *name_column = table->columns[numcols];
                char *name = so_SimulationSubType_get_name(subtype);
                for (int row = 0; row < current_numrows; row++) {
                    so_Column_add_string(name_column, name);
                } 
            }

            // Handle replicate column
            so_Column *replicate_column = table->columns[numcols + have_name];
            int *replicate = so_SimulationBlock_get_replicate(block);
            for (int row = 0; row < current_numrows; row++) {
                so_Column_add_int(replicate_column, *replicate);
            }

            table->numrows += current_numrows;
        }
    }

    return table;
}
