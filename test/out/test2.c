#include <stdio.h>

int main(void) {
double fact = 0;
long long i = 0, j = 0;
fact = 1LL;
for (i = 1LL; i <= 5LL; i += 1LL) {
for (j = 1LL; j <= i; j += 1LL) fact = (fact * j);
printf("%lld %f\n", i, fact);
}
return 0;
}
