//test recursive function calls
#include <stdio.h>
#define WriteLine() printf("\n");
#define WriteLong(x) printf(" %lld", (long)x);
#define ReadLong(a) if (fscanf(stdin, "%lld", &a) != 1) a = 0;
#define long long long


const long n = 4;

long a, m, q, r;
long count;
long res;

void copy()
{
  long a,b,c,d,e,f;
  long x;
  e=1;
  a = 2;
  b = a;
  d = b;
  x = 1;
  if(b == 2){
    c = a;
  }else{
    c = a;
    d = b;
  }
  x = 2;
  if(c == a){
    a = 3;
    d = a;
  }else{
    x = 3;
    if(b == 3){
      e = a + b;
    }else{
      e = b + c;
    }
    d = e;
  }
  x = 4;
  f = d;
  while(f<10){
    e = f;
    d = e;
    f = d + a;
    b = c + f;
  }
}
void Factorial(long n)
{
  if (n == 0) {
    res = 1;
  } else {
    Factorial(n-1);
    res = n * res;
  }
}


void FibRec(long n)
{
  long x, y;

  if (n <= 1) {
    res = 1;
  } else {
    FibRec(n-1);
    x = res;
    FibRec(n-2);
    y = res;
    res = x + y;
  }
}


void MoveDisc(long from, long to)
{
  WriteLong(from);
  WriteLong(to);
  WriteLine();
  count = count + 1;
}


void MoveTower(long from, long by, long to, long height)
{
  if (height == 1) {
    MoveDisc(from, to);
  } else {
    MoveTower(from, to, by, height-1);
    MoveDisc(from, to);
    MoveTower(by, from, to, height-1);
  }
}


void Hanoi(long height)
{
  count = 0;
  MoveTower(1, 2, 3, height);
  WriteLine();
  WriteLong(count);
  WriteLine();
}


void main()
{
  a = 16807;
  m = 127;
  m = m * 256 + 255;
  m = m * 256 + 255;
  m = m * 256 + 255;
  q = m / a;
  r = m % a;
  Factorial(7);
  WriteLong(res);
  WriteLine();
  WriteLine();
  FibRec(11);
  WriteLong(res);
  WriteLine();
  WriteLine();
  Hanoi(3);
  WriteLine();
}


/*
 expected output:
 5040

 144

 1 3
 1 2
 3 2
 1 3
 2 1
 2 3
 1 3

 7
*/
