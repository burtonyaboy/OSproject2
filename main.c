#include <stdio.h>

int main(int argc, const char **argv)
{
	if(argc < 5)
	{
		printf("Error: go fuck yourself!\n");
		return 1;
	}

	FILE *fptr;
	fptr = fopen(argv[1],"r");

	printf("what r u lookin at\n");
	return 0;
}
