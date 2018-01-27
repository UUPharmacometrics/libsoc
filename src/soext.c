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
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

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

    int path_length = so_string_path_length(filename);
    char *path = NULL;
    if (path_length) {
        path = pharmml_strndup(filename, path_length);
    }
    so->path = path;

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

    int path_length = so_string_path_length(filename);
    char *path = NULL;
    if (path_length) {
        path = pharmml_strndup(filename, path_length);
    }
    self->path = path;

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
            pharmml_columnType *column_type = so_Table_get_columnType(mle, col);
            int number_of_columnTypes = so_Table_get_num_columnTypes(mle, col);
            char *columnId = so_Table_get_columnId(mle, col);
            if (so_Table_get_index_from_name(table, columnId) == -1) {    // Do we not yet have this parameter?
                so_Table_new_column_no_copy(table, columnId, column_type, number_of_columnTypes, value_type, NULL);
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
                so_Table_new_column_no_copy(table, columnId, &column_type, 1, value_type, NULL);
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
        int numrows = 0;
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

xmlDoc *so_SO_pharmml_dom(so_SO *self)
{
    so_PharmMLRef *ref = so_SO_get_PharmMLRef(self);
    if (!ref)
        return NULL;

    xmlDoc *doc;
    char *pharmml_name = so_PharmMLRef_get_name(ref);
    if (so_string_path_length(pharmml_name) == 0 && self->path) {     // PharmMLRef does not contain path and we have path to SO
        int size = strlen(pharmml_name) + strlen(self->path) + 1;
        char *path = malloc(size);
        if (!path)
            return NULL;
        sprintf(path, "%s%s", self->path, pharmml_name);
        doc = xmlParseFile(path);
        free(path);
    } else {
        doc = xmlParseFile(pharmml_name);   // Every call will parse the PharmML. This could be cached.
    }

    return doc;
}

xmlXPathContext *so_SO_pharmml_context(xmlDoc *doc)
{
    xmlXPathContext *context = xmlXPathNewContext(doc);
    if (!context)
        return context;

    int fail;
    fail = xmlXPathRegisterNs(context, BAD_CAST "x", BAD_CAST "http://www.pharmml.org/pharmml/0.8/PharmML");
    fail |= xmlXPathRegisterNs(context, BAD_CAST "math", BAD_CAST "http://www.pharmml.org/pharmml/0.8/Maths");
    fail |= xmlXPathRegisterNs(context, BAD_CAST "ct", BAD_CAST "http://www.pharmml.org/pharmml/0.8/CommonTypes");
    fail |= xmlXPathRegisterNs(context, BAD_CAST "ds", BAD_CAST "http://www.pharmml.org/pharmml/0.8/Dataset");
    fail |= xmlXPathRegisterNs(context, BAD_CAST "mdef", BAD_CAST "http://www.pharmml.org/pharmml/0.8/ModelDefinition");
    fail |= xmlXPathRegisterNs(context, BAD_CAST "msteps", BAD_CAST "http://www.pharmml.org/0.8/ModellingSteps");
    fail |= xmlXPathRegisterNs(context, BAD_CAST "design", BAD_CAST "http://www.pharmml.org/0.8/TrialDesign");
    fail |= xmlXPathRegisterNs(context, BAD_CAST "po", BAD_CAST "http://www.pharmml.org/probonto/ProbOnto");

    if (fail) {
        xmlXPathFreeContext(context);
        return NULL;
    }

    return context;
}

/** \memberof so_SO
 * Check if a parameter is a correlation
 * \param self - The SO structure
 * \param name - The name of the parameter
 * \return - 0 if a correlation, 1 if not a correlation, -1 if error or parameter not found
 */
int so_SO_is_correlation_parameter(so_SO *self, const char *name)
{
    int result = -1;

    xmlDoc *doc = so_SO_pharmml_dom(self);
    if (!doc) {
        return result;
    }
    xmlXPathContext *context = so_SO_pharmml_context(doc);
    if (!context) {
        xmlFreeDoc(doc);
        return result;
    }

    xmlXPathObject *object = xmlXPathEvalExpression(BAD_CAST "/x:PharmML/mdef:ModelDefinition/mdef:ParameterModel/mdef:Correlation/mdef:Pairwise/"
            "mdef:CorrelationCoefficient/ct:Assign/ct:SymbRef", context);

    if (!object) {
        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);
        return result;
    }

    xmlNodeSet *nodes = object->nodesetval;
    int size = nodes ? nodes->nodeNr : 0;
    for (int i = 0; i < size; i++) {
        char *symb_name = (char *) xmlGetNoNsProp(nodes->nodeTab[i], BAD_CAST "symbIdRef");
        if (strcmp(symb_name, name) == 0) {
            free(symb_name);
            result = 0;
            break;
        }
        free(symb_name);
    }

    if (result == -1) {
        result = 1;
    }

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    return result;
}

