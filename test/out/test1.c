#include <stdio.h>

int main(void) {
double s1 = 0, s2 = 0, a = 0, b = 0;
long long i = 0, j = 0;
s1 = 0LL;
s2 = 0LL;
if (scanf("%lf %lf", &a, &b) != 2) return 1;
for (i = 0LL; i <= 10LL; i += 2LL) {
s1 = (s2 + a);
s2 = 0LL;
for (j = 1LL; j <= 20LL; j += 1LL) s2 = (s2 + b);
}
printf("%f\n", s1);
return 0;
}
