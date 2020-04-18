#include <stdio.h>

int main(int argc, const char **argv)
{
	double long file_len = 0;

	if(argc < 5)
	{
		printf("Error: go fuck yourself!\n");
		return 1;
	}

	FILE *fptr;
	fptr = fopen(argv[1],"r");
	fseek(fptr,0,SEEK_END);
	file_len = ftell(fptr);
	printf("file len; %Lf\n", file_len);
	fseek(fptr,0,SEEK_SET);

	file_data = malloc(fptr);
		
	free(file_data);
	printf("what r u lookin at\n");
	return 0;
}
