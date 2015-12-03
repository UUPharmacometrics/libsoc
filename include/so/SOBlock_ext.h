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

#ifndef _SO_SOBLOCK_EXT_H
#define _SO_SOBLOCK_EXT_H

void so_SOBlock_add_rawresults_datafile(so_SOBlock *self, char *description, char *path, char *oid);
void so_SOBlock_add_rawresults_graphicsfile(so_SOBlock *self, char *description, char *path, char *oid);
void so_SOBlock_add_message(so_SOBlock *self, char *type, char *toolname, char *name, char *content, int severity);

#endif
