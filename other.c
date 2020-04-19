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

void OPT(int frame_count, int page_count, pages_t pages);
void LRU(int frame_count, int page_count, pages_t pages);
void FIFO(int frame_count, int page_count, pages_t pages);
void CLK(int frame_count, int page_count, pages_t pages);
int search_page(frame_t frames, int page);
void print_frames(frame_t frames);
int oldest_entry(frame_t tags);
void clear_frames(frame_t *frames);
int find_next(int current, frame_t frames);
int largest_entry(frame_t frames);

int main(int argc, const char **argv)
{
	unsigned int file_size;
	unsigned int frame_low, frame_high, page_count;
	pages_t pages;
	char *file_data;
	FILE *fptr;

	// Not enough args
	if(argc < 5)
    {
	    printf("Usage: %s <instructions file> <frame count minimum> <frame count maximum> <page count>", argv[0]);
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

	//printf("%d %d\n", frame_high, frame_low);

	free(file_data);

	for(int frames = frame_low; frames <= frame_high; frames++)
	{
		FIFO(frames, page_count, pages);
		LRU(frames, page_count, pages);
		CLK(frames, page_count, pages);
		OPT(frames, page_count, pages);
	}

	return 0;
}

void OPT(int frame_count, int page_count, pages_t pages)
{
    int tmp_frame = 0, fault_count = 0;
	frame_t frames, next_ref, pages_cpy;

	frames.frame = (int *) malloc(frame_count * sizeof(int));
	next_ref.frame = (int *) malloc(frame_count * sizeof(int));

	bzero((void *)frames.frame, sizeof(int)*frame_count);
	bzero((void *)next_ref.frame, sizeof(int)*frame_count);
	memcpy(&pages_cpy,&pages,sizeof(pages_t));

	frames.frame_count = frame_count;
	next_ref.frame_count = frame_count;

	for(int i = 0; i < page_count; i++)
	{
        if((tmp_frame = search_page(frames, pages.page[i])) >= 0)
        {
            // If the page is already in a frame, search for the next
            // occurrence
            next_ref.frame[tmp_frame] = find_next(i, pages_cpy);
        }
        else
        {
            fault_count++;
            // Find any pages that are never used again
            tmp_frame = search_page(next_ref, -1);
            if(tmp_frame > 0)
            {
                // This frame is never used again so its fine to overwrite it
                frames.frame[tmp_frame] = pages.page[i];
                next_ref.frame[tmp_frame] = find_next(i, pages_cpy);
            }
            else
            {
                tmp_frame = search_page(next_ref, largest_entry(next_ref));
                frames.frame[tmp_frame] = pages.page[i];
                next_ref.frame[tmp_frame] = find_next(i, pages_cpy);
            }
        }
	}
    printf("%d\t",fault_count);
	print_frames(frames);
    free(next_ref.frame);
	free(frames.frame);
}

void LRU(int frame_count, int page_count, pages_t pages)
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
	printf("%d\t",fault_count);
	print_frames(frames);
	free(frames.frame);
	free(tags.frame);
}

void FIFO(int frame_count, int page_count, pages_t pages)
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
	printf("%d\t",fault_count);
	print_frames(frames);
    free(frames.frame);
}

void CLK(int frame_count, int page_count, pages_t pages)
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
                } else {
		            set.frame[pointer] = 0;
		        }
            }
        }
	}
    printf("%d\t",fault_count);
	print_frames(frames);

	free(frames.frame);
	free(set.frame);
}

// Return the distance to next page, -1 o.w.
int find_next(int current, frame_t frames)
{
    int distance = 0;

    if((distance = search_page(frames, frames.frame[current])) < 0)
        return -1;
    return distance - current;
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

int largest_entry(frame_t frames)
{
    int largest = frames.frame[0];
    for(int i = 1; i < frames.frame_count; i++)
        if (largest < frames.frame[i])
            largest = frames.frame[i];
    return largest;
}

void print_frames(frame_t frames)
{
	for(int i = 0; i < frames.frame_count; i++)
		printf("%d ",frames.frame[i]);
	printf("\n");
}

void clear_frames(frame_t *frames)
{
    // Clear out the frames
    bzero((void *) frames->frame,sizeof(int) * frames->frame_count);
    free(frames->frame);
}