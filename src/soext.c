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
    so_SO_start_element(so, name, nb_attributes, (const char **) attributes);
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
    so_SO_characters(so, (const char *) ch, len);
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

    if (!err) 
        return so;
    else {
        so_SO_free(so);
        xmlError *error = xmlGetLastError();
        last_error = error->message;
        return NULL;
    }
}

/** \memberof so_SO
 * Write an SO structure to file
 * \param self - The SO to write
 * \param filename - the file to write to
 * \param pretty - 1 for nice indentation, 0 for compact
 * \sa so_SO_read
 */
void so_SO_write(so_SO *self, char *filename, int pretty)
{
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");

    xmlNodePtr root = (xmlNodePtr) so_SO_xml(self);
    
    xmlNewProp(root, BAD_CAST "xmlns", BAD_CAST "http://www.pharmml.org/so/0.2/StandardisedOutput");
    xmlNewProp(root, BAD_CAST "xmlns:xsi", BAD_CAST "http://www.w3.org/2001/XMLSchema-instance");
    xmlNewProp(root, BAD_CAST "xmlns:ds", BAD_CAST "http://www.pharmml.org/pharmml/0.6/Dataset");
    xmlNewProp(root, BAD_CAST "xmlns:ct", BAD_CAST "http://www.pharmml.org/pharmml/0.6/CommonTypes");
    xmlNewProp(root, BAD_CAST "xsi:schemaLocation", BAD_CAST "http://www.pharmml.org/so/0.2/StandardisedOutput");
    xmlNewProp(root, BAD_CAST "implementedBy", BAD_CAST "MJS");
    xmlNewProp(root, BAD_CAST "writtenVersion", BAD_CAST "0.2");
    xmlNewProp(root, BAD_CAST "id", BAD_CAST "i1");

    xmlDocSetRootElement(doc, root);

    xmlSaveFormatFileEnc(filename, doc, "UTF-8", pretty);
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
