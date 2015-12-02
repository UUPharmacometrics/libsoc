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

#ifndef _SO_ESTRING_H
#define _SO_ESTRING_H

#include <so/xml.h>

typedef struct so_estring so_estring;

so_estring *so_estring_new(char *name);
void so_estring_free(so_estring *self);
void so_estring_ref(so_estring *self);
void so_estring_unref(so_estring *self);
void so_estring_set_name(so_estring *self, char *name);
char *so_estring_get_name(so_estring *self);
void so_estring_set_string(so_estring *self, char *string);
char *so_estring_get_string(so_estring *self);

#endif
