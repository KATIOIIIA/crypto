// des.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define SHIFT_LEFT(x/*num*/, n/*shift*/, p/*p*/) ((((x) << (n))&((1<<p)-1)) | ((x) >> ((p) - (n))))

#define MEDIUM_BITS(x/*num*/, i /*�� ������� ����� � �����*/,len/*����� �����*/) ((x&((1<<(len-i))-1))>>i)
#define EDGE_BITS(x/*num*/, i /*�� ������� ����� � �����*/,len/*����� �����*/) (((x>>(len-i))<<i)|(x&((1<<i)-1)))

#define ROUNDS 2
//#define DEBUG

#include <string.h>

void printBin(unsigned _int64 x,unsigned nBits)
{
  unsigned _int64 mask =0, i;
  mask =9223372036854775808;
  for(i=1; i<=nBits; i++) {
	  printf("%c", x & mask ? '1' : '0');
      x <<= 1;
	  if (i%8==0) printf(" ");
  }
  printf("\n");
}

// ��������� ������� ������������
int tableIP[] = {
	58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};
	
// ����������� ������������ E - ����
int tableE[] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

// ������������ ��� ������������ �����
int tablePermutedChoice[] = {
	57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36, 
	63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
};
// ������������ � ������� ��� �����
int tableTranspositionWithCompression[] = {
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10, 23, 19, 12, 4,
	26,  8, 16,  7, 27, 20, 13,  2, 41, 52, 31, 37, 47, 55, 30, 40,
	51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

// S - �����
int tableS[8][64] = {
	{
		14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, //16
		0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8, //32
		4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0, //48
		15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
	},
	{
		15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10, 
		3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5, 
		0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15, 
		13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
	},
	{
		10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8, 
		13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
		13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7, 
		1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
	},
	{
		7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15, 
		13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
		10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4, 
		3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
	},
	{ 
		2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9, 
		14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
		4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14, 
		11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
	},
	{
		12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11, 
		10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
		9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6, 
		4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
	},
	{
		4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1, 
		13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
		1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2, 
		6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
	},
	{
		13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7, 
		1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
		7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8, 
		2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
	}
};

// P �����
int tableP[]={
		16, 7, 20, 21, 29, 12, 28, 17,
		1, 15, 23, 26, 5, 18, 31, 10,
		2, 8, 24, 14, 32, 27, 3, 9,
		19, 13, 30, 6, 22, 11, 4, 25
};

// �������������� ������������
int tableIPBack[]={
	40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25
};

// ��� ��������
int stepShiftLeftKey[]  = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
// ��� ����������
int stepShiftRightKey[] = {0, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// 
// 1) ������ ��� ���� �������� 64 �� ��������� ������� ������������ IP
// 2) ����������� ������������ (E - ����) � �.�.
unsigned _int64 MIX(unsigned _int64 x, int len, int bitsCount,int mixTable[])
{
	int i=0;
	unsigned _int64 res=0;
	for (i=0;i<len;i++)
	{
		res=res<<1;
		res=res|((x>>(bitsCount-mixTable[i]))&1);
	}
	return res;
}
unsigned _int64 delete8bit(unsigned _int64 x, unsigned int len)
{
	unsigned _int64 res=0;
	int i=0;
	
	unsigned _int64 mask =1;
	for (i=len;i>=0;i--)
	{
		if (i%8!=0)
			res=((res<<1)|((x>>(i-1))&1));
	}
	return res;
}

unsigned _int64 Encryption(unsigned _int64 M, unsigned _int64 key[],bool EorD)
{
	// ��������� ������������
	unsigned _int64 X = MIX(M,64,64,tableIP);

	// ��������� 64 ���������� ���� �� 32; 
	unsigned _int64	 L = X >> (32); 
	unsigned _int64 mask =1;
	mask=(mask<<32)-1;
	unsigned _int64 R = X & mask; 

	// ������� ����
	unsigned _int64 C=0;
	unsigned _int64 P=0,R1=0,L1=0;
	#ifdef DEBUG  	
		printf("M: \n");
		printBin(M,64);
		printf("X: \n");
		printBin(X,64);
		printf("L: \n");	
		printBin(L,64);
		printf("R: \n");
		printBin(R,64);
	#endif
	// ������� ������ 8 ���, � ����� �������� 56 ������ ���� �0
	for (int j=0;j<ROUNDS;j++)
	{	
		C=0;
		P=0;
		R1=0;
		#ifdef DEBUG  
			printf("\n\n\nKEY: \n");printBin(key[j],64); 
		#endif
		// ������ ����� ��������� �� 48 ���:
		unsigned _int64 E = MIX(R,48,32,tableE); 
		#ifdef DEBUG  
			printBin(E,64);
			printf("E: \n");
		#endif
		// XOR�� ���� � ���� ������:
		unsigned _int64 B;
		// ���� �������, �� ����� K1, K2, .., � ���� ���������, �� K16, K15,..
		if (EorD) B = E ^(key[j]);
		else B = E ^(key[ROUNDS-j-1]);
		#ifdef DEBUG  
			printf("B: \n");  
			printBin(B,64);
		#endif
		// ------------------------------
		// ������ � S - �������
		// ------------------------------
	
		// ����� ���� B �� 8 ���:
		// 32 ������ ����� ����� S ������
		int i;
		for (i=1;i<=8;i++)
		{
			mask=1;
			mask=(mask<<(6*(8-i+1)))-1;
			unsigned _int64 B0 =(B&mask)>>(48-(i*6));
			// ������� 1 � ��������� ���: - ����� ������� � S �����
			unsigned _int64 edgeB0=EDGE_BITS(B0,1,6);
			// ������� 6 ������� �����: - ����� ������ � S �����
			unsigned _int64 mediumB0=MEDIUM_BITS(B0,1,6)+1;
			unsigned _int64 Res =tableS[i-1][(edgeB0)*16-1+mediumB0]; 
			#ifdef DEBUG  
				printf("B0: \n"); 
				printBin(B0,64);
				printf("i:  %d",i-1);
				printf("(%d,",mediumB0);
				printBin(mediumB0,64);
				printf("%d) = ",edgeB0);
				printBin(edgeB0,64);
				printf("%d\n",Res); 
			#endif
			C=(C<<4)|Res;
		}
		#ifdef DEBUG  
			printf("C: \n"); 
			printBin(C,64);
		#endif

		//------------------------------------------------------------------------------------
		// ������������ ����� P �����
		P=MIX(C,32,32,tableP);
		L1=R;
		// XOR � ����� ����� �������������� ������
		R= L ^ P;
		L=L1;
		#ifdef DEBUG  
			printf("P: \n"); 
			printBin(P,64);
			printf("R1: %d\n",R); 
			printBin(R,64);
			printf("L1: %d\n",L); 
			printBin(L,64);
		#endif
	}
	// �������������� ������������
	unsigned _int64 Result;
	Result=MIX((R<<32)|L,64,64,tableIPBack);
	return Result;
}


void generationKeys(unsigned _int64 key, unsigned _int64 keys[])
{
	// ----------------------------------------------------------------------
	// ����
	// ��.������: key - ����; keys[] - ����� ��� ������� �����;
	// C0, D0: �0D0 - 56 ������ ����� ����� ��� 8 �����
	// ���.������: 
	//------------------------------------------------------------------------

	unsigned _int64 mask;
	// ������� ������ 8 ���, � ����� �������� 56 ������ ���� �0
	unsigned _int64 K0=MIX(key,56,64,tablePermutedChoice); 
	for (int nCycle=0;nCycle<ROUNDS;nCycle++)
	{
		// ����� �� 28
		unsigned _int64 C0 = K0 >> (28); 
		mask =1;
		mask=(mask<<28)-1;
		unsigned _int64 D0 = K0 & mask; 
		// ���������� �������� �� 1 ��� 2 ���� � ����������� �� ������ ����� S
		unsigned _int64 C1;
		unsigned _int64 D1; 
		C1 = SHIFT_LEFT(C0,	stepShiftLeftKey[nCycle],28); 
		D1 = SHIFT_LEFT(D0,	stepShiftLeftKey[nCycle],28);
		// ��������� ������ C1D1:
		unsigned _int64 K01=(C1<<28)|D1; 
		// ��������� ������������ �� �������
		keys[nCycle] = MIX(K01,48,56,tableTranspositionWithCompression);
		#ifdef DEBUG  
			printf("C0: \n"); printBin(C0,64);
			printf("D0: \n"); printBin(D0,64);
			printBin(C1,64);
			printBin(D1,64);
			printBin(C1,64);
			printBin(D1,64);
			printBin(K01,64);
			printf("K[i]: \n"); 
			printBin(keys[nCycle],64);
		#endif
		K0 = (C1<<28)|D1;
	}
	return;
}

bool EncryptionFile(char *inFileName,char *outFileName, unsigned _int64 keys[], bool EorD)
{
	FILE *fIn=NULL,*fOut=NULL;
	if (!(fIn  = fopen(inFileName,"rb"))) return false;
	if (!(fOut = fopen(outFileName,"wb"))) return false;
	while (!feof(fIn))
	{	
		unsigned _int64 text=0;
		fread(&text,8,1,fIn);
		unsigned _int64 res=Encryption(text,keys,EorD); 
		fwrite(&res,8,1,fOut);
	}
	fclose(fIn);
	fclose(fOut);
	return true;
}


int _tmain(int argc, _TCHAR* argv[])
{
	
	unsigned _int64 keys[ROUNDS]={0};
	//unsigned _int64 key = 8311768687351637302;
	unsigned _int64 key = 831176;
	generationKeys(key,keys);

	bool status = EncryptionFile("crypt/text1.txt","crypt/etext1.txt",keys,true);
	
	 EncryptionFile("crypt/text2.txt","crypt/etext2.txt",keys,true);
	EncryptionFile("crypt/text3.txt","crypt/etext3.txt",keys,true);
	
EncryptionFile("crypt/text4.txt","crypt/etext4.txt",keys,true);
	
EncryptionFile("crypt/text5.txt","crypt/etext5.txt",keys,true);

	

//	bool status = EncryptionFile("img.bmp","img1.bmp",keys,true);
//	if (status) EncryptionFile("img1.bmp","img2.bmp",keys,false);
	
//	bool status = EncryptionFile("���� ����.mp3","���� ����EC.mp3",keys,true);
//	if (status) EncryptionFile("���� ����EC.mp3","���� ����2.mp3",keys,false);






	return 0;
}

