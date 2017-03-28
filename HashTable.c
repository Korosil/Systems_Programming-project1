#include "HashTable.h"
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <sys/types.h>


unsigned int hash(char *str, int bound) {

    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    if (hash >= bound)
        hash %= bound;

    return hash;

}

void cdrcpy(CDR *src, CDR *dest) {

    dest->cdr_uniq_id = (char *) malloc((strlen(src->cdr_uniq_id) + 1) * sizeof(char));
    strcpy(dest->cdr_uniq_id, src->cdr_uniq_id);

    dest->destination_number = (char *) malloc((strlen(src->destination_number) + 1) * sizeof(char));
    strcpy(dest->destination_number, src->destination_number);

    strcpy(dest->date, src->date);
    strcpy(dest->init_time, src->init_time);

    dest->duration = src->duration;
    dest->calltype = src->calltype;

    dest->tarrif = src->tarrif;
    dest->fault_condition = src->fault_condition;

}


hash_table HT_create(int size, int bsize) {

    int i;
    hash_table ht;

    if (!(ht = (hash_table) malloc(sizeof(hashtable)))) {

        perror("\tError ");
        return NULL;

    }

    if (!(ht->buckets = (primary_bucket_ptr *) malloc(size * sizeof(primary_bucket_ptr)))) {

        perror("\tError ");
        return NULL;

    }

    for (i = 0; i < size; ++i)
        ht->buckets[i] = NULL;

    ht->hash_table_size = size;
    ht->primary_bucket_size = bsize / sizeof(primary_bucket);
    ht->info_bucket_size = bsize / sizeof(info_bucket);

    return ht;

}


void HT_destroy(hash_table ht) {

    info_bucket_ptr     traverse_info_bucket,     info_bucket_todel;
    primary_bucket_ptr  traverse_primary_bucket,  primary_bucket_todel;
    int  i, j, k;

    for (i = 0; i < ht->hash_table_size; i++) {

        if (ht->buckets[i]) {

            traverse_primary_bucket = primary_bucket_todel = ht->buckets[i];

            while (traverse_primary_bucket) {

                for (j = 0; j < traverse_primary_bucket->num_inserted_numbers; j++) {

                    free(traverse_primary_bucket->stored_numbers_array[j].number);

                    traverse_info_bucket = traverse_primary_bucket->stored_numbers_array[j].info;
                    info_bucket_todel = traverse_info_bucket;

                    while (traverse_info_bucket) {

                        for (k = 0; k < traverse_info_bucket->num_inserted_CDRs; k++) {

                            if (traverse_info_bucket->cdr_info_array[k].cdr_uniq_id) {
                                
                                free(traverse_info_bucket->cdr_info_array[k].cdr_uniq_id);
                                free(traverse_info_bucket->cdr_info_array[k].destination_number);

                            }

                        }

                        free(traverse_info_bucket->cdr_info_array);

                        traverse_info_bucket = info_bucket_todel->next_info_bucket_ptr;
                        
                        free(info_bucket_todel);
                        info_bucket_todel = traverse_info_bucket;

                    }

                }

                free(traverse_primary_bucket->stored_numbers_array);
                traverse_primary_bucket = primary_bucket_todel->next_primary_bucket_ptr;
                free(primary_bucket_todel);
                primary_bucket_todel = traverse_primary_bucket;

            }
    
        }

    }


    free(ht->buckets);

    free(ht);
}


