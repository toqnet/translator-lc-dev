#include <stdio.h>

int main(void) {
double r = 0, pi = 0, area = 0, length = 0, temp = 0;
long long i = 0;
if (scanf("%lf", &r) != 1) return 1;
pi = 3.14159;
length = ((2LL * pi) * r);
area = (pi * (r * r));
temp = ((-length) + 10.5);
printf("%f %f %f\n", length, area, temp);
return 0;
}
