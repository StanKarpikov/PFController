#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "Protocol.h"

//int __stdcall DllMain()
//{
//	return 0;
//}

/*
int main() {
	int i;
	float adc[9];
	short oscilog[64];
	DBG("Connect %d\n", Protocol::Connect());
	DBG("Test %d\n", Protocol::Test());

	if (Protocol::GetAdcActive(adc, 9)) {
		for (i = 0; i < 9; i++) {
			printf("%f ", adc[i]);
		}
		printf("\n");
	} else {
		DBG("GetAdcActive failed\n");
	}

	if (Protocol::GetOscilog(0, oscilog, 64)) {
		for (i = 0; i < 64; i++) {
			DBG("%d ", oscilog[i]);
		}
		DBG("\n");
	} else {
		DBG("GetOscilog failed\n");
	}

	Protocol::Disconnect();
	getchar();
	return 0;
}
*/
