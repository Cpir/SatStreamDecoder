#include "windirent.h"

unsigned int get_num_images(char *imgdirpath);
int load_images(dircnt_t *dirptr, char *imgdirpath);
char * get_file_name(char *name);
dircnt_t *reed_dir(char **argv, char path[]); 