/** \memberof so_SO
 * Check if a parameter is a structural parameter
 * \param self - The SO structure
 * \param name - The name of the parameter
 * \return - 0 if a structural parameter, 1 if not structural, -1 if error or parameter not found
 */
int so_SO_is_structural_parameter(so_SO *self, const char *name)
{
    int result = -1;

    xmlDoc *doc = so_SO_pharmml_dom(self);
    if (!doc) {
        return result;
    }
    xmlXPathContext *context = so_SO_pharmml_context(doc);
    if (!context) {
        xmlFreeDoc(doc);
        return result;
    }

    // Find all variability parameters
    // Normal1: stdev, Normal2: var, Normal3: precision 
    xmlXPathObject *object = xmlXPathEvalExpression(BAD_CAST
        "/x:PharmML/mdef:ModelDefinition/mdef:ParameterModel/mdef:RandomVariable/mdef:Distribution/po:ProbOnto[@name='Normal1']/po:Parameter[@name='stdev']/ct:Assign/ct:SymbRef|"
        "/x:PharmML/mdef:ModelDefinition/mdef:ParameterModel/mdef:RandomVariable/mdef:Distribution/po:ProbOnto[@name='Normal2']/po:Parameter[@name='var']/ct:Assign/ct:SymbRef|"
        "/x:PharmML/mdef:ModelDefinition/mdef:ParameterModel/mdef:RandomVariable/mdef:Distribution/po:ProbOnto[@name='Normal3']/po:Parameter[@name='precision']/ct:Assign/ct:SymbRef|"
        "/x:PharmML/mdef:ModelDefinition/mdef:ParameterModel/mdef:Correlation/mdef:Pairwise/mdef:CorrelationCoefficient/ct:Assign/ct:SymbRef", context);

    if (!object) {
        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);
        return result;
    }

    xmlNodeSet *nodes = object->nodesetval;
    int size = nodes ? nodes->nodeNr : 0;
    for (int i = 0; i < size; i++) {
        char *symb_name = (char *) xmlGetNoNsProp(nodes->nodeTab[i], BAD_CAST "symbIdRef");
        if (strcmp(symb_name, name) == 0) {
            free(symb_name);
            result = 1;
            goto out;
        }
        free(symb_name);
    }

    // name is not variability but is it a PopulationParameter at all?
    xmlXPathObject *pop_object = xmlXPathEvalExpression(BAD_CAST "/x:PharmML/mdef:ModelDefinition/mdef:ParameterModel/mdef:PopulationParameter", context);
    if (!pop_object) {
        goto out;
    }

    nodes = pop_object->nodesetval;
    size = nodes ? nodes->nodeNr : 0;
    for (int i = 0; i < size; i++) {
        char *symb_name = (char *) xmlGetNoNsProp(nodes->nodeTab[i], BAD_CAST "symbId");
        if (strcmp(symb_name, name) == 0) {
            free(symb_name);
            result = 0;
            break;
        }
        free(symb_name);
    }

    xmlXPathFreeObject(pop_object);
out:
    xmlXPathFreeObject(object);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);

    return result;
}

xmlNode *so_SO_pharmml_find_random_variable(xmlXPathContext *context, const char *name)
{
    xmlXPathObject *randvar_object = xmlXPathEvalExpression(BAD_CAST "/x:PharmML/mdef:ModelDefinition/mdef:ParameterModel/mdef:RandomVariable", context);
    if (!randvar_object) {
        return NULL;
    }
            
    // Search all RVs for our symbol
    xmlNodeSet *nodes = randvar_object->nodesetval;
    int size = nodes ? nodes->nodeNr : 0;
    xmlXPathObject *search_object;
    xmlNode *found_randvar = NULL;
    for (int i = 0; i < size; i++) {
        if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
            search_object = xmlXPathNodeEval(nodes->nodeTab[i], BAD_CAST
                   "./mdef:Distribution/po:ProbOnto[@name='Normal1']/po:Parameter[@name='stdev']/ct:Assign/ct:SymbRef|"
                   "./mdef:Distribution/po:ProbOnto[@name='Normal2']/po:Parameter[@name='var']/ct:Assign/ct:SymbRef|"
                   "./mdef:Distribution/po:ProbOnto[@name='Normal3']/po:Parameter[@name='precision']/ct:Assign/ct:SymbRef", context);
            if (!search_object) {
                continue;
            }
            xmlNodeSet *symbnodes = search_object->nodesetval;
            int symbnodes_size = symbnodes ? symbnodes->nodeNr : 0;
            if (symbnodes_size != 1) {
                xmlXPathFreeObject(search_object);
                continue;
            }
            xmlChar *symb_name = xmlGetNoNsProp(symbnodes->nodeTab[0], BAD_CAST "symbIdRef");
            if (!symb_name) {
                xmlXPathFreeObject(search_object);
                continue;
            }
            if (strcmp((char *) symb_name, name) == 0) {
                // Found the Correct RandomVariable
                free(symb_name);
                xmlXPathFreeObject(search_object);
                found_randvar = nodes->nodeTab[i];
                break;
            }
            free(symb_name);
            xmlXPathFreeObject(search_object);
        }
    }
    return found_randvar;
}

