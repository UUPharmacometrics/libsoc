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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

double pharmml_string_to_double(const char *str)
{
    return atof(str);
}

char *pharmml_double_to_string(double x)
{
    size_t needed = snprintf(NULL, 0, "%f", x) + 1;
    if (needed <= 0) {
        return NULL;
    }
    char *buffer = malloc(needed);
    if (!buffer) {
        return NULL;
    }
    int err = snprintf(buffer, needed, "%f", x);
    if (err < 0) {
        free(buffer);
        buffer = NULL;
    } 
    return buffer;
}

int pharmml_string_to_int(const char *str)
{
    return atoi(str);
}

char *pharmml_int_to_string(int x)
{
    size_t needed = snprintf(NULL, 0, "%d", x) + 1;
    if (needed <= 0) {
        return NULL;
    }
    char *buffer = malloc(needed);
    if (!buffer) {
        return NULL;
    }
    int err = snprintf(buffer, needed, "%d", x);
    if (err < 0) {
        free(buffer);
        buffer = NULL;
    }
    return buffer;
}

char *pharmml_strdup(const char *str)
{
    size_t len = 1 + strlen(str);
    char *p = malloc(len);
    if (p) {
        memcpy(p, str, len);
    }

    return p;
}

char *pharmml_strndup(const char *str, size_t n)
{
    char *p = malloc(n + 1);
    if (p) {
        memcpy(p, str, n);
        p[n] = '\0';
    }

    return p;
}

int pharmml_copy_string_array(char **source, char **dest, int length)
{
    int fail = 0;

    for (int i = 0; i < length; i++) {
        dest[i] = pharmml_strdup(source[i]);
        if (!dest[i]) {
            for (int j = 0; j < i; j++) {
                free(dest[j]);
            }
            fail = 1;
            break;
        }
    }

    return fail;
}

void pharmml_free_string_array(char **array, int length)
{
    for (int i = 0; i < length; i++) {
        free(array[i]);
    }
    free(array);
}
