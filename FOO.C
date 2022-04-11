#include <stdio.h>
#include <stdlib.h>
#include "typedefs.h"

#define random(num) (SHORT)(((LONG)rand()*(num))/(0x8000U))
int main()
{
	FILE *fp = fopen("rand.txt","wt");
	
	if (fp)
	{
		LONG i;
		for (i = 0; i < 100; i++)
		{
			int val = random(20);
			fprintf (fp,"%d\n", val);
		}
		
		fclose(fp);
	}
	return EXIT_SUCCESS;
}

