#ifndef Itoa_h
#define Itoa_h

#ifdef __cplusplus
extern "C"{
#endif

char *itoa(int value, char *str, int base);
char *utoa(unsigned long value, char *str, int base);

char *ltoa(long value, char *str, int base);
char *ultoa(unsigned long value, char *str, int base);

char *dtostrf(double value, signed char width, unsigned char prec, char *str);

#ifdef __cplusplus
}
#endif

#endif
