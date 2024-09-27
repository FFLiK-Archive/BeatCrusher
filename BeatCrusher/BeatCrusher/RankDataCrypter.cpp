#include "RankDataCrypter.h"

int IntDataEncrypter(int data) {
	data ^= CODE;
	data *= 2;
	data -= CODE;
	data ^= CODE;
	return data;
}

int IntDataDecrypter(int data) {
	data ^= CODE;
	data += CODE;
	if (data % 2)
		return 0;
	data /= 2;
	data ^= CODE;
	return data;
}
