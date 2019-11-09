#include "file.h"
//определяем число файлов в директории
unsigned int get_num_images(char *imgdirpath)
{
 DIR *dir;
 struct dirent* content;
 unsigned int num_images = 0;
 dir = opendir(imgdirpath);
 if (!dir) {
  fprintf(stderr, "Could not open Folder %s\n", imgdirpath);
  return 0;
 }
 num_images = 0;
 while ((content = readdir(dir)) != NULL) {
  if (strcmp(".", content->d_name) == 0 || strcmp("..", content->d_name) == 0) {
   continue;
  }
  num_images++;
 }
 closedir(dir);
 return num_images;
}

int load_images(dircnt_t *dirptr, char *imgdirpath)
{
 DIR *dir;
 struct dirent* content;
 int i = 0;
 /*читаем субдиректорию*/
 dir = opendir(imgdirpath);
 if (!dir) {
  fprintf(stderr, "Could not open Folder %s\n", imgdirpath);
  return 1;
 }
 else {
  fprintf(stderr, "Folder opened successfully\n");
 }
 while ((content = readdir(dir)) != NULL) {
  if (strcmp(".", content->d_name) == 0 || strcmp("..", content->d_name) == 0) {
   continue;
  }
  strcpy(dirptr->filename[i], content->d_name);
  i++;
 }
 closedir(dir);
 return 0;
}


char * get_file_name(char *name)
{
 char *fname = strtok(name, ".");
 return fname;
}

dircnt_t *reed_dir(char **argv, char path[])
{
	unsigned int n;
	img_fol_t img_fol;//Папки и имена исходного и конечного файлов
	memset(&img_fol, 0, sizeof(img_fol_t));
	char * p = strchr(path, '\0');//указатель на начало строки адреса
	if (path < p  &&
		*(p - 1) != '\\'  &&  *(p - 1) != '/'  &&  *(p - 1) != ':') {
		strcpy(p++, "\\");
	}
	strcpy(p, argv[4]);//добавляем в адрес субдиректорию с входными файлами
	img_fol.imgdirpath = (char *)path;
	dircnt_t *dirptr = (dircnt_t*)malloc(sizeof(dircnt_t));//структурка с атрибутами файла
	dirptr->num_images = get_num_images(img_fol.imgdirpath);//число входных файлов
	if (dirptr) {
		dirptr->filename_buf = (char*)malloc(dirptr->num_images * 4096 * sizeof(char));
		dirptr->filename = (char**)malloc(dirptr->num_images * sizeof(char*));
		if (!dirptr->filename_buf) { return 0; }
		for (n = 0; n < dirptr->num_images; n++) {
			dirptr->filename[n] = dirptr->filename_buf + n * 4096;//читаем имена всех файлов в субдиректории
		}
	}
	if (load_images(dirptr, img_fol.imgdirpath) == 1) { return 0; }//если нет файлов в субдиректории - уходим
	if (dirptr->num_images == 0) {
		fprintf(stdout, "Folder is empty\n");
		return 0;
	}
	return dirptr;
}