int HT_insert(hash_table ht, char *key, CDR data) {

    int position, i, num_inserted_numbers, num_inserted_CDRs, bound;
    info_bucket_ptr traverse_ptr; 
    primary_bucket_ptr traverse_bc_ptr;
    int exstal_flag, exstal_bc_flag, bc_found;

    bound = ht->hash_table_size;
    position = hash(key, bound);

    /* no bucket exists in the specific position */
    if (!ht->buckets[position]) {

        if (!(ht->buckets[position] = (primary_bucket_ptr) malloc (sizeof(primary_bucket)))) {

            perror("\tError ");
            return -1;

        }

        if (!(ht->buckets[position]->stored_numbers_array = (record *) malloc (ht->primary_bucket_size * sizeof(record)))) {

            perror("\tError ");
            return -1;

        }

        for (i = 0; i < ht->primary_bucket_size; i++) {

            ht->buckets[position]->stored_numbers_array[i].number = NULL;
            ht->buckets[position]->stored_numbers_array[i].info = NULL;
        }


        if (!(ht->buckets[position]->stored_numbers_array[0].number = (char *) malloc ((strlen(key) + 1) * sizeof(char)))) {

            perror("\tError ");
            return -1;

        }

        strcpy(ht->buckets[position]->stored_numbers_array[0].number, key);
        ht->buckets[position]->num_inserted_numbers = 1;
        ht->buckets[position]->next_primary_bucket_ptr = NULL;

        if (!(ht->buckets[position]->stored_numbers_array[0].info = (info_bucket_ptr) malloc (sizeof(info_bucket)))) {

            perror("\tError ");
            return -1;

        }

        if (!(ht->buckets[position]->stored_numbers_array[0].info->cdr_info_array = (CDR *) malloc (ht->info_bucket_size * sizeof(CDR)))) {

            perror("\tError ");
            return -1;

        }

        for (int i = 0; i < ht->info_bucket_size; ++i)
        {
            
        }

        cdrcpy(&data, &(ht->buckets[position]->stored_numbers_array[0].info->cdr_info_array[0]));
        ht->buckets[position]->stored_numbers_array[0].info->num_inserted_CDRs = 1;
        ht->buckets[position]->stored_numbers_array[0].info->next_info_bucket_ptr = NULL;

        return OK;

    } else {

        traverse_bc_ptr = ht->buckets[position];
        bc_found = 0;

        while (traverse_bc_ptr) {

            for (i = 0; i < traverse_bc_ptr->num_inserted_numbers; i++) {

                if (!strcmp(traverse_bc_ptr->stored_numbers_array[i].number, key)) {

                    bc_found = 1;
                    break;
         
               }

            }

            if (bc_found)
                break;

            if (traverse_bc_ptr->next_primary_bucket_ptr)
                traverse_bc_ptr = traverse_bc_ptr->next_primary_bucket_ptr;
            else
                break;
            
        }

        /*if key was not found in a primary bucket*/
        if (!bc_found) {
            
                num_inserted_numbers = traverse_bc_ptr->num_inserted_numbers;

                /*if there is no space in the last bucket*/
                if (num_inserted_numbers == ht->primary_bucket_size - 1) {

                    traverse_bc_ptr->next_primary_bucket_ptr = malloc (sizeof(primary_bucket));
                    traverse_bc_ptr = traverse_bc_ptr->next_primary_bucket_ptr;
                    traverse_bc_ptr->stored_numbers_array = (record*) malloc(ht->primary_bucket_size * sizeof(record));
                    traverse_bc_ptr->next_primary_bucket_ptr = NULL;
                    traverse_bc_ptr->num_inserted_numbers = 0;
                    num_inserted_numbers = 0;

                }

                if (!(traverse_bc_ptr->stored_numbers_array[num_inserted_numbers].number = (char *) malloc ((strlen(key) + 1) * sizeof(char)))) {

                    perror("\tError ");
                    return -1;

                }
                strcpy(traverse_bc_ptr->stored_numbers_array[num_inserted_numbers].number, key);
                traverse_bc_ptr->num_inserted_numbers++;

                if (!(traverse_bc_ptr->stored_numbers_array[num_inserted_numbers].info = (info_bucket_ptr) malloc (sizeof(info_bucket)))) {

                    perror("\tError ");
                    return -1;
                
                }

                if (!(traverse_bc_ptr->stored_numbers_array[num_inserted_numbers].info->cdr_info_array = (CDR*) malloc(ht->info_bucket_size * sizeof(CDR)))) {

                    perror("\tError ");
                    return -1;

                }

                traverse_bc_ptr->stored_numbers_array[num_inserted_numbers].info->next_info_bucket_ptr = NULL;
                cdrcpy(&data, &(traverse_bc_ptr->stored_numbers_array[num_inserted_numbers].info->cdr_info_array[0]));
                traverse_bc_ptr->stored_numbers_array[num_inserted_numbers].info->num_inserted_CDRs = 1;

        } 
        /*key has been found in a primary bucket*/
        else {

            traverse_ptr = traverse_bc_ptr->stored_numbers_array[i].info;

            while (traverse_ptr->next_info_bucket_ptr)
                traverse_ptr = traverse_ptr->next_info_bucket_ptr;

            if (traverse_ptr->num_inserted_CDRs == ht->info_bucket_size-1) {

                //allocate new info bucket
                //traverse_ptr->num_inserted_CDRs--;
                traverse_ptr->next_info_bucket_ptr = (info_bucket_ptr) malloc (sizeof(info_bucket));
                traverse_ptr = traverse_ptr->next_info_bucket_ptr;
                traverse_ptr->cdr_info_array = (CDR*) malloc(ht->info_bucket_size * sizeof(CDR));
                cdrcpy(&data, &(traverse_ptr->cdr_info_array[0]));
                traverse_ptr->num_inserted_CDRs = 1;
                traverse_ptr->next_info_bucket_ptr = NULL;

            }
            else {

                num_inserted_CDRs = traverse_ptr->num_inserted_CDRs;
                // printf("paei na mpei sti thesi %d %s\n",num_inserted_CDRs,data.cdr_uniq_id );
                cdrcpy(&data, &(traverse_bc_ptr->stored_numbers_array[i].info->cdr_info_array[num_inserted_CDRs]));
                traverse_bc_ptr->stored_numbers_array[i].info->num_inserted_CDRs++;

            }

        }   

    }

    return OK;

}

