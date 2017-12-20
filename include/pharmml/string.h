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

#ifndef _PHARMML_STRING_H
#define _PHARMML_STRING_H

double pharmml_string_to_double(const char *str);
char *pharmml_double_to_string(double x);
int pharmml_string_to_int(const char *str);
char *pharmml_int_to_string(int x);
char *pharmml_strdup(const char *str);
char *pharmml_strndup(const char *str, size_t n);
int pharmml_copy_string_array(char **source, char **dest, int length);
void pharmml_free_string_array(char **array, int length);
int so_string_path_length(char *path);

#endif
