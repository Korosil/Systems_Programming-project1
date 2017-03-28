#include  "header.h"


#define KEY_NOT_FOUND 1
#define KEY_REMOVED   0

typedef struct info_bucket {

    CDR *cdr_info_array;
    int num_inserted_CDRs;
    struct info_bucket *next_info_bucket_ptr;

} info_bucket;

typedef info_bucket *info_bucket_ptr;


typedef struct record {

    char *number;
    info_bucket_ptr info;

} record;


#define   STORED_NUMBERS_ARRAY_SIZE   50

typedef struct primary_bucket {

    record *stored_numbers_array;
    int num_inserted_numbers;
    struct primary_bucket *next_primary_bucket_ptr;

} primary_bucket;
typedef primary_bucket *primary_bucket_ptr;


typedef struct hashtable {

    primary_bucket_ptr *buckets;
    int hash_table_size;
    int primary_bucket_size;
    int info_bucket_size;

} hashtable;

typedef hashtable *hash_table;



int timecmp(Time, Time);
int datecmp(Date, Date);




/*****************************************************************************************************************/
/*                                               PROTOTYPES                                                      */
/*****************************************************************************************************************/

hash_table HT_create(int, int);

int HT_insert(hash_table, char *, CDR);

void HT_destroy(hash_table);

int HT_remove(hash_table , char*, char*);

void HT_print(hash_table , char);

void HT_topdest (hash_table , char*);

void HT_rearrange (info_bucket_ptr*, int );

void HT_lookup (hash_table, char*, Time, Time, Date, Date);

void HT_find (hash_table, char*, Time, Time, Date, Date);