/*number is the caller, key is unique id */
int HT_remove(hash_table callers, char* key, char* number) {

    int       bound  , position;
    int        i, y;

    primary_bucket_ptr  traverse_primary_bucket;
       info_bucket_ptr  traverse_info_bucket;

    bound = callers->hash_table_size;
    position = hash(key, bound);

    if (!callers->buckets[position])
        return KEY_NOT_FOUND;

    traverse_primary_bucket = callers->buckets[position];

    while (traverse_primary_bucket) {

        if (traverse_primary_bucket->num_inserted_numbers >= 0) {

            for (i = 0; i <= traverse_primary_bucket->num_inserted_numbers; i++) {

                if (!strcmp(traverse_primary_bucket->stored_numbers_array[i].number, number)) {

                    fprintf(stderr, "%s\n","HT_remove reports: key for delete has just been found." );

                    traverse_info_bucket = traverse_primary_bucket->stored_numbers_array[i].info;

                    while (traverse_info_bucket) {

                        if (traverse_info_bucket->num_inserted_CDRs >= 0) {

                            for (y = 0; y <= traverse_info_bucket->num_inserted_CDRs; y++) {

                                if (!strcmp(traverse_info_bucket->cdr_info_array[y].cdr_uniq_id, key)) {

                                    traverse_info_bucket->num_inserted_CDRs --;
                                    free (traverse_info_bucket->cdr_info_array[y].cdr_uniq_id);
                                    free (traverse_info_bucket->cdr_info_array[y].destination_number);

                                    HT_rearrange(&traverse_info_bucket, y);

                                    return KEY_REMOVED;
                                }

                            }

                        }

                        if (traverse_info_bucket->next_info_bucket_ptr)
                            traverse_info_bucket = traverse_info_bucket->next_info_bucket_ptr;
                        else
                            return KEY_NOT_FOUND;

                    }

                }

            }

        }

        if (traverse_primary_bucket->next_primary_bucket_ptr)
            traverse_primary_bucket = traverse_primary_bucket->next_primary_bucket_ptr;
        else
            return KEY_NOT_FOUND;

    }

}

void HT_rearrange (info_bucket_ptr* bucket, int position) {

    CDR tmpcdr;
    info_bucket_ptr info_bucket, previous;

    int local_inserted;

    /*goto last info bucket*/
    info_bucket = *bucket;

    while(info_bucket->next_info_bucket_ptr) { 
    
        previous = info_bucket;
        info_bucket = info_bucket->next_info_bucket_ptr;

    }

    local_inserted = info_bucket->num_inserted_CDRs;

    tmpcdr.destination_number = malloc ((strlen(info_bucket->cdr_info_array[local_inserted].destination_number)+1) * sizeof(char));
    tmpcdr.cdr_uniq_id        = malloc ((strlen(info_bucket->cdr_info_array[local_inserted].cdr_uniq_id)+1) * sizeof(char));
    cdrcpy (&(info_bucket->cdr_info_array[local_inserted]), &tmpcdr);

    cdrcpy (&(tmpcdr), &((*bucket)->cdr_info_array[position]));

    free (tmpcdr.destination_number);
    free (tmpcdr.cdr_uniq_id);

    local_inserted = --(info_bucket->num_inserted_CDRs);
    if (local_inserted < 0) {

        free (info_bucket->cdr_info_array);
        free (info_bucket);

        previous->next_info_bucket_ptr = NULL;

    }

}


