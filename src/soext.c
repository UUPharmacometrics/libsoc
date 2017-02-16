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
#include <libxml/xmlwriter.h>

#include <so.h>
#include <so/private/SO.h>
#include <so/private/SOBlock.h>
#include <pharmml/common_types.h>
#include <so/Table.h>

static char *last_error;

char *so_get_last_error()
{
    return last_error;
}

void so_SO_on_start_element(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI,
    int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
    char *name = (char *) localname;
    so_SO *so = (so_SO *) ctx;
    if (strcmp("SO", name) == 0) {
        so_SO_init_attributes(so, nb_attributes, (const char **) attributes);
    } else {
        int fail = so_SO_start_element(so, name, nb_attributes, (const char **) attributes);
        if (fail) {     // FIXME: should possibly abort here
            so->error = fail;
        }
    }
}

void so_SO_on_end_element(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI)
{
    char *name = (char *) localname;
    so_SO *so = (so_SO *) ctx;
    so_SO_end_element(so, name);
}

void so_SO_on_characters(void *ctx, const xmlChar *ch, int len)
{
    so_SO *so = (so_SO *) ctx;
    int fail = so_SO_characters(so, (const char *) ch, len);
    if (fail) {         // FIXME: should possibly abort here
        so->error = fail;
    }
}

void error_func(void *ctx, const char *msg, ...)
{

}

/** \memberof so_SO
 * Read an SO from file
 * \param filename - the file to read
 * \return A pointer to an so_SO structure containing the read file
 * \sa so_SO_write
 */
so_SO *so_SO_read(char *filename)
{
    so_SO *so = so_SO_new();

    xmlSAXHandler sax_handler;

    memset(&sax_handler, 0, sizeof(xmlSAXHandler));
    sax_handler.initialized = XML_SAX2_MAGIC;
    sax_handler.startElementNs = so_SO_on_start_element;
    sax_handler.endElementNs = so_SO_on_end_element;
    sax_handler.characters = so_SO_on_characters;

    // Turn off error output to STDERR
    xmlGenericErrorFunc handler = (xmlGenericErrorFunc) error_func;
    initGenericErrorDefaultFunc(&handler);

    int err = xmlSAXUserParseFile(&sax_handler, so, filename); 

    if (so->error) {
        so_SO_free(so);
        last_error = "SO read error";
        return NULL;
    }

    if (err) { 
        so_SO_free(so);
        xmlError *error = xmlGetLastError();
        last_error = error->message;
        return NULL;
    }

    return so;
}

/** \memberof so_SO
 * Write an SO structure to file
 * \param self - The SO to write
 * \param filename - the file to write to
 * \param pretty - 1 for nice indentation, 0 for compact
 * \return - 0 if no error
 * \sa so_SO_read
 */
int so_SO_write(so_SO *self, char *filename, int pretty)
{
    int rc;
    xmlTextWriterPtr writer;

    writer = xmlNewTextWriterFilename(filename, 0);
    if (!writer) return 1;
    if (pretty) {
        rc = xmlTextWriterSetIndent(writer, 1);
        if (rc < 0) return 1;
        rc = xmlTextWriterSetIndentString(writer, BAD_CAST "  ");
        if (rc < 0) return 1;
    }

    rc = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
    if (rc < 0) return 1;

    rc = so_SO_xml(self, writer);
    if (rc != 0) return 1;
   
    // Properties must be set AFTER the Element was written. 

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) return 1;

    xmlFreeTextWriter(writer);

    return 0;
}

/** \memberof so_SO
 * Get a specific SOBlock given its name
 * \param self - The SO structure
 * \param name - the name (blkId) to look for
 * \return - A pointer to the SOBlock with the given name or NULL if not found
 * \sa so_SO_get_SOBlock
 */
so_SOBlock *so_SO_get_SOBlock_from_name(so_SO *self, char *name)
{
    for (int i = 0; i < self->num_SOBlock; i++) {
        if (strcmp(name, self->SOBlock[i]->blkId) == 0) {
            return self->SOBlock[i];
        }
    }

    return NULL;
}

/** \memberof so_SO
 * Gather all mle population estimates over all SOBlocks
 * \param self - The SO structure
 * \return - A pointer to a newly created table with the results
 */
