#include <stdio.h>
#include "Itoa.h"

static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";

static void strreverse(char *begin, char *end)
{
	char aux;

	while (end > begin)
		aux = *end, *end-- = *begin, *begin++ = aux;
}

char *ltoa(long value, char *str, int base)
{
	char *wstr = str;
	int sign;

	// Validate base
	if (base < 2 || base > 35)
	{
		*wstr = '\0';
		return str;
	}

	// Take care of sign
	if ((sign = value) < 0)
		value = -value;

	// Conversion. Number is reversed.
	do
	{
		*wstr++ = num[value % base];
	} while (value /= base);

	if (sign < 0)
		*wstr++ = '-';

	*wstr = '\0';
	// Reverse string
	strreverse(str, wstr - 1);
	return str;
}

char *itoa(int value, char *str, int base)
{
	return ltoa(value, str, base);
}

char *ultoa(unsigned long value, char *str, int base)
{
	char *wstr = str;

	// Validate base
	if (base < 2 || base > 35)
	{
		*wstr = '\0';
		return str;
	}

	// Conversion. Number is reversed.
	do
	{
		*wstr++ = num[value % base];
	} while (value /= base);

	*wstr = '\0';
	// Reverse string
	strreverse(str, wstr - 1);
	return str;
}

char *utoa(unsigned int value, char *str, int base)
{
	return ultoa(value, str, base);
}

char *dtostrf(double value, signed char width, unsigned char prec, char *str)
{
	sprintf(str, "%.*f", prec, value);
	return str;
}