void HT_print(hash_table ht, char type) {

    int i, j, y, flag;
    primary_bucket_ptr primary_bucket_toprint;
       info_bucket_ptr info_bucket_toprint;
    char* string;
    char meaningless1[21] , meaningless2[13];
    
    strcpy(meaningless1, " has been called by:");
    strcpy(meaningless2, " has called:");

    switch (type) {

        case '1':
            printf("\n\n\n\nAbout to print callers' hashtable ...\n\n");
            string = malloc(13*sizeof(char));
            strcpy(string, meaningless2);
            break;
        case '2':
            printf("\n\n\n\nAbout to print callees' hashtable ...\n\n");
            string = malloc(21*sizeof(char));
            strcpy(string, meaningless1);
            break;

    }

    for (i = 0; i < ht->hash_table_size; ++i) {

        printf("Bucket %d:\n",i);

        if (!ht->buckets[i])
            printf("\t(no number has been hashed here yet)\n");
        else {

            primary_bucket_toprint = ht->buckets[i];
            printf("\tNumber:\t\t\t\t%s\n",string);

            while (primary_bucket_toprint) {

                for (j = 0; j < primary_bucket_toprint->num_inserted_numbers; j++) {

                    flag = 1;

                    if (primary_bucket_toprint->stored_numbers_array[j].number) {

                        if (flag) {

                            printf("\t\t%s\t\t",primary_bucket_toprint->stored_numbers_array[j].number);
                            flag = 0;

                        }

                        info_bucket_toprint = primary_bucket_toprint->stored_numbers_array[j].info;

                        while (info_bucket_toprint) {

                            for (y = 0; y < info_bucket_toprint->num_inserted_CDRs; y++) {

                                if (info_bucket_toprint->cdr_info_array[y].destination_number)
                                    printf("\t\t\t  %s.\n",info_bucket_toprint->cdr_info_array[y].destination_number );
                                else
                                    printf("skalw %d j=%d -- i=%d \n", info_bucket_toprint->num_inserted_CDRs,j,i);
                            }

                            info_bucket_toprint = info_bucket_toprint->next_info_bucket_ptr;

                        }

                    }

                }

                primary_bucket_toprint = primary_bucket_toprint->next_primary_bucket_ptr;

            }

        }
     
        printf("\n\n");

    }

    free(string);

}


void HT_topdest (hash_table ht, char* caller) {

    int position,bound, *results, i,j, max,maxpos, found;
    primary_bucket_ptr bucket_tmp;
    info_bucket_ptr    info_tmp;
    char* local_destination_number, *code;

    bound = ht->hash_table_size;
    position = hash(caller, bound);

    results = malloc(999*sizeof(int));
    found = 0;

    for (i = 0; i < 999; i++)
        results[i] = 0;

    if (!ht->buckets[position]) 
        fprintf(stderr, "\n\tCaller \"%s\" cannot be found.\n",caller );
    else {

        bucket_tmp = ht->buckets[position];

        while (bucket_tmp) {

            for (i = 0; i < bucket_tmp->num_inserted_numbers; i++) {

                if (!strcmp(bucket_tmp->stored_numbers_array[i].number, caller)) {

                    found = 1;
                    info_tmp = bucket_tmp->stored_numbers_array[i].info;

                    while (info_tmp) {

                        for (j=0; j<info_tmp->num_inserted_CDRs; j++) {

                            local_destination_number = malloc((strlen(info_tmp->cdr_info_array[j].destination_number) +1)*sizeof(char));
                            strcpy(local_destination_number, info_tmp->cdr_info_array[j].destination_number);
                            code = strtok (local_destination_number, "-");
                            if (code[0] == '0')
                                code++;

                            results[atoi(code)] +=1;

                            free (local_destination_number);
                        }

                        info_tmp = info_tmp->next_info_bucket_ptr;

                    }

                }

                if (found)
                    break;

            }

            if (found)
                break;
            else
                bucket_tmp = bucket_tmp->next_primary_bucket_ptr;

        }

    }

    /*find max*/
    max = results[0];
    maxpos = 0;

    for (i = 1; i<999; i++) {

        if (results[i]>max) {

            maxpos = i;
            max = results[i];

        }

    }

    if (max) {

        if (maxpos < 100)
            printf("Top destination for caller \"%s\":  Country:0%d --- %d made calls \n", caller, maxpos, max);
        else
            printf("Top destination for caller \"%s\":  Country:%d --- %d made calls \n", caller, maxpos, max);
    }
    else
        printf("No calls found\n");


    free(results);

    return;
} 

int timecmp (Time time1, Time time2) {

    if (strcmp(time1, time2) >= 0)
        return 1;
    else
        return 2;

}

int datecmp (Date date1, Date date2) {

    if (strcmp(date1, date2) >= 0)
        return 1;
    else
        return 2;

}

