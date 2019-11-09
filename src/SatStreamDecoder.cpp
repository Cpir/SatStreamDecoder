#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include <xmmintrin.h>//��� ��������� SSE2 � AVX
#include "fpf.h"
#include "preprocessing.h"


int main(int argc, char **argv)
{
	FILE *in_file, *out_file;//������ ������� � ��������� ������
	unsigned short* bit_stream, *i_data, *q_data;//��������� ������� ��������
	unsigned int* out_data;
	unsigned char* raw_data;	
	unsigned int n;
	char satt = 'a';
	if (argc>4) satt = argv[5][0];
	char path[MAX_PATH];//������ ����� �������� ��������
	char infilename[MAX_PATH];//������ ����� �������� �����

	GetCurrentDirectory(sizeof(path), path);//����� ����� ������� �����
	dircnt_t *dirptr = reed_dir(argv,path);//���������� ������� �����
	printf("Primary processing start==>\r\n");
	out_file = fopen("out.w32", "wb");

	for (n = 0; n < dirptr->num_images; n++) {//��� ���� ������� w32 ��������� ���� ����� �����
		sprintf(infilename, "%s/%s", path, dirptr->filename[n]);
		in_file = fopen(infilename, "rb");
		if (in_file == NULL) { printf("bad filename\r\n"); system("Pause"); return 0; }
		fseek(in_file, 0, SEEK_END);//������� ������ �����
		long length = ftell(in_file) / 2;//����� ����� � 16-�� ������ ������
		rewind(in_file);//���������� ��������� � ������ �����
		printf(dirptr->filename[n]);
		printf(" bitstream file reading %d bytes\r\n", length * 2);
		bit_stream = (unsigned short*)malloc(sizeof(unsigned short)*(length));//��������� ������� ������ 
		fread(bit_stream, sizeof(unsigned short), (length), in_file);//������ ���� � �����
		fclose(in_file);
		i_data = (unsigned short*)malloc(sizeof(unsigned short)*(length / 2));
		q_data = (unsigned short*)malloc(sizeof(unsigned short)*(length / 2));
		get_iq(bit_stream,length,i_data,q_data);//��������� ����� �� I � Q ������ 16 ��� !!!!!!! ����� ������� �������, �� ������� �������, ����� ��������� �������� ���� !!!!!!!
		free(bit_stream);		
		if(satt=='a') out_data = diff_decoding(length,i_data,q_data);//������������� NRZ-M !!!!!!!!!! ����� ������� ������� !!!!!!!!!! ����� XOR � ���������� ������������
		if (satt == 'a') raw_data = deinterleavingIQ(out_data,length);//Deinterleaving �������� �����������
		if (satt == 'a') {fwrite(raw_data, 1, length * 2, out_file); free(out_data); free(raw_data);}
		if (satt == 't') { raw_data = deinterleaving(i_data, length); /*rbit_shift(raw_data, length, 1);*/ fwrite(i_data, 1, length, out_file); }
		
		free(i_data);
		free(q_data);		
	}
	fclose(out_file);

	printf("RAW-data file created out.w32\r\n");
	printf("Secondary processing start==>\r\n");

	start(argc, argv);//��������� ��������� ��������� ����� framework

	printf("Planetary Data System file created out.w32.pds\r\n");
	system("Pause");
	return 0;
}