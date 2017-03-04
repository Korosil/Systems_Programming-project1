#include "HashTable.h"


unsigned long hash (char *str) {

    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;

}


hash_table HT_create (long size) {

          long i ;
    hash_table ht;

    if (!(ht = (hash_table) malloc (sizeof (hashtable)))) {

        perror ("\tError ");
        return -1;

    }

    if (!(ht->buckets = (primary_bucket_ptr*) malloc (size * sizeof (primary_bucket_ptr)))) {

        perror ("\tError ");
        return -1;

    }

    for (i = 0; i < size; ++i)
        ht->buckets[i] = NULL;

    ht->hash_table_size = size;

    return ht;

}


int HT_insert (hash_table ht, char* key, CDR data) {

    long  position;
    int   num_inserted_numbers;

    position = hash (key);
    if (!ht->buckets[position]) {

        if (!(ht->buckets[position] = (primary_bucket_ptr) malloc (sizeof (primary_bucket)))) {

            perror ("\tError ");
            return -1;

        }

        ht->buckets[position]->num_inserted_numbers = 0;
        ht->buckets[position]->next_primary_bucket_ptr = NULL;

    }

    num_inserted_numbers = ht->buckets[position]->num_inserted_numbers;
    if (num_inserted_numbers == STORED_NUMBERS_ARRAY_SIZE - 1)

    ht->buckets[position]->stored_numbers_array[num_inserted_numbers]

}


void HT_destroy (hash_table ht) {

    int i;

}