/** \memberof so_SO
 * Get the name of the RandomVariable connected to a variability parameter
 * \param self - The SO structure
 * \param name - The name of the variability parameter
 * \return - A pointer to the name or NULL if not found or error. The returned string needs to be freed.
 */
char *so_SO_random_variable_from_variability_parameter(so_SO *self, const char *name)
{
    xmlDoc *doc = so_SO_pharmml_dom(self);
    if (!doc) {
        return NULL;
    }
    xmlXPathContext *context = so_SO_pharmml_context(doc);
    if (!context) {
        xmlFreeDoc(doc);
        return NULL;
    }

    xmlNode *randvar = so_SO_pharmml_find_random_variable(context, name);
    if (!randvar) {
       xmlXPathFreeContext(context);
       xmlFreeDoc(doc);
       return NULL;
    }

    char *random_variable_name = (char *) xmlGetNoNsProp(randvar, BAD_CAST "symbId");

    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);

    if (!random_variable_name) {
        return NULL;
    }

    return pharmml_strdup(random_variable_name);
}

/** \memberof so_SO
 * Check if a parameter is a variability parameter connected to RUV
 * \param self - The SO structure
 * \param name - The name of the parameter
 * \return -  0 if connected to RUV, 1 if not connected to RUV, -1 if error or parameter not found
 */
int so_SO_is_ruv_parameter(so_SO *self, const char *name)
{
    int result = -1;

    xmlXPathContext *context = NULL;
    xmlDoc *doc = so_SO_pharmml_dom(self);
    if (!doc) {
        goto end;
    }
    context = so_SO_pharmml_context(doc);
    if (!context) {
        goto end;
    }

    xmlNode *found_randvar = so_SO_pharmml_find_random_variable(context, name);
    if (!found_randvar)
        goto end;

    // Find blkId of VariabilityReference of RandomVariable
    xmlXPathObject *varref_object = xmlXPathNodeEval(found_randvar, BAD_CAST "./ct:VariabilityReference/ct:SymbRef", context);
    if (!varref_object)
        goto end;

    xmlNodeSet *varref_nodes = varref_object->nodesetval;
    int varref_size = varref_nodes ? varref_nodes->nodeNr : 0;
    if (varref_size != 1) {
        xmlXPathFreeObject(varref_object);
        goto end;
    }
    char *varref_blk = (char *) xmlGetNoNsProp(varref_nodes->nodeTab[0], BAD_CAST "blkIdRef");
    if (!varref_blk) {
        xmlXPathFreeObject(varref_object);
        goto end;
    }

    xmlXPathFreeObject(varref_object);

    // Find residualError VariabilityModel matching blkId
    const char *xpath_template_string = "/x:PharmML/mdef:ModelDefinition/mdef:VariabilityModel[@blkId='%s' and @type='residualError']";
    int xpath_size = snprintf(NULL, 0, xpath_template_string, varref_blk) + 1;
    char *xpath_string = malloc(xpath_size);
    if (!xpath_string) {
        free(varref_blk);
        goto end;
    }
    sprintf(xpath_string, xpath_template_string, varref_blk);
    free(varref_blk);

    xmlXPathObject *varmodel_object = xmlXPathEvalExpression(BAD_CAST xpath_string, context);
    free(xpath_string);
    if (!varmodel_object)
        goto end;

    xmlNodeSet *varmodel_nodes = varmodel_object->nodesetval;
    int varmodel_size = varmodel_nodes ? varmodel_nodes->nodeNr : 0;
    if (varmodel_size == 1) {
        result = 0;
    } else {
        result = 1;
    }

    xmlXPathFreeObject(varmodel_object);

end:
    if (context) xmlXPathFreeContext(context);
    if (doc) xmlFreeDoc(doc);
    return result;
}
