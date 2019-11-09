//процедуры предобрабоки после демодулятора
#include "preprocessing.h"
#include <stdlib.h>
#include <iostream>

unsigned char i_prev_bit = 0;//предыдущий I-бит
unsigned char i_cur_bit = 0;//текущий I-бит
unsigned char q_prev_bit = 0;//предыдущий Q-бит
unsigned char q_cur_bit = 0;//текущий Q-бит

//разделяем поток на I и Q каждые 16 бит !!!!!!! МОЖНО СДЕЛАТЬ БЫСТРЕЕ, НЕ КОПИРУЯ МАССИВЫ, ЧЕРЕЗ УКАЗАТЕЛИ ВХОДНОГО ПОКА !!!!!!!
void get_iq(unsigned short *in_stream, long length, unsigned short *i, unsigned short *q) {
	for (long n = 0; n < length - 1; n++)
	{
		if (n & 1) q[n / 2] = in_stream[n];
		else i[n / 2] = in_stream[n];
		//i_data[i + 1] = bit_stream[i * 2 + 1];
		//q_data[i + 1] = bit_stream[i * 2 + 3];
	}
}
//декодирование NRZ-M
unsigned int *diff_decoding(long length, unsigned short *i, unsigned short *q) {
	unsigned int* out_data = (unsigned int*)malloc(sizeof(unsigned int)*(length / 2));
	unsigned short m_bit = 0;//маска номера бита
	memset(out_data, 0, length * 2);//заполняем поток нулями
	// используем двухбитное скользящее окно в параллель для I- и Q-потока !!!!!!!!!! МОЖНО СДЕЛАТЬ БЫСТРЕЕ !!!!!!!!!! через XOR и инструкции сопроцессора
	for (long n = 0; n < length * 8; n++)
	{
		m_bit = (unsigned short)((0x8000) >> (unsigned short)(n & 0xf));//маска номера бита слева от старшего к младшему
																		//пишем вычисленный I-бит 
		i_cur_bit = (unsigned short)(((i[n / 16]) & (m_bit)) == 0 ? 0 : 1);//определяем бит в байте
		if (i_cur_bit == i_prev_bit) out_data[n / 16] = (unsigned int)(out_data[n / 16] | (0 << (unsigned short)(31 - 2 * (n & 0xf))));//если не было перехода - ставим 0
		else out_data[n / 16] = (unsigned int)(out_data[n / 16] | (1 << (unsigned short)(31 - 2 * (n & 0xf))));//если был - ставим 1
																											   //пишем вычисленный Q-бит 
		q_cur_bit = (unsigned short)(((q[n / 16]) & (m_bit)) == 0 ? 0 : 1);//определяем бит в байте
		if (q_cur_bit == q_prev_bit) out_data[n / 16] = (unsigned int)(out_data[n / 16] | (0 << (unsigned short)(30 - 2 * (n & 0xf))));//если не было перехода - ставим 0
		else out_data[n / 16] = (unsigned int)(out_data[n / 16] | (1 << (30 - 2 * (unsigned short)(n & 0xf))));//если был - ставим 1

		i_prev_bit = i_cur_bit;//запоминаем нераскодированный I-бит для будущего сравнения
		q_prev_bit = q_cur_bit;//запоминаем нераскодированный Q-бит для будущего сравнения
	}
return out_data;
}
//Deinterleaving обратное перемежение
unsigned char *deinterleavingIQ(unsigned int *in_data, long length) {
	unsigned char *out_data = (unsigned char*)malloc(sizeof(unsigned char)*(length * 2));
	//структурируем поток канального уровня из двух I и Q
	for (long i = 0; i < length * 2; i += 4)//побайтно переворачиваем двойное слово
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
	//структурируем поток канального уровня из одного I или Q
	for (long i = 0; i < length; i += 2)//побайтно переворачиваем двойное слово
	{
		out_data[i] = (unsigned char)(in_data[i / 2] >> 8);
		out_data[i + 1] = (unsigned char)(in_data[i / 2]);
	}
	return out_data;
}
//сдвигаем поток на указанное число бит	
void rbit_shift(unsigned char *raw_data, long length, int n) {	
	unsigned char mask = 0x00;
	for (int i = 0; i < n; i++) {
		mask = (mask >> 1) + 0x80;
	}
	for (long i = 1; i < length; i++) {
		raw_data[i - 1] = (raw_data[i - 1] << n) | ((raw_data[i] & mask) >> (8 - n));
	}
}