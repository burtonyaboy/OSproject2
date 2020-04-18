#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct page_address_stream {
	int *page;
	int count;
} pages_t;

typedef struct frame_storage {
	int *frame;
	int frame_count;
} frame_t;

void OPT();
void LRU();
void FIFO(int frame_count, int page_count, pages_t pages);
void CLK();
int search_page(frame_t frames, int page);

int main(int argc, const char **argv)
{
	unsigned int file_size;
	unsigned int frame_low, frame_high, page_count;
	pages_t pages;
	char *file_data;
	FILE *fptr;

	// Not enough args
	if(argc < 5)
		return 1;

	fptr = fopen(argv[1],"r");
	if(fptr == NULL)
		return 1;

	// Read args
	frame_low = atoi(argv[2]);
	frame_high = atoi(argv[3]);
	page_count = atoi(argv[4]);

	// Get file size and create some space to read it
	fseek(fptr, 0, SEEK_END);
	file_size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	file_data = malloc(file_size);

	// Read it
	fread(file_data,1,file_size,fptr);
	fclose(fptr);

	// Put values into data structure
	pages.page = (int *) malloc(file_size * sizeof(int));
	char *pch = strtok(file_data, " ");
	pages.page[0] = atoi(pch);
	for(pages.count = 1; pch != NULL; pages.count++)
	{
		pages.page[pages.count] = atoi(pch);
		pch = strtok(NULL, " ");
	}

	//printf("%d %d\n", frame_high, frame_low);

	free(file_data);

	for(int frames = frame_low; frames <= frame_high; frames++)
	{
		FIFO(frames, page_count, pages);
	}

	free(pages.page);
	return 0;
}

void OPT(int frame_count, int page_count, pages_t pages)
{
	frame_t frames;
	char frame_full = 0;

	frames.frame = (int *) malloc(frame_count * sizeof(int));
	for(int i = 0; i < page_count; i++)
	{
		frames.frame[i % frame_count] = pages.page[i];
	}
	printf("%d %d %d\n", frames.frame[0], frames.frame[1], frames.frame[2]);
}

void LRU(int frame_count, int page_count, pages_t pages)
{

}

void FIFO(int frame_count, int page_count, pages_t pages)
{
	frame_t frames;
	char frame_full = 0;
	int oldest = 0;
	frames.frame = (int *) malloc(frame_count * sizeof(int));
	frames.frame_count = frame_count;

	// Main loop
	for(int i = 0; i < page_count; i++)
	{
		// Search for the page, if its in then skip it
		if(search_page(frames, pages.page[i]) >= 0)
			continue;

		frames.frame[oldest] = pages.page[i];
		oldest = (oldest + 1) % frame_count;
	}
	printf("%d %d %d\n", frames.frame[0], frames.frame[1], frames.frame[2]);
}

void CLK(int frame_count, int page_count, pages_t pages)
{

}

// Return -1 if not found, otherwise return page #
int search_page(frame_t frames, int page)
{
	for(int i = 0; i < frames.frame_count; i++)
	{
		if(page == frames.frame[i])
			return i;
	}
	return -1;
}
