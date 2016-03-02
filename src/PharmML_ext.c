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

#include <pharmml.h>
#include <pharmml/private/PharmML.h>

static char *last_error;

char *pharmml_get_last_error()
{
    return last_error;
}

void pharmml_PharmML_on_start_element(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI,
    int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
    char *name = (char *) localname;
    pharmml_PharmML *pharmml = (pharmml_PharmML *) ctx;
    if (strcmp("PharmML", name) == 0) {
        pharmml->error = pharmml_PharmML_start_element(pharmml, name, nb_attributes, (const char **) attributes);
        //pharmml_PharmML_init_attributes(pharmml, nb_attributes, (const char **) attributes);
    } else {
        pharmml->error = pharmml_PharmML_start_element(pharmml, name, nb_attributes, (const char **) attributes);
    }
}

void pharmml_PharmML_on_end_element(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI)
{
    char *name = (char *) localname;
    pharmml_PharmML *pharmml = (pharmml_PharmML *) ctx;
    pharmml_PharmML_end_element(pharmml, name);
}

void pharmml_PharmML_on_characters(void *ctx, const xmlChar *ch, int len)
{
    pharmml_PharmML *pharmml = (pharmml_PharmML *) ctx;
    pharmml->error = pharmml_PharmML_characters(pharmml, (const char *) ch, len);
}

static void error_func(void *ctx, const char *msg, ...)
{

}

/** \memberof pharmml_PharmML
 * Read a PharmML from file
 * \param filename - the file to read
 * \return A pointer to an pharmml_PharmML structure containing the read file
 * \sa pharmml_PharmML_write
 */
pharmml_PharmML *pharmml_PharmML_read(char *filename)
{
    pharmml_PharmML *pharmml = pharmml_PharmML_new();

    xmlSAXHandler sax_handler;

    memset(&sax_handler, 0, sizeof(xmlSAXHandler));
    sax_handler.initialized = XML_SAX2_MAGIC;
    sax_handler.startElementNs = pharmml_PharmML_on_start_element;
    sax_handler.endElementNs = pharmml_PharmML_on_end_element;
    sax_handler.characters = pharmml_PharmML_on_characters;

    // Turn off error output to STDERR
    xmlGenericErrorFunc handler = (xmlGenericErrorFunc) error_func;
    initGenericErrorDefaultFunc(&handler);

    int err = xmlSAXUserParseFile(&sax_handler, pharmml, filename); 

    if (!err) 
        return pharmml;
    else {
        pharmml_PharmML_free(pharmml);
        xmlError *error = xmlGetLastError();
        last_error = error->message;
        return NULL;
    }

    if (pharmml->error) {
        pharmml_PharmML_free(pharmml);
        last_error = "Memory allocation error";
        return NULL;
    }
}

/** \memberof pharmml_PharmML
 * Write an PharmML structure to file
 * \param self - The PharmML to write
 * \param filename - the file to write to
 * \param pretty - 1 for nice indentation, 0 for compact
 * \return - 0 if no error
 * \sa pharmml_PharmML_read
 */
int pharmml_PharmML_write(pharmml_PharmML *self, char *filename, int pretty)
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

    rc = pharmml_PharmML_xml(self, writer);
    if (rc != 0) return 1;
   
    // Properties must be set AFTER the Element was written. 

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) return 1;

    xmlFreeTextWriter(writer);

    return 0;
}
