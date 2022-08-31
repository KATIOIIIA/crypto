// rc6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include <iostream>
using namespace std;

#define SHIFT_RIGHT(x/*num*/, n/*shift*/) ((x>>(n))|(x<<(32-n)))
#define SHIFT_LEFT(x/*num*/, n/*shift*/) ((x<<(n))|(x>>(32-n)))
#define R 20
#define W 32
unsigned short m_dwS[2*R + 3] = {0};

unsigned short OffsetAmount(unsigned short dwVar)
{
	int nLgw = (int)(log((double)W)/log(2.0));
	dwVar = dwVar << (W - nLgw);
	dwVar = dwVar >> (W - nLgw);
	return dwVar;
}
 

void KeyGen(unsigned short  dwKey)
{
	unsigned short PW = ((exp(1.0)-2)*(1<<(W)));			//для 32 = 0xB7E15163;
    unsigned short QW =( (((sqrt(5.0)+1)/2)-1)*(1<<(W)));	//для 32 = 0x9E3779B9;
    unsigned short i, A, B;
    unsigned short dwByteOne, dwByteTwo, dwByteThree, dwByteFour;

    dwByteOne = dwKey >> 24;
    dwByteTwo = dwKey >> 8;
    dwByteTwo = dwByteTwo & 0x0010;
    dwByteThree = dwKey << 8;
    dwByteThree = dwByteThree & 0x0100;
    dwByteFour = dwKey << 24;

    dwKey = dwByteOne|dwByteTwo|dwByteThree|dwByteFour;

    m_dwS[0] = PW;

    for(i=1; i<2*R+4; i++) m_dwS[i] = m_dwS[i - 1] + QW;
	i = A = B = 0;
	int v = 3*max(1, 2*R+4);
	for(int s = 1; s <= v; s++)
    {
        A = m_dwS[i] =SHIFT_LEFT(m_dwS[i] + A + B,OffsetAmount(3));
        B = dwKey = SHIFT_LEFT(dwKey + A + B, OffsetAmount(A + B));
		i = (i + 1) % (2 * R + 4);
    }
}


unsigned short Encoder(unsigned short data)
{
	unsigned short *pdwTemp= (unsigned short*)&data;
	/*
	A = pdwTemp[0]
	B = pdwTemp[1]
	C = pdwTemp[2]
	D = pdwTemp[3]
	*/
	pdwTemp[1] =pdwTemp[1] + m_dwS[0];
    pdwTemp[3] = pdwTemp[3] + m_dwS[1];
	unsigned short logw = OffsetAmount((unsigned short)(log(W*1.0)/log(2.0)));
	for(int i = 1; i <= R; i++)
    {
		unsigned short t = SHIFT_LEFT((pdwTemp[1]*(2*pdwTemp[1] + 1)),logw);
        unsigned short u = SHIFT_LEFT((pdwTemp[3]*(2*pdwTemp[3] + 1)) ,logw);
        pdwTemp[0] = (SHIFT_LEFT((pdwTemp[0] ^ t) , OffsetAmount(u))) + m_dwS[2*i];
        pdwTemp[2] = (SHIFT_LEFT((pdwTemp[2] ^ u) ,OffsetAmount(t))) + m_dwS[2*i + 1] ;
        (pdwTemp[0], pdwTemp[1], pdwTemp[2], pdwTemp[3])  =  (pdwTemp[1], pdwTemp[2], pdwTemp[3], pdwTemp[0]);
	}
    pdwTemp[0] = pdwTemp[0] + m_dwS[2*R + 2];
    pdwTemp[2] = pdwTemp[2] + m_dwS[2*R + 3];
	return *pdwTemp;
}

unsigned short  Decoder(unsigned short data)
{
    unsigned short *pdwTemp= (unsigned short*)&data;
	pdwTemp[2] = pdwTemp[2] - m_dwS[2*R + 3];
    pdwTemp[0] = pdwTemp[0] - m_dwS[2*R + 2];
	unsigned short logw = OffsetAmount((unsigned short)(log(W*1.0)/log(2.0)));
	for(int i = R; i >= 1; i--)
    {
        (pdwTemp[0], pdwTemp[1], pdwTemp[2], pdwTemp[3]) = (pdwTemp[3], pdwTemp[0], pdwTemp[1], pdwTemp[2]);
        unsigned short u = SHIFT_LEFT((pdwTemp[3]*(2*pdwTemp[3] + 1)) ,logw);
        unsigned short t = SHIFT_LEFT((pdwTemp[1]*(2*pdwTemp[1] + 1)) ,logw);
        pdwTemp[2] = (SHIFT_RIGHT((pdwTemp[2] - m_dwS[2*i + 1]) , OffsetAmount(t))) ^ u;
        pdwTemp[0] = (SHIFT_RIGHT((pdwTemp[0] - m_dwS[2*i]), OffsetAmount(u))) ^ t;
    }
    pdwTemp[3] = pdwTemp[3] - m_dwS[1];
    pdwTemp[1] = pdwTemp[1] - m_dwS[0];
	return *pdwTemp;
}

bool EncryptionFile(char *inFileName,char *outFileName,bool EorD)
{
	FILE *fIn = NULL, *fOut = NULL;
	if (!(fIn  = fopen(inFileName,"rb"))) return false;
	if (!(fOut = fopen(outFileName,"wb"))) return false;
	while (!feof(fIn))
	{
		unsigned short text=0;
		fread(&text,2,1,fIn);
		unsigned short res;
		if (EorD) res = Encoder(text);
		else res = Decoder(text);
		fwrite(&res,2,1,fOut);
	}
	fclose(fIn);
	
	fclose(fOut);
	return true;
}


int _tmain(int argc, _TCHAR* argv[])
{
	KeyGen(123);
	/*
	unsigned short data =22345;
	printf("%ld\n",data);
	unsigned short res = Encoder(data);
	printf("%ld\n",res);
	KeyGen(123);
	unsigned short res2 = Decoder(res);
	printf("%ld\n",res2);
	*/
	printf("Encoder...");
	EncryptionFile("data/renuar.jpg","data/renuar_encoder.jpg",true);
	printf("\nDecoder...");
    EncryptionFile("data/renuar_encoder.jpg","data/renuar_decoder.jpg",false);
	printf("\n");
	return 0;
}

