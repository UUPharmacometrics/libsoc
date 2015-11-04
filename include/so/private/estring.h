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