void HT_lookup (hash_table ht, char* key, Time time1, Time time2, Date date1, Date date2) {

    int timeretval = 0;
    int dateretval = 0;
    int bound, position, i, j, goon;

    primary_bucket_ptr pm_bucket;
    info_bucket_ptr  info_bucket;
    CDR cdrtoprint;

    if (strcmp(time1, " ")) {
        if (strcmp(time2, " "))
            timeretval = timecmp(time1, time2);
    }

    if (strcmp(date1, " ")) {
        if (strcmp(date2, " "))
            dateretval = datecmp(date1, date2);
    }


    bound = ht->hash_table_size;
    position = hash (key, bound);

    pm_bucket = ht->buckets[position];

    while(pm_bucket) {

        for (i=0; i<=pm_bucket->num_inserted_numbers; i++) {

            if (!strcmp(pm_bucket->stored_numbers_array[i].number, key)) {

                info_bucket = pm_bucket->stored_numbers_array[i].info;
                printf("%s has received the following calls:\n",key );

                while (info_bucket) {

                    for(j=0; j<info_bucket->num_inserted_CDRs; j++) {

                        cdrtoprint = info_bucket->cdr_info_array[j];

                        if (timeretval) {

                            if ((timecmp(time1, cdrtoprint.init_time) == 1) && (timecmp(cdrtoprint.init_time, time2)) == 2) {
                                
                                if (dateretval) {

                                    if ((datecmp(date1,cdrtoprint.date) == 1) && (datecmp(cdrtoprint.date, date2) == 2)) {

                                        printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );
                                        continue;
                                    
                                    }

                                }
                                
                                printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );
                                continue;
                            
                            }

                        }

                        if (dateretval) {

                            if ((datecmp(date1, cdrtoprint.date) == 1) && (datecmp(cdrtoprint.date, date2)) == 2) {
                                
                                if (timeretval) {

                                    if ((timecmp(time1,cdrtoprint.init_time) == 1) && (timecmp(cdrtoprint.init_time, time2) == 2)) {

                                        printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );
                                        continue;
                                    
                                    }

                                }
                                
                                printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );
                                continue;
                            
                            }

                        }
                        else
                            printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );

                    }

                    info_bucket = info_bucket->next_info_bucket_ptr;
                }
            }

        }

        pm_bucket = pm_bucket->next_primary_bucket_ptr;

    }

}


void HT_find (hash_table ht, char* key, Time time1, Time time2, Date date1, Date date2) {

    int timeretval = 0;
    int dateretval = 0;
    int bound, position, i, j, goon;

    primary_bucket_ptr pm_bucket;
    info_bucket_ptr  info_bucket;
    CDR cdrtoprint;

    if (strcmp(time1, " ")) {
        if (strcmp(time2, " "))
            timeretval = timecmp(time1, time2);
    }

    if (strcmp(date1, " ")) {
        if (strcmp(date2, " "))
            dateretval = datecmp(date1, date2);
    }


    bound = ht->hash_table_size;
    position = hash (key, bound);

    pm_bucket = ht->buckets[position];

    while(pm_bucket) {

        for (i=0; i<=pm_bucket->num_inserted_numbers; i++) {

            if (!strcmp(pm_bucket->stored_numbers_array[i].number, key)) {

                info_bucket = pm_bucket->stored_numbers_array[i].info;
                printf("%s has called the following CDRs:\n",key );

                while (info_bucket) {

                    for(j=0; j<info_bucket->num_inserted_CDRs; j++) {

                        cdrtoprint = info_bucket->cdr_info_array[j];

                        if (timeretval) {

                            if ((timecmp(time1, cdrtoprint.init_time) == 1) && (timecmp(cdrtoprint.init_time, time2)) == 2) {
                                
                                if (dateretval) {

                                    if ((datecmp(date1,cdrtoprint.date) == 1) && (datecmp(cdrtoprint.date, date2) == 2)) {

                                        printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );
                                        continue;
                                    
                                    }

                                }
                                
                                printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );
                                continue;
                            
                            }

                        }

                        if (dateretval) {

                            if ((datecmp(date1, cdrtoprint.date) == 1) && (datecmp(cdrtoprint.date, date2)) == 2) {
                                
                                if (timeretval) {

                                    if ((timecmp(time1,cdrtoprint.init_time) == 1) && (timecmp(cdrtoprint.init_time, time2) == 2)) {

                                        printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );
                                        continue;
                                    
                                    }

                                }
                                
                                printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );
                                continue;
                            
                            }

                        }
                        else
                            printf("\t %s;%s;%s;%s;%d;%c;%d\n",cdrtoprint.cdr_uniq_id, cdrtoprint.destination_number,cdrtoprint.date,cdrtoprint.init_time,cdrtoprint.duration,cdrtoprint.tarrif,cdrtoprint.fault_condition );

                    }

                    info_bucket = info_bucket->next_info_bucket_ptr;
                }
            }

        }

        pm_bucket = pm_bucket->next_primary_bucket_ptr;

    }

}