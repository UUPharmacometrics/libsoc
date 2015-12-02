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

#ifndef _SO_PRIVATE_ESTRING_H
#define _SO_PRIVATE_ESTRING_H

struct so_estring {
    char *name;
    char *string;
    int reference_count;
};

so_xml so_estring_xml(so_estring *self);
void so_estring_start_element(so_estring *self, const char *localname, int nb_attributes, const char **attributes);
void so_estring_end_element(so_estring *self, const char *localname);
void so_estring_characters(so_estring *self, const char *ch, int len);

#endif
