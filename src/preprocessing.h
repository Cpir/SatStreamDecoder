//Процедуры предобработки после демодулятора
void get_iq(unsigned short *in_stream, long length, unsigned short *i, unsigned short *q); 
unsigned int *diff_decoding(long length, unsigned short *i, unsigned short *q);
unsigned char *deinterleavingIQ(unsigned int *in_data, long length);
unsigned char *deinterleaving(unsigned short *in_data, long length);
void rbit_shift(unsigned char *raw_data, long length, int n);