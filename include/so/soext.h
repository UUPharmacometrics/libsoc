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

#ifndef _SO_SOEXT_H
#define _SO_SOEXT_H

char *so_get_last_error();
so_SO *so_SO_read(char *filename);
int so_SO_write(so_SO *self, char *filename, int pretty);
so_SOBlock *so_SO_get_SOBlock_from_name(so_SO *self, char *name);
so_Table *so_SO_all_population_estimates(so_SO *self);
so_Table *so_SO_all_standard_errors(so_SO *self);
int so_SO_is_ruv_parameter(so_SO *self, const char *name);
int so_SO_is_structural_parameter(so_SO *self, const char *name);
int so_SO_is_correlation_parameter(so_SO *self, const char *name);
char *so_SO_random_variable_from_variability_parameter(so_SO *self, const char *name);

#endif
