// aes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
using namespace std;
#define SHIFT_RIGHT(x/*num*/, n/*shift*/) ((x>>(n))|(x<<(32-n)))
#define SHIFT_LEFT(x/*num*/, n/*shift*/) ((x<<(n))|(x>>(32-n)))
// ����� �������
#define Nr 10
// ����� ��������
#define Nb 4



unsigned S[256] = {0};
unsigned invertS[256]={0};
unsigned poly[4] = {0};
unsigned invertPoly[4] = {0};
unsigned roundConst[15]={0};
// ----------- ���������� S-�����
// ��������� ���������
unsigned multPoly(unsigned poly1, unsigned poly2)
{
	unsigned res = 0;
	while (poly2)
	{
		if (poly2&1) res^=poly1;
		poly1<<=1;
		poly2>>=1;
	}
	return res;
}
// ��������� ����� ������ ��������
unsigned lenPoly(unsigned x)
{
	int pow=0;
	for (;x;x>>=1) pow++;
	//do pow++; while(num>>=1);
	return pow;
}
// ������� ���������
unsigned divPoly(unsigned num1, unsigned num2)
{
	unsigned res = 0;
	unsigned tmp = 0, mult = 0;
	unsigned lenNum2 = lenPoly(num2);
	while (lenPoly(num1)>=lenPoly(num2)) 
	{
		tmp = num2;
		mult = 1;
		while (lenPoly(num1)>lenPoly(tmp))
		{
			tmp<<=1;
			mult<<=1;
		}
		res^=mult;
		num1^=tmp;
	}
	return res;
}
// �������� ���� ����� �� ����
unsigned multNum(unsigned num1, unsigned num2)
{
	unsigned res = 0;
	while (num2)
	{
		if (num2&1) res^=num1;
		num1<<=1;
		if (num1 & 0x100) num1^=0x11b;
		num2>>=1;
	}
	return res;
}
// ������ ������� 1 (�������������)
unsigned Euclid (unsigned a, unsigned b, long *x,long *y)
{
	unsigned x2=1,x1=0,y2=0,y1=1,r=0,q=0;
	while(b)
	{
		q = divPoly(a,b);
		r = a^multPoly(q,b);
		*x = x2^multPoly(q,x1);
		*y = y2^multPoly(q,y1);
		a = b;
		b = r;
		x2 = x1;
		x1 = *x;
		y2 = y1;
		y1 = *y;
	}
	*x = x2;
	*y = y2;
	return a;
}
// ������ ������� 2 (�����������)
unsigned gcd (unsigned a, unsigned b, int &x, int &y)
{
	if (a==0)
	{
		x = 0;
		y = 1;
		return b;
	}
	int x1,y1;
	unsigned mod = b^multPoly(divPoly(b,a),a); // ������� �� �������
	unsigned d = gcd (mod,a,x1,y1);
	x = y1 ^ multPoly(divPoly(b,a),x1);
	y = x1;
	return d;
}
// ����������������� �������� � num
unsigned multiplicativeInverse(unsigned num)
{
	//long a,b;
	//Euclid(num,0x11b,&a,&b);
	int a=0,b;
	gcd(num,0x11b,a,b);
	return a;
}
// xor'�� ��� ����
unsigned xorBits(unsigned x, unsigned len)
{
	for (len>>=1; x>>1; len>>=1)
		x = (x>>len)^x & ((1<<len)-1);
	return x;
}


// ������� ������������� r = A*b^{-1}^off, ��� off = 0x63
unsigned afinTransformation(unsigned b)
{
	unsigned r = 0;
	unsigned A[] = {0x8F,0xC7,0xE3,0xF1,0xF8,0x7C,0x3E,0x1F};
	b = multiplicativeInverse(b);
	for (int i = 0; i<8; i++)
	{
		r=(r<<1)+xorBits(A[i]&b,8);
	}
	r^=0x63;
	return r;
}
// S[i] = A*i^{-1}+off = afinTransform(i);
void createSbox()
{
	for (int i = 0; i<256; i++)
	{
		S[i]=afinTransformation(i);
		invertS[S[i]]=i;
	}
	poly[0]=0x3010102; // ��������� ��������� 3��� �������
	invertPoly[0]=0xb0d090e; // �����������
	for (int i=1;i<Nb;i++)
	{
		poly[i]=SHIFT_RIGHT(poly[i-1],8);
		invertPoly[i]=SHIFT_RIGHT(invertPoly[i-1],8);
	}
	// �������������� ��������� ��� ������:
	char *pRoundConst=(char*)roundConst;
	char *pTmp = pRoundConst;
	*pRoundConst = 1;
	for (int i=1;i<15;i++)
	{
		pRoundConst+=4;
		*pRoundConst=multNum(*pTmp,2);
		pRoundConst[1]=pRoundConst[2]=pRoundConst[3]=0;
		pTmp=pRoundConst;
	}
}
// ----------- 

