#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "lz77.h"

int main()
{
    //local variables
    FILE* fptr;
    int text_len = 0;

    char* buffer;

    int lookahead_startIndex = -1;
    int lookahead_endIndex = -1;
    int search_startIndex = -1;
    int search_endIndex = -1;


    //open the file
    if ((fptr = fopen("text.txt", "r")) == NULL)
    {
        printf("couldn't open the file...");

        return 1;
    }

    //get text length
    text_len = get_length(fptr);
    buffer = (char*) malloc(text_len * sizeof(char));

    read_file(fptr, buffer, text_len);
    printf("Text from file:\n");
    puts(buffer);
    printf("\n");
    // print headings
    print_headings();

    //create the queue for storing the triplets
    queue_t* Queue = create_queue(20);


    // main part - ENCODE
    // ==========================================================================
    int matchLength = 0;
    int longest_matchLength = 0;
    for (int i = 0; lookahead_startIndex < text_len - 1; i++)
    {
        matchLength = 0;
        // initial values
        if (i == 0)
        {
            lookahead_startIndex = 0;
            lookahead_endIndex = LA_BUFF-1;

            search_startIndex = 0;
            search_endIndex = lookahead_startIndex - 1;
        }
        else
        {
            if (lookahead_endIndex + longest_matchLength + 1 >= text_len)
            {
                lookahead_endIndex = text_len - 1;
            }
            else
            {
                lookahead_endIndex = lookahead_endIndex + longest_matchLength + 1;
            }
            lookahead_startIndex = lookahead_startIndex + longest_matchLength + 1;

            search_endIndex = lookahead_startIndex - 1;

            if (search_endIndex >= SEARCH_BUFF)
            {
                //search_startIndex = search_startIndex + longest_matchLength + 1;
                search_startIndex = search_endIndex - SEARCH_BUFF + 1;
            }

            // is lookahead buffer's starting index out of range?
            if (lookahead_startIndex >= text_len)
            {
                break;
            }
        }

        // print the window
        //  search
        print_searchBuff(search_startIndex, search_endIndex, buffer);
        //  lookahead
        print_lookaheadBuff(lookahead_startIndex, lookahead_endIndex, buffer);

        // search
        int la_iter = lookahead_startIndex;
        int offset_counter = 0;
        triplet_t triplet = { 0, 0, buffer[lookahead_startIndex] };
        longest_matchLength = 0;
        for (int start = search_endIndex; start >= search_startIndex; start--)
        {
            offset_counter++;
            matchLength = 0;
            if (buffer[start] == buffer[la_iter])
            {
                matchLength++;
                la_iter++;

                for (int j = start+1; j <= search_endIndex && buffer[j] == buffer[la_iter]; j++, la_iter++, matchLength++)
                {
                    // do nothing
                }

                if (matchLength > longest_matchLength)
                {
                    triplet.offset = offset_counter;
                    triplet.len_of_match = matchLength;
                    triplet.codeword = buffer[la_iter];
                    longest_matchLength = matchLength;
                }

                la_iter = lookahead_startIndex;
            }
        }

        //printf("Triplet:\n");
        printf("   |   %d", triplet.offset);
        printf("   |   %d", triplet.len_of_match);
        printf("   |   %c   |\n\n", triplet.codeword);
        enqueue(Queue, triplet);
    }
    // ==========================================================================


    // main part - DECODE
    // ==========================================================================
    char decoding_arr[text_len];
    //initialize the decoding array
    init_array(decoding_arr, text_len);
    print_decoding_heading(text_len);

    search_startIndex = 0;
    search_endIndex = 0;
    int index = 0;

    while (!isEmpty(Queue))
    {
        triplet_t current_triplet = dequeue(Queue);

        if (current_triplet.offset == 0)
        {
            decoding_arr[index] = current_triplet.codeword;
            index++;
        }
        else
        {
            for (int i = search_endIndex - current_triplet.offset, j = 0; j < current_triplet.len_of_match; i++, j++)
            {
                decoding_arr[index] = decoding_arr[i];
                index++;
            }

            decoding_arr[index] = current_triplet.codeword;
            index++;
        }

        search_endIndex = search_endIndex + current_triplet.len_of_match + 1;

        if (search_endIndex >= SEARCH_BUFF)
        {
            // not sure if this works
            search_startIndex = search_endIndex - SEARCH_BUFF;
        }

        print_decoding_array(decoding_arr, text_len, current_triplet);
    }
    // ==========================================================================

    return 0;
}


void print_searchBuff(int search_startIndex, int search_endIndex, char* buffer)
{
    printf("[");
    if (search_endIndex < SEARCH_BUFF)
    {
        for (int i = search_endIndex; i < SEARCH_BUFF-1; i++)
        {
            printf(" ");
        }
    }
    for (int srch_temp = search_startIndex; srch_temp <= search_endIndex; srch_temp++)
    {
        printf("%c", buffer[srch_temp]);
    }
    printf("]");
}

void print_lookaheadBuff(int lookahead_startIndex, int lookahead_endIndex, char* buffer)
{
    int counter = 0;

    printf("(");
    for (int la_temp = lookahead_startIndex; la_temp <= lookahead_endIndex; la_temp++)
    {
        printf("%c", buffer[la_temp]);
        counter++;
    }
    if (counter < LA_BUFF)
    {
        for (int i = counter; i < LA_BUFF; i++)
        {
            printf(" ");
        }
    }
    printf(")");
}

void read_file(FILE* fptr, char* buffer, int text_len)
{
    while (fgets(buffer, text_len+1, fptr))
    {
        puts(buffer);
    }
}

int get_length(FILE* fptr)
{
    fseek(fptr, 0, SEEK_END);
    int end = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    return end;
}

void print_decoding_array(char arr[], int len, triplet_t current_triplet)
{
    printf("|");
    for (int i = 0; i < len; i++)
    {
        printf("%c", arr[i]);
    }
    printf("|\t->   |   %d   |   %d   |   %c   |\n", current_triplet.offset, current_triplet.len_of_match, current_triplet.codeword);
}

void init_array(char decoding_arr[], int text_len)
{
    for (int i = 0; i < text_len; i++)
    {
        decoding_arr[i] = ' ';
    }
}

void print_headings()
{
    printf("========================ENCODING========================\n");
    printf("[");
    for (int i = 0; i < SEARCH_BUFF - 2; i++)
    {
        printf(" ");
    }
    printf("sb]");

    printf("(lab");
    for (int i = 0; i < LA_BUFF - 3; i++)
    {
        printf(" ");
    }
    printf(")   |   O   |   L   |   C   |\n");
    printf("--------------------------------------------------------\n");
}
void print_decoding_heading(int text_len)
{
    printf("============================DECODING============================\n");
    printf("|arr");
    for (int i = 0; i < text_len-3; i++)
    {
        printf(" ");
    }
    printf("|\t->   |   O   |   L   |   C   |\n");
    printf("----------------------------------------------------------------\n");
}

// queue functions
queue_t* create_queue(int capacity)
{
    queue_t* queue = (queue_t*) malloc(sizeof(queue_t));

    queue->capacity = capacity;
    queue->rear     = capacity - 1;
    queue->front    = queue->size = 0;
    queue->triplets = (triplet_t*) malloc(capacity * sizeof(triplet_t));

    return queue;
}

int isEmpty(queue_t* queue)
{
    return (queue->size == 0);
}

int isFull(queue_t* queue)
{
    return (queue->size == queue->capacity);
}

void enqueue(queue_t* queue, triplet_t triplet)
{
    if (isFull(queue))
    {
        printf("Queue is full\n");
        return;
    }

    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size = queue->size + 1;
    queue->triplets[queue->rear] = triplet;
    //printf("enqueued -> %d\t%d\t%c\n\n\n", triplet.offset, triplet.len_of_match, triplet.codeword);
}

triplet_t dequeue(queue_t* queue)
{
    triplet_t triplet;
    triplet.offset = INT_MIN;

    if (isEmpty(queue))
    {
        printf("Queue is empty\n");
        return triplet;
    }
    triplet = queue->triplets[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;

    return triplet;
}








