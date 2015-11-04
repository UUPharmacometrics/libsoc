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

#include <libxml/tree.h>
#include <so/estring.h>
#include <so/private/estring.h>

#include <so/private/util.h>

/** \struct so_estring
	 \brief A structure representing an element containing a generic string
*/

/** \memberof so_estring
 * Create a new empty so_estring structure.
 * \return A pointer to the newly created struct
 * \sa so_estring_free
 */
so_estring *so_estring_new(char *name)
{
    so_estring *self = extcalloc(sizeof(so_estring));
    so_estring_set_name(self, name);
    self->reference_count = 1;

    return self;
}

/** \memberof so_estring
 * Free all memory associated with a so_estring structure.
 * \param self - a pointer to the structure to free
 * \sa so_estring_new
 */
void so_estring_free(so_estring *self)
{
    if (self) {
        free(self->name);
        free(self->string);
        free(self);
    }
}

/** \memberof so_estring
 * Increase the reference count of an so_estring structure 
 * \param self - a pointer to the structure
 * \sa so_estring_unref
 */
void so_estring_ref(so_estring *self)
{
    self->reference_count++;
}

/** \memberof so_estring
 * Decrease the reference count of an so_estring structure.
 * If the reference count reaches zero the so_estring will be freed
 * \param self - a pointer to the estring
 * \sa so_estring_ref, so_estring_free
 */
void so_estring_unref(so_estring *self)
{
    if (self) {
        self->reference_count--;
        if (!self->reference_count) {
            so_estring_free(self);
        }
    }
}

/** \memberof so_estring
 * Set the name of an estring element
 * \param self - pointer to an so_estring
 * \param name - A pointer to the string to set as name
 * \sa so_estring_get_name
 */
void so_estring_set_name(so_estring *self, char *name)
{
    self->name = extstrdup(name);
}

/** \memberof so_estring
 * Get the name of an estring element
 * \param self - pointer to an so_estring
 * \return A pointer to the name string. The string will be copied so it is the responsibility of the caller to free this if necessary.
 * \sa so_estring_set_name
 */
char *so_estring_get_name(so_estring *self)
{
    return self->name;
}

/** \memberof so_estring
 * Set the content of an estring
 * \param self - pointer to an so_estring
 * \param string - A pointer to the string to set as content
 * \sa so_estring_get_string
 */
void so_estring_set_string(so_estring *self, char *string)
{
    self->string = extstrdup(string);
}

/** \memberof so_estring
 * Get the content of an estring
 * \param self - pointer to an so_estring
 * \return string - A pointer to the content string. The string will be copied so it is the responsibility of the caller to free this if necessary.
 * \sa so_estring_set_string
 */
char *so_estring_get_string(so_estring *self)
{
    return self->string;
}

so_xml so_estring_xml(so_estring *self)
{
	xmlNodePtr xml = NULL;

    if (self->name && self->string) {
		xml = xmlNewNode(NULL, BAD_CAST self->name);
        xmlAddChild(xml, xmlNewText(BAD_CAST self->string));
    }

    return xml;
}

void so_estring_start_element(so_estring *self, const char *localname, int nb_attributes, const char **attributes)
{
}

void so_estring_end_element(so_estring *self, const char *localname)
{
}

void so_estring_characters(so_estring *self, const char *ch, int len)
{
   self->string = extstrndup(ch, len); 
}