so_Table *so_SO_all_population_estimates(so_SO *self)
{
    so_Table *table = so_Table_new();

    if (!table) {
        return table;
    }

    int numcols = 0;

    // Find all parameters and create an empty column for each
    for (int i = 0; i < so_SO_get_number_of_SOBlock(self); i++) {
        so_SOBlock *block = so_SO_get_SOBlock(self, i);
        so_Estimation *est = so_SOBlock_get_Estimation(block);
        if (!est)
            continue;
        so_PopulationEstimates *pe = so_Estimation_get_PopulationEstimates(est);
        if (!pe)
            continue;
        so_Table *mle = so_PopulationEstimates_get_MLE(pe);
        if (!mle)
            continue;
        int numcols_mle = so_Table_get_number_of_columns(mle);
        for (int col = 0; col < numcols_mle; col++) {
            pharmml_valueType value_type = so_Table_get_valueType(mle, col);
            pharmml_columnType column_type = PHARMML_COLTYPE_UNDEFINED;        // Currently no support for multiple types so set unknown for now
            char *columnId = so_Table_get_columnId(mle, col);
            if (so_Table_get_index_from_name(table, columnId) == -1) {    // Do we not yet have this parameter?
                so_Table_new_column_no_copy(table, columnId, column_type, value_type, NULL);
                numcols++;
            }
        }
    }

    // Add one row per SOBlock
    for (int i = 0; i < so_SO_get_number_of_SOBlock(self); i++) {
        so_SOBlock *block = so_SO_get_SOBlock(self, i);
        so_Estimation *est = so_SOBlock_get_Estimation(block);
        so_Table *mle = NULL;
        if (est) {
            so_PopulationEstimates *pe = so_Estimation_get_PopulationEstimates(est);
            if (pe) {
                mle = so_PopulationEstimates_get_MLE(pe);
            }
        }

        table->numrows++;

        for (int col = 0; col < numcols; col++) {
            so_Column *new_column = table->columns[col];
            if (mle) {
                char *id = so_Table_get_columnId(table, col);
                void *data = so_Table_get_column_from_name(mle, id);
                if (data) {
                    double *real = (double *) data;     // Must probably be real
                    so_Column_add_real(new_column, real[0]);
                } else {
                    so_Column_add_real(new_column, pharmml_na());
                }
            } else {        // No table set all to NA
                so_Column_add_real(new_column, pharmml_na()); 
            }
        }
    }

    return table;
}

/** \memberof so_SO
 * Gather all mle standard errors for all parameters over all SOBlocks
 * \param self - The SO structure
 * \return - A pointer to a newly created table with the results
 */
so_Table *so_SO_all_standard_errors(so_SO *self)
{
    so_Table *table = so_Table_new();

    if (!table) {
        return table;
    }

    int numcols = 0;

    // Find all parameters and create an empty column for each
    for (int i = 0; i < so_SO_get_number_of_SOBlock(self); i++) {
        so_SOBlock *block = so_SO_get_SOBlock(self, i);
        so_Estimation *est = so_SOBlock_get_Estimation(block);
        if (!est)
            continue;
        so_PrecisionPopulationEstimates *ppe = so_Estimation_get_PrecisionPopulationEstimates(est);
        if (!ppe)
            continue;
        so_MLE *mle = so_PrecisionPopulationEstimates_get_MLE(ppe);
        if (!mle)
            continue;
        so_Table *se = so_MLE_get_StandardError(mle);
        if (!se)
            continue;
        int numrows_se = so_Table_get_number_of_rows(se);
        char **names = (char **) so_Table_get_column_from_name(se, "Parameter");
        pharmml_valueType value_type = so_Table_get_valueType(se, 1);      // SEs are in the second column
        pharmml_columnType column_type = PHARMML_COLTYPE_UNDEFINED;         // SE table does not support this in the SO
        for (int row = 0; row < numrows_se; row++) {
            char *columnId = names[row];
            if (so_Table_get_index_from_name(table, columnId) == -1) {    // Do we not yet have this parameter?
                so_Table_new_column_no_copy(table, columnId, column_type, value_type, NULL);
                numcols++;
            }
        }
    }

    // Add one row per SOBlock
    for (int i = 0; i < so_SO_get_number_of_SOBlock(self); i++) {
        so_SOBlock *block = so_SO_get_SOBlock(self, i);
        so_Estimation *est = so_SOBlock_get_Estimation(block);
        double *data = NULL;
        char **names;
        int numrows;
        if (est) {
            so_PrecisionPopulationEstimates *pe = so_Estimation_get_PrecisionPopulationEstimates(est);
            if (pe) {
                so_MLE *mle = so_PrecisionPopulationEstimates_get_MLE(pe);
                if (mle) {
                    so_Table *se = so_MLE_get_StandardError(mle);
                    if (se) {
                        data = (double *) so_Table_get_column_from_name(se, "SE");
                        names = (char **) so_Table_get_column_from_name(se, "Parameter");
                        numrows = so_Table_get_number_of_rows(se);
                    }
                }
            }
        }

        table->numrows++;

        for (int col = 0; col < numcols; col++) {
            so_Column *new_column = table->columns[col];
            if (data) {
                // Find row in se table
                char *id = so_Table_get_columnId(table, col);
                if (id) {
                    int found = 0;
                    for (int row = 0; row < numrows; row++) {           // Loop through rows to find id
                        if (strcmp(names[row], id) == 0) {
                            so_Column_add_real(new_column, data[row]);
                            found = 1;
                            break;
                        }
                    }
                    if (!found)
                        so_Column_add_real(new_column, pharmml_na());
                } else {
                    so_Column_add_real(new_column, pharmml_na());
                }
            } else {        // No table set all to NA
                so_Column_add_real(new_column, pharmml_na());
            }
        }
    }

    return table;
}
