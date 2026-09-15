#include <stdio.h>
#include "add.h"

int main()
{
int a,b,choice;
printf("Enter two numbers\n");
scanf("%d %d", &a, &b);
printf("Enter 1 to add or 2 to subtract ");
scanf("%d",&choice);
if(choice==1)
add(a, b);
else if(choice==2)
	printf("The difference is %d\n", a-b);
else 
	printf("Invalid Choice");

return 0;
}
