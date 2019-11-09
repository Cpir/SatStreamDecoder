//��������� ������������ ����� ������������
#include "preprocessing.h"
#include <stdlib.h>
#include <iostream>

unsigned char i_prev_bit = 0;//���������� I-���
unsigned char i_cur_bit = 0;//������� I-���
unsigned char q_prev_bit = 0;//���������� Q-���
unsigned char q_cur_bit = 0;//������� Q-���

//��������� ����� �� I � Q ������ 16 ��� !!!!!!! ����� ������� �������, �� ������� �������, ����� ��������� �������� ���� !!!!!!!
void get_iq(unsigned short *in_stream, long length, unsigned short *i, unsigned short *q) {
	for (long n = 0; n < length - 1; n++)
	{
		if (n & 1) q[n / 2] = in_stream[n];
		else i[n / 2] = in_stream[n];
		//i_data[i + 1] = bit_stream[i * 2 + 1];
		//q_data[i + 1] = bit_stream[i * 2 + 3];
	}
}
//������������� NRZ-M
unsigned int *diff_decoding(long length, unsigned short *i, unsigned short *q) {
	unsigned int* out_data = (unsigned int*)malloc(sizeof(unsigned int)*(length / 2));
	unsigned short m_bit = 0;//����� ������ ����
	memset(out_data, 0, length * 2);//��������� ����� ������
	// ���������� ���������� ���������� ���� � ��������� ��� I- � Q-������ !!!!!!!!!! ����� ������� ������� !!!!!!!!!! ����� XOR � ���������� ������������
	for (long n = 0; n < length * 8; n++)
	{
		m_bit = (unsigned short)((0x8000) >> (unsigned short)(n & 0xf));//����� ������ ���� ����� �� �������� � ��������
																		//����� ����������� I-��� 
		i_cur_bit = (unsigned short)(((i[n / 16]) & (m_bit)) == 0 ? 0 : 1);//���������� ��� � �����
		if (i_cur_bit == i_prev_bit) out_data[n / 16] = (unsigned int)(out_data[n / 16] | (0 << (unsigned short)(31 - 2 * (n & 0xf))));//���� �� ���� �������� - ������ 0
		else out_data[n / 16] = (unsigned int)(out_data[n / 16] | (1 << (unsigned short)(31 - 2 * (n & 0xf))));//���� ��� - ������ 1
																											   //����� ����������� Q-��� 
		q_cur_bit = (unsigned short)(((q[n / 16]) & (m_bit)) == 0 ? 0 : 1);//���������� ��� � �����
		if (q_cur_bit == q_prev_bit) out_data[n / 16] = (unsigned int)(out_data[n / 16] | (0 << (unsigned short)(30 - 2 * (n & 0xf))));//���� �� ���� �������� - ������ 0
		else out_data[n / 16] = (unsigned int)(out_data[n / 16] | (1 << (30 - 2 * (unsigned short)(n & 0xf))));//���� ��� - ������ 1

		i_prev_bit = i_cur_bit;//���������� ����������������� I-��� ��� �������� ���������
		q_prev_bit = q_cur_bit;//���������� ����������������� Q-��� ��� �������� ���������
	}
return out_data;
}
//Deinterleaving �������� �����������
unsigned char *deinterleavingIQ(unsigned int *in_data, long length) {
	unsigned char *out_data = (unsigned char*)malloc(sizeof(unsigned char)*(length * 2));
	//������������� ����� ���������� ������ �� ���� I � Q
	for (long i = 0; i < length * 2; i += 4)//�������� �������������� ������� �����
	{
		out_data[i] = (unsigned char)(in_data[i / 4] >> 24);
		out_data[i + 1] = (unsigned char)(in_data[i / 4] >> 16);
		out_data[i + 2] = (unsigned char)(in_data[i / 4] >> 8);
		out_data[i + 3] = (unsigned char)(in_data[i / 4]);
	}
	return out_data;
}
unsigned char *deinterleaving(unsigned short *in_data, long length) {
	unsigned char *out_data = (unsigned char*)malloc(sizeof(unsigned char)*(length * 2));
	//������������� ����� ���������� ������ �� ������ I ��� Q
	for (long i = 0; i < length; i += 2)//�������� �������������� ������� �����
	{
		out_data[i] = (unsigned char)(in_data[i / 2] >> 8);
		out_data[i + 1] = (unsigned char)(in_data[i / 2]);
	}
	return out_data;
}
//�������� ����� �� ��������� ����� ���	
void rbit_shift(unsigned char *raw_data, long length, int n) {	
	unsigned char mask = 0x00;
	for (int i = 0; i < n; i++) {
		mask = (mask >> 1) + 0x80;
	}
	for (long i = 1; i < length; i++) {
		raw_data[i - 1] = (raw_data[i - 1] << n) | ((raw_data[i] & mask) >> (8 - n));
	}
}