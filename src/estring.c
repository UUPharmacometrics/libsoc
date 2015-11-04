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

so_estring *so_estring_new(char *name)
{
    so_estring *self = extcalloc(sizeof(so_estring));
    so_estring_set_name(self, name);
    self->reference_count = 1;

    return self;
}

void so_estring_free(so_estring *self)
{
    if (self) {
        free(self->name);
        free(self->string);
        free(self);
    }
}

void so_estring_ref(so_estring *self)
{
    self->reference_count++;
}

void so_estring_unref(so_estring *self)
{
    if (self) {
        self->reference_count--;
        if (!self->reference_count) {
            so_estring_free(self);
        }
    }
}

void so_estring_set_name(so_estring *self, char *name)
{
    self->name = extstrdup(name);
}

char *so_estring_get_name(so_estring *self)
{
    return self->name;
}

void so_estring_set_string(so_estring *self, char *string)
{
    self->string = extstrdup(string);
}

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
