#include <stdio.h>
#include "add.h"
int main()
{
int a,b;
printf("Enter two numbers\n");
scanf("%d %d", &a, &b);
add(a, b);
return 0;
}
