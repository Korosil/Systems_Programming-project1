#include  "header.h"



#define   CDR_INFO_ARRAY_SIZE   50

typedef struct info_bucket {

                   CDR    cdr_info_array [CDR_INFO_ARRAY_SIZE];
                   int    num_inserted_cdrs;
    struct info_bucket*   next_info_bucket_ptr;

} info_bucket;

typedef info_bucket*  info_bucket_ptr;



typedef struct record {

               char*  number;
    info_bucket_ptr   info  ;

} record;


#define   STORED_NUMBERS_ARRAY_SIZE   50

typedef struct primary_bucket {

                   record*   stored_numbers_array [STORED_NUMBERS_ARRAY_SIZE];
                      int    num_inserted_numbers;
    struct primary_bucket*   next_primary_bucket_ptr;

} primary_bucket;
typedef primary_bucket*     primary_bucket_ptr;



typedef struct hashtable {

    primary_bucket_ptr*    buckets;
                  long     hash_table_size;

} hashtable;

typedef hashtable*    hash_table;





/*****************************************************************************************************************/
/*                                               PROTOTYPES                                                      */
/*****************************************************************************************************************/

hash_table HT_create  (int);
void       HT_destroy (hash_table);

