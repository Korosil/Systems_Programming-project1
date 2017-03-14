#include "HashTable.h"


unsigned long hash (char *str, int bound) {

    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    if (hash >= bound)
        hash %= bound;

    return hash;

}

void cdrcpy (CDR* src, CDR* dest) {

    dest->cdr_uniq_id = (char*) malloc ((strlen (src->cdr_uniq_id) + 1) * sizeof (char));
    strcpy (dest->cdr_uniq_id, src->cdr_uniq_id);

    dest->destination_number = (char*) malloc ((strlen (src->destination_number) + 1) * sizeof (char));
    strcpy (dest->destination_number, src->destination_number);

    strcpy (dest->date, src->date);
    strcpy (dest->init_time, src->init_time);

    dest->duration = src->duration;
    dest->type = src->type;

    dest->tarrif = src->tarrif;
    dest->fault_condition = src->fault_condition;

}

//void* findfirstfree (void* tosearch, char* type) {
//
//    int   num;
//    void* totraverse;
//    char  flag;
//
//    if (!strcmp (type, "primary_b")) {
//
//        (primary_bucket_ptr) tosearch;
//        totraverse = tosearch;
//        num = tosearch->num_inserted_numbers;
//        flag = 0;
//
//        while (num == STORED_NUMBERS_ARRAY_SIZE - 1) {
//
//            if (totraverse->next_primary_bucket_ptr)
//                totraverse = totraverse->next_primary_bucket_ptr;
//            else {
//
//                flag = 1;
//                break;
//
//            }
//
//            num = totraverse->num_inserted_numbers;
//
//        }
//
//        if (flag)
//            totraverse = (primary_bucket_ptr) malloc (sizeof (primary_bucket));
//
//    }
//
//}
//

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

    long    position;
    int     num_inserted_numbers, num_inserted_CDRs, bound;
    void*   traverse_ptr, *traverse_bc_ptr;
    char    exstal_flag;

    bound = ht->hash_table_size;
    position = hash (key, bound);


    /* no bucket exists in the specific position */
    if (!ht->buckets[position]) {

        if (!(ht->buckets[position] = (primary_bucket_ptr) malloc (sizeof (primary_bucket)))) {

            perror ("\tError ");
            return -1;

        }

        if (!(ht->buckets[position]->stored_numbers_array[0].number = (char*) malloc ((strlen (key) + 1) * sizeof (char)))) {

            perror ("\tError ");
            return  -1;

        }
        strcpy (ht->buckets[position]->stored_numbers_array[0].number, key);

        if (!(ht->buckets[position]->stored_numbers_array[0].info = (info_bucket_ptr) malloc (sizeof (info_bucket)))) {

            perror ("\tError ");
            return -1;

        }
        cdrcpy (&data, &(ht->buckets[position]->stored_numbers_array[0].info->cdr_info_array[0]));

        ht->buckets[position]->stored_numbers_array[0].info->num_inserted_CDRs = 1;
        ht->buckets[position]->stored_numbers_array[0].info->next_info_bucket_ptr = NULL;

        ht->buckets[position]->num_inserted_numbers = 1;
        ht->buckets[position]->next_primary_bucket_ptr = NULL;

        return OK;
    }
    else {

        /*find first free in stored nums array*/
        num_inserted_numbers = ht->buckets[position]->num_inserted_numbers;
        traverse_bc_ptr = ht->buckets[position];
        exstal_bc_flag = 1;

        while (num_inserted_numbers == STORED_NUMBERS_ARRAY_SIZE - 1) {

            if (traverse_bc_ptr->next_primary_bucket_ptr)
                num_inserted_numbers = traverse_bc_ptr->next_primary_bucket_ptr->num_inserted_numbers;
            else
                exstal_bc_flag = 0;

        }

        /*insert key without allocating new primary bucket*/
        if (exstal_bc_flag) {

            if (!(ht->buckets[position]->stored_numbers_array[num_inserted_numbers].number = (char*) malloc ((strlen (key) + 1) * sizeof (char)))) {

                perror ("\tError ");
                return  -1;

            }
            strcpy (ht->buckets[position]->stored_numbers_array[num_inserted_numbers].number, key);
            ht->buckets[position]->num_inserted_numbers++ ;

            num_inserted_CDRs = ht->buckets[position]->stored_numbers_array[num_inserted_numbers].info->num_inserted_CDRs;
            traverse_ptr = ht->buckets[position]->stored_numbers_array[num_inserted_numbers].info;
            exstal_flag = 1;

            /*find first free position in info buckets*/
            while (num_inserted_CDRs == CDR_INFO_ARRAY_SIZE - 1) {

                if (traverse_ptr->next_info_bucket_ptr)
                    num_inserted_CDRs = traverse_ptr->next_info_bucket_ptr->num_inserted_CDRs;
                else
                    exstal_flag = 0;

            }

            /*insert info without allocating new info bucket*/
            if (exstal_flag) {

                cdrcpy (&data, &(ht->buckets[position]->stored_numbers_array[num_inserted_numbers].info->cdr_info_array[num_inserted_CDRs]));
                ht->buckets[position]->stored_numbers_array[num_inserted_numbers].info->num_inserted_CDRs++;
            }
            /*new info bucket needs to be allocated*/
            else {

                if (!(traverse_ptr->next_info_bucket_ptr = (info_bucket_ptr) malloc (sizeof (info_bucket)))) {

                    perror ("\tError ");
                    return -1;

                }

                cdrcpy (&data, &(traverse_ptr->next_info_bucket_ptr->cdr_info_array[0]));
                traverse_ptr->next_info_bucket_ptr->num_inserted_CDRs = 1;
                traverse_ptr->next_info_bucket_ptr->next_info_bucket_ptr = NULL;

            }

        }
        /*new primary bucket needs to be allocated*/
        else {

            if (!(traverse_bc_ptr->next_primary_bucket_ptr = (primary_bucket_ptr) malloc (sizeof (primary_bucket)))) {

                perror ("\tError ");
                return -1;

            }

            if (!(traverse_bc_ptr->next_primary_bucket_ptr->stored_numbers_array[0].number = (char*) malloc ((strlen (key) + 1) * sizeof (char)))) {

                perror ("\tError ");
                return  -1;

            }
            strcpy (traverse_bc_ptr->next_primary_bucket_ptr->stored_numbers_array[0].number, key);

            if (!(traverse_bc_ptr->next_primary_bucket_ptr->stored_numbers_array[0].info = (info_bucket_ptr) malloc (sizeof (info_bucket)))) {

                perror ("\tError ");
                return -1;

            }
            cdrcpy (&data, &(traverse_bc_ptr->next_primary_bucket_ptr->stored_numbers_array[0].info->cdr_info_array[0]));

            traverse_bc_ptr->next_primary_bucket_ptr->stored_numbers_array[0].info->num_inserted_CDRs = 1;
            traverse_bc_ptr->next_primary_bucket_ptr->stored_numbers_array[0].info->next_info_bucket_ptr = NULL;

            traverse_bc_ptr->next_primary_bucket_ptr->num_inserted_numbers = 1;
            traverse_bc_ptr->next_primary_bucket_ptr->next_primary_bucket_ptr = NULL;

            return OK;

        }

    }

    return OK;

}


void HT_destroy (hash_table ht) {

    int i;

    for (i = 0; i < ht->hash_table_size; i++) {

        //if (ht->buckets[i])

    }

}

