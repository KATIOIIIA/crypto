// rsa.cpp : Defines the entry polong for the console application.
//

#include "stdafx.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
// формируем "решето" простых чисел 
const long size = 1024*16;
bool arr[size];

// формируем "решето" простых чисел 
void simpleNum()
{
	for(long k = 2; k < size; k++) arr[k] = true;
	for(long i = 2; i <  size; i++) {
		if(arr[i] == true) {
			for(long j = i * 2; j < size; j++) {
				if(j % i == 0) arr[j] = false;
			}
		}
	}
}

long phiFunc(long x)
{
  long n = 1;
  for(long i = 2; i <= x / i; i++) {
    if (x % i == 0) {
      x/= i;
      while(x % i == 0) {
        x/= i;
        n*= i;
      }
      n*= i - 1;
    }
  }
  if (x>1) return n*(x-1);
  return n;
}

_int64 gcd(_int64 a, _int64 b)
{
	_int64 tmp;
	while (b>0)
	{
		tmp = a % b;
		a = b;
		b = tmp;
	}
	return a;
}

// алгоритм быстрого возведения в стпень
_int64 pow (_int64 msg, _int64 e)
{
	_int64 res = 1;
	while(e)
	{
		if (e&1) res*=msg;
		msg*=msg;
		e = e>>1;
	}
	return res;
}

// цепочка сложений
// (x^y) mod n
_int64 pow2 (_int64 x, _int64 y,_int64 n)
{
	_int64 res = 1;
	//_int64 tmpX = x;
	//_int64 tmpY = y;
	while (y)
	{
		if (y & 1) res = (res * x) % n;
		y = y >> 1;
		x = (x * x) % n;
	}
	return res;
}




void extended_euclid(_int64 a, _int64 b,_int64 *x, _int64 *y,_int64 *d)
{
  _int64 q, r, x1, x2, y1, y2;
  if (b == 0) {
    *d = a, *x = 1, *y = 0;
    return;
  }
  x2 = 1, x1 = 0, y2 = 0, y1 = 1;
  while (b > 0) {
    q = a / b, r = a - q * b;
    *x =abs( x2 - q * x1), 
		
		*y = y2 - q * y1;
    a = b, b = r;
    x2 = x1, x1 = *x, y2 = y1, y1 = *y;
  }
  *d = a, *x = x2, *y = y2;
}

_int64  gcd21 (_int64  a, _int64  b, _int64  &x, _int64  &y)
{
	if (a==0)  {
		x = 0;
		y = 1;
		return b;
	}
	_int64  x1,y1;
	_int64  d = gcd21(b%a,a,x1,y1);
	x = y1 - (b/a)*x1;
	y = x1;
	return d;

}

_int64 extended_euclid2(_int64 a, _int64 b)
{
  _int64 q, r, x1, x2,x=0;
  x2 = 1, x1 = 0;
  while (b > 0) {
    q = a / b, r = a - q * b;
    x = x2 - q * x1;

    a = b;
	b = r;
    x2 = x1;
	x1 = x;
  }
  return x;
}

_int64 euler(_int64 n){
  
  _int64 t = sqrt(n*1.0)+1,answ=1,ta,i;
  
  for (i=2;i<t;i++){
    
    ta=0;
    
    while (n%i == 0){
      ta++;
      n/=i;
    }
    
    if (ta)
      answ*=pow(i,ta-1)*(i-1);
    
  }
  
  if (n-1)
    
    answ*=(n-1);
  return answ;
}
 
bool EncryptionFile(char *inFileName,char *outFileName, _int64 keyX, _int64 keyY,int sizeBlockIn, int sizeBlockOut)
{
	FILE *fIn=NULL,*fOut=NULL, *fOut2=NULL;
	if (!(fIn  = fopen(inFileName,"rb"))) return false;
	if (!(fOut = fopen(outFileName,"wb"))) return false;
	while (!feof(fIn))
	{	
		_int64 text=0;
		fread(&text,sizeBlockIn,1,fIn);
		//printf("text = %c   ", text);
		_int64 res=pow2(text,keyX,keyY); 
		
		fwrite(&res,sizeBlockOut,1,fOut);
		
		//printf("encode = %c\n", res);
	}
	fclose(fIn);
	fclose(fOut);
	printf("\n\n\n");
	return true;
}

bool ParseInt64(char *lpszValue, _int64 pValue)
{
    char *pszStop;

    errno = 0;
    pValue = _strtoi64(lpszValue, &pszStop, 10);

    // Return no overflow and all characters parsed.
    return errno == 0 && *pszStop == '\0';
}

bool EncryptionData(char *in,char *out, _int64 keyX, _int64 keyY,int sizeBlockIn, int sizeBlockOut)
{

	while (*in)
	{	
		_int64 text=0;

		char *tIn=new char(4);
		strncpy(tIn, in, sizeBlockIn);
		ParseInt64(tIn,text);
		text=_strtoi64(tIn,0,10);
		printf("text = %d   ", text);
		//_int64 res=pow2(text,keyX,keyY); 
	//strncpy(out,new string(res),sizeBlockOut);
		in+=sizeBlockIn;
		
		//printf("encode = %d\n",res);
	}

	printf("\n\n\n");
	return true;
}

int _tmain(long argc, _TCHAR* argv[])
{
	srand(time(NULL));
	simpleNum();
	_int64 d = 0, y=0;
	_int64 p = 0, q = 0, n = 0, phi = 0, e = 0;
	while (d<=0)
	{
		p = rand()%size;
		while (!arr[p])  p = rand()%size;
		q = rand()%size;
		while (!arr[q])  q = rand()%size;

		n = p*q;
		phi = (p-1)*(q-1);
		e = rand()%size;
 		while((e<=phi) && gcd(e,phi)!=1  ) e = rand()%size;
		gcd21(e, phi, d, y);
	}
//	bool status = EncryptionFile("data/mytext.txt","data/etext1.txt",e,n,2,4);
	//if (status) EncryptionFile("data/etext1.txt","data/detext1.txt",d,n,4,2)

	char buf[23]={0};
EncryptionData("kukareku katerina hello",buf,e,n,2,4);



	//bool status = EncryptionFile("data/Григ Утро.mp3","data/Григ УтроEC.mp3",e,n,2,4);
	//if (status) EncryptionFile("data/Григ УтроEC.mp3","data/Григ Утро2.mp3",d,n,4,2);
	/*
	printf("p = %ld\n",p);
	printf("q = %ld\n",q);
	 
	printf("phi = %ld\n",phi);
	printf("e = %ld\n",e);
	printf("d = %ld\n",d);
	for (int i = 2; i<5; i++)
	{
	FILE *fIn=NULL;
	if (!(fIn  = fopen("data/mytext.txt","rb"))) return false;
	
	while (!feof(fIn))
	{	
		_int64 text=0;
		fread(&text,i,1,fIn);
		printf("%ld\n", text);
		_int64 res=pow2(text,e,n);
		printf("%ld\n", res); 
		_int64 res2=pow2(res,d,n);
		printf("%ld\n\n", res2);
	}
	fclose(fIn);
	printf("i: %d\n",i);
	}*/
	return 0;
}

