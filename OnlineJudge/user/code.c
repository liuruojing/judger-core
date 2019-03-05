#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
	double number;
	int num;
	scanf("%lf %d", &number, &num);
	double sum = number;
	double pre = sum;
	for (int n = 0;n<num - 1;n++)
	{
		pre = sqrt(pre);
		sum = sums + pre;

	}

	printf("%.2f", sum);
	return 0;


}