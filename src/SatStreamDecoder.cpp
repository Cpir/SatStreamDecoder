#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include <xmmintrin.h>//для поддержки SSE2 и AVX
#include "fpf.h"
#include "preprocessing.h"


int main(int argc, char **argv)
{
	FILE *in_file, *out_file;//потоки входных и выходного файлов
	unsigned short* bit_stream, *i_data, *q_data;//указатели рабочих массивов
	unsigned int* out_data;
	unsigned char* raw_data;	
	unsigned int n;
	char satt = 'a';
	if (argc>4) satt = argv[5][0];
	char path[MAX_PATH];//полный адрес рабочего каталога
	char infilename[MAX_PATH];//полный адрес входного файла

	GetCurrentDirectory(sizeof(path), path);//узнаём адрес рабочей папки
	dircnt_t *dirptr = reed_dir(argv,path);//определяем входные файлы
	printf("Primary processing start==>\r\n");
	out_file = fopen("out.w32", "wb");

	for (n = 0; n < dirptr->num_images; n++) {//для всех входных w32 формируем один сырой поток
		sprintf(infilename, "%s/%s", path, dirptr->filename[n]);
		in_file = fopen(infilename, "rb");
		if (in_file == NULL) { printf("bad filename\r\n"); system("Pause"); return 0; }
		fseek(in_file, 0, SEEK_END);//сичтаем размер файла
		long length = ftell(in_file) / 2;//длина файла в 16-ти битных словах
		rewind(in_file);//возвращаем указатель в начало файла
		printf(dirptr->filename[n]);
		printf(" bitstream file reading %d bytes\r\n", length * 2);
		bit_stream = (unsigned short*)malloc(sizeof(unsigned short)*(length));//указатель позиции потока 
		fread(bit_stream, sizeof(unsigned short), (length), in_file);//читаем файл в поток
		fclose(in_file);
		i_data = (unsigned short*)malloc(sizeof(unsigned short)*(length / 2));
		q_data = (unsigned short*)malloc(sizeof(unsigned short)*(length / 2));
		get_iq(bit_stream,length,i_data,q_data);//разделяем поток на I и Q каждые 16 бит !!!!!!! МОЖНО СДЕЛАТЬ БЫСТРЕЕ, НЕ КОПИРУЯ МАССИВЫ, ЧЕРЕЗ УКАЗАТЕЛИ ВХОДНОГО ПОКА !!!!!!!
		free(bit_stream);		
		if(satt=='a') out_data = diff_decoding(length,i_data,q_data);//декодирование NRZ-M !!!!!!!!!! МОЖНО СДЕЛАТЬ БЫСТРЕЕ !!!!!!!!!! через XOR и инструкции сопроцессора
		if (satt == 'a') raw_data = deinterleavingIQ(out_data,length);//Deinterleaving обратное перемежение
		if (satt == 'a') {fwrite(raw_data, 1, length * 2, out_file); free(out_data); free(raw_data);}
		if (satt == 't') { raw_data = deinterleaving(i_data, length); /*rbit_shift(raw_data, length, 1);*/ fwrite(i_data, 1, length, out_file); }
		
		free(i_data);
		free(q_data);		
	}
	fclose(out_file);

	printf("RAW-data file created out.w32\r\n");
	printf("Secondary processing start==>\r\n");

	start(argc, argv);//запускаем вторичную обработку через framework

	printf("Planetary Data System file created out.w32.pds\r\n");
	system("Pause");
	return 0;
}