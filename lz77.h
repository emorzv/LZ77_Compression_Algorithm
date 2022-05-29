#ifndef LZ77_H
#define LZ77_H

// constants
#define SEARCH_BUFF 10
#define LA_BUFF     10

typedef struct
{
    int offset;
    int len_of_match;
    char codeword;
} triplet_t;

typedef struct
{
    int capacity;
    int front;
    int rear;
    int size;
    triplet_t* triplets;
} queue_t;

// function prototypes
int get_length(FILE* fptr);
void read_file(FILE*, char*, int);
void print_searchBuff(int, int, char*);
void print_lookaheadBuff(int, int, char*);
void print_decoding_array(char*, int, triplet_t);
void init_array(char*, int);
void print_headings();
void print_decoding_heading(int);

queue_t* create_queue(int);
int isEmpty(queue_t*);
int isFull(queue_t*);
void enqueue(queue_t*, triplet_t);
triplet_t dequeue(queue_t*);


#endif // LZ77_H
