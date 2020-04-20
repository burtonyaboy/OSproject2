#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dynamic_array {
    int *cell;
    int max;
    int curr;
} arr_t;

typedef struct page_address_stream {
	int *page;
	int count;
} pages_t;

typedef struct frame_storage {
	int *frame;
	int frame_count;
} frame_t;

int OPT(int frame_count, int page_count, pages_t pages);
int LRU(int frame_count, int page_count, pages_t pages);
int FIFO(int frame_count, int page_count, pages_t pages);
int CLK(int frame_count, int page_count, pages_t pages);
int search_page(frame_t frames, int page);
int oldest_entry(frame_t tags);
int locate_next(arr_t array, int n);
int locate_max(arr_t array);

int main(int argc, const char **argv)
{
	unsigned int file_size;
	int frame_low, frame_high, page_count;
	pages_t pages;
	char *file_data;
	FILE *fptr;

	// Not enough args
	if(argc < 5)
    {
	    printf("Usage: ./%s <instructions file> <frame count minimum> <frame count maximum> <page count>", argv[0]);
	    return 0;
    }

	fptr = fopen(argv[1],"r");
	if(fptr == NULL)
	{
	    printf("File: %s not found!!\n", argv[1]);
	    return 1;
	}

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

	// Get rid of the raw file data
	free(file_data);
    int result[4];
    FILE *csv = fopen("output.csv","w+");
    if(csv == NULL)
        return 1;
	// Run each algorithm with each frame count
    printf("+FRAM+FIFO+LRU-+CLK-+OPT-+\n");
    fprintf(csv,"Frames,FIFO,LRU,CLK,OPT\n");
    for(int frames = frame_low; frames <= frame_high; frames++)
	{
		result[0] = FIFO(frames, page_count, pages);
		result[1] = LRU(frames, page_count, pages);
		result[2] = CLK(frames, page_count, pages);
		result[3] = OPT(frames, page_count, pages);
		// top border
		printf("|%d\t |%d |%d |%d |%d |\n",frames,result[0],result[1],result[2],result[3]);
		fprintf(csv,"%d,%d,%d,%d,%d\n",frames, result[0], result[1], result[2], result[3]);
	}
    fclose(csv);
    printf("+----+----+----+----+----+\n");
	// Success
	return 0;
}

// Replace page that wont be used for the longest
// Return the number of page faults
int OPT(int frame_count, int page_count, pages_t pages)
{
    // setup structures to keep track of when pages are being used next
    int tmp_frame = 0, fault_count = 0, next_loc = 0;
	arr_t frames, next_ref, pages_a;

	frames.cell = (int *) malloc(frame_count * sizeof(int));
    pages_a.cell = pages.page;
	next_ref.cell = (int *) malloc(frame_count * sizeof(int));

	bzero((void *)frames.cell, sizeof(int)*frame_count);

	frames.max = next_ref.max = frame_count;
	pages_a.max = pages.count;

	next_ref.curr = frames.curr = -1;

	// Set all frames as available for use
	for(int i = 0; i < next_ref.max; i++)
	    next_ref.cell[i] = -1;

	// Main loop
	for(int i = 0; i < page_count; i++)
	{
	    pages_a.curr = i;
	    // Everytime our program grabs a page, future requests need to be updated
        for(int j = 0; j < next_ref.max; j++)
        {
            if(next_ref.cell[j] < 0)
                next_ref.cell[j] = -1;
            else
                next_ref.cell[j]--;
        }

	    // If the page is already in a frame...
        if((tmp_frame = locate_next(frames, pages.page[i])) >= 0)
        {
            // Update how close its next instance is
            if((next_loc = locate_next(pages_a, pages.page[i])) >= 0)
                next_ref.cell[tmp_frame] = next_loc - i;
            else
                next_ref.cell[tmp_frame] = -1;
        }
        else
        {
            fault_count++;
            // Find any pages that are never used again
            tmp_frame = locate_next(next_ref, -1);
            if(tmp_frame >= 0)
            {
                // This frame is never used again so its fine to overwrite it
                frames.cell[tmp_frame] = pages.page[i];
                // Find how far it is until it's referenced again
            }
            else
            {
                // If there are no other frames, find the one that will be used latest
                tmp_frame = locate_max(next_ref);
                frames.cell[tmp_frame] = pages.page[i];
                // Find how far it is until it is used again
            }
            // Set how long it will be until this page is used again
            if((next_loc = locate_next(pages_a, frames.cell[tmp_frame])) < 0)
                next_ref.cell[tmp_frame] = -1;
            else
                next_ref.cell[tmp_frame] = next_loc - i;
        }
	}
    // Free this memory
    free(next_ref.cell);
	free(frames.cell);
	return fault_count;
}

