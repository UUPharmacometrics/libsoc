#ifndef _SO_ESTRING_H
#define _SO_ESTRING_H

#include <so/xml.h>

/** \struct so_estring
	 \brief A structure representing an element containing a generic string
*/
typedef struct so_estring so_estring;

/** \memberof so_estring
 * Create a new empty so_estring structure.
 * \return A pointer to the newly created struct
 * \sa so_estring_free
 */
so_estring *so_estring_new(char *name);
/** \memberof so_estring
 * Free all memory associated with a so_estring structure.
 * \param self - a pointer to the structure to free
 * \sa so_estring_new
 */
void so_estring_free(so_estring *self);
/** \memberof so_estring
 * Set the name of an estring element
 * \param self - pointer to an so_estring
 * \param name - A pointer to the string to set as name
 * \sa so_estring_get_name
 */

void so_estring_ref(so_estring *self);
void so_estring_unref(so_estring *self);

void so_estring_set_name(so_estring *self, char *name);
/** \memberof so_estring
 * Get the name of an estring element
 * \param self - pointer to an so_estring
 * \return A pointer to the name string. The string will be copied so it is the responsibility of the caller to free this if necessary.
 * \sa so_estring_set_name
 */
char *so_estring_get_name(so_estring *self);
/** \memberof so_estring
 * Set the content of an estring
 * \param self - pointer to an so_estring
 * \param string - A pointer to the string to set as content
 * \sa so_estring_get_string
 */
void so_estring_set_string(so_estring *self, char *string);
/** \memberof so_estring
 * Get the content of an estring
 * \param self - pointer to an so_estring
 * \return string - A pointer to the content string. The string will be copied so it is the responsibility of the caller to free this if necessary.
 * \sa so_estring_set_string
 */
char *so_estring_get_string(so_estring *self);

#endif