// ----------- ��������� �����
unsigned keyExp[Nb*(Nr+1)]= {0};

// ����� 4� �������� ������� ����� � ��������� S���� � ������� �� 4�������
unsigned subWord(unsigned x)
{
	char *p = (char*)&x;
	for (int i=0;i<Nb;i++) p[i]=S[p[i]];
	return x;
}


void keyExpansion(unsigned *key)
{
	unsigned tmp=0;
	for (int i = 0;i<Nb;i++) keyExp[i]=key[i];
	for (int i = Nb;i<Nb*(Nr+1);i++)
	{
		tmp = keyExp[i-1];
		if (i%Nb) tmp = subWord(SHIFT_RIGHT(tmp,24))^roundConst[i/Nb];
		keyExp[i]=keyExp[i-Nb]^tmp;
	}
}

// ----------- 

//������������ state, ��������� S �����, �������� �� ���������� � ������� �����
void subBytes(unsigned char *state, unsigned s[])
{
	for (int i=0; i<16; i++) state[i]=s[state[i]];
}

// ������������ state, ���������� ������ ��������� ��� ������ state �� ������ ��������
void shiftRow(unsigned *state, int bInvert)
{
	if (bInvert) 
		for (int i=1;i<Nb;i++) state[i]=SHIFT_LEFT(state[i],i<<3);
	else
		for (int i=Nb-1;i;i--) state[i]=SHIFT_RIGHT(state[i],i<<3);

}

// ����� ��� ������� state � ��������� �� ������ (���������� ���� �� �����)
void mixColumn(unsigned *pS, unsigned *poly)
{
	unsigned r=0;
	char *pR=(char*)&r, *pPoly=NULL, *pM=NULL;
	for (int i=0; i<Nb; i++)
	{
		pPoly=(char*)(pS+i);
		for (int j=0; j<Nb; j++)
		{
			pM = (char*)(poly+j);
			pR[j]=multNum(pPoly[0],pM[0])^multNum(pPoly[1],pM[3])^multNum(pPoly[2],pM[2])^multNum(pPoly[3],pM[1]);
		}
		pS[i]=r;
	}	
}

// roundKey xor'���� � state
void addRoundKey(unsigned *state, unsigned *key)
{
	for (int i=0;i<Nb;i++) state[i]^=key[i];
}

	FILE *fIn=NULL,*fOut=NULL;
// ����������
void Encryption(unsigned char *text)
{
	unsigned *pText = (unsigned *) text;
	addRoundKey(pText,keyExp);
	// ������ �������
	for (int i=1; i<Nr; i++)
	{
		subBytes(text,S);
	
		shiftRow(pText,0);
		mixColumn(pText,poly);
		addRoundKey(pText,keyExp+i*Nb);
	}
	subBytes(text,S);
	shiftRow(pText,0);//printf("aaround = %s  \n",text);
	addRoundKey(pText,keyExp+Nr*Nb);
}
// ���������� (�������� �������� � ��������
void Decryption(unsigned char *text)
{
	unsigned *pText = (unsigned *) text;
	addRoundKey(pText,keyExp+Nb*Nr);
	shiftRow(pText,1);
	subBytes(text,invertS);
	// ������ �������
	for (int i=Nr-1; i; i--)
	{
		addRoundKey(pText,keyExp+i*Nb);
		mixColumn(pText,invertPoly);
		shiftRow(pText,1);
		subBytes(text,invertS);		
	}
	addRoundKey(pText,keyExp);
}


bool EncryptionFile(char *inFileName,char *outFileName,bool EorD)
{
	if (!(fIn  = fopen(inFileName,"rb"))) return false;
	if (!(fOut = fopen(outFileName,"wb"))) return false;
	while (!feof(fIn))
	{
		unsigned char text[16]={0};
		fread(&text,16,1,fIn);
		if (EorD) Encryption(text);
		else Decryption(text);
		fwrite(text,16,1,fOut);
	}
	fclose(fIn);
	
	fclose(fOut);
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	createSbox();
	
	unsigned key = 123;
	keyExpansion(&key);
	
	printf("%d\n",keyExp);
//EncryptionFile("data/text.txt","data/text2.txt",true);
//EncryptionFile("data/text2.txt","data/detext.txt",false);
	char data[256] = {0};
	cout<<"Enter file input"<<endl;
//	cin>>data;
EncryptionFile("data/img2.bmp","data/img3.bmp",true);
//EncryptionFile("data/img2.bmp","data/img3.bmp",false);
//EncryptionFile("data/���� ����.mp3","data/���� ����1.mp3",true);
//EncryptionFile("data/���� ����1.mp3","data/���� ����2.mp3",false);

	return 0;
}