// Replace the least recently used page
// Return the number of page faults
int LRU(int frame_count, int page_count, pages_t pages)
{
    int fault_count = 0;
	frame_t frames, tags;

	tags.frame =  (int *) malloc(frame_count * sizeof(int));
	frames.frame = (int *) malloc(frame_count * sizeof(int));

	bzero((void *)frames.frame, sizeof(int)*frame_count);
	bzero((void *)tags.frame, sizeof(int)*frame_count);

	frames.frame_count = frame_count;
	tags.frame_count = frame_count;

	int tmp_frame, oldest = 0;

	// Main loop
	for(int i = 0; i < page_count; i++)
	{
		// Search for the page, update that it was used
		if((tmp_frame = search_page(frames, pages.page[i])) >= 0)
		{
			tags.frame[tmp_frame] = i;
			oldest = oldest_entry(tags);
			continue;
		}
		else
        {
		    fault_count++;
            // Otherwise find and replace the oldest
            tmp_frame = search_page(tags, oldest);
            frames.frame[tmp_frame] = pages.page[i];
            tags.frame[tmp_frame] = i;
            oldest = oldest_entry(tags);
        }
	}
	//print_frames(frames);
	free(frames.frame);
	free(tags.frame);
	return fault_count;
}

int FIFO(int frame_count, int page_count, pages_t pages)
{
    int fault_count = 0;
	frame_t frames;
	int oldest = 0;
	frames.frame = (int *) malloc(frame_count * sizeof(int));
	bzero((void *)frames.frame, sizeof(int) * frame_count);
	frames.frame_count = frame_count;

	// Main loop
	for(int i = 0; i < page_count; i++)
	{
		// Search for the page, if its in then skip it
		if(search_page(frames, pages.page[i]) >= 0)
			continue;

        fault_count++;
		frames.frame[oldest] = pages.page[i];
		oldest = (oldest + 1) % frame_count;
	}
	//printf("%d\t",fault_count);
	//print_frames(frames);
    free(frames.frame);
	return fault_count;
}

int CLK(int frame_count, int page_count, pages_t pages)
{
    int fault_count = 0;
	frame_t frames, set;
	int pointer = 0, tmp_frame;

	frames.frame = (int *) malloc(frame_count * sizeof(int));
	set.frame = (int *) malloc(frame_count * sizeof(int));

	bzero((void *)set.frame, sizeof(int)*frame_count);
	bzero((void *)frames.frame, sizeof(int)*frame_count);

	set.frame_count = frame_count;
	frames.frame_count = frame_count;

	// Main loop
	for(int i = 0; i < page_count; i++)
	{
		// Search for the page, if its in then set the bit
		if((tmp_frame = search_page(frames, pages.page[i])) >= 0)
        {
		    set.frame[tmp_frame] = 1;
        } else {
		    fault_count++;
		    for(; pointer < frame_count; pointer = (pointer + 1) % frame_count)
            {
		        // If a replacement is found
		        if(set.frame[pointer] == 0)
                {
		            frames.frame[pointer] = pages.page[i];
		            set.frame[pointer] = 1;
		            pointer = (pointer + 1) % frame_count;
		            break;
                }
		        else
                {
		            set.frame[pointer] = 0;
		        }
            }
        }
	}
    //printf("%d\t",fault_count);
	//print_frames(frames);

	free(frames.frame);
	free(set.frame);
	return fault_count;
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

// Find the smallest number in a bunch of tags
int oldest_entry(frame_t tags)
{
	int smallest = tags.frame[0];
	for(int i = 1; i < tags.frame_count; i++)
		if (smallest > tags.frame[i])
			smallest = tags.frame[i];
	return smallest;
}

int locate_max(arr_t array)
{
    int max = 0;
    for(int i = 0; i < array.max; i++)
    {
        if(array.cell[i] > array.cell[max])
            max = i;
    }
    return max;
}

int locate_next(arr_t array, int n)
{
    for(int i = array.curr + 1; i < array.max; i++)
    {
        if(array.cell[i] == n)
            return i;
    }
    return -1;
}