#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>


#include    "HashTable.h"
#include    "BinMaxHeap.h"



typedef struct structures {

    hash_table callers_ht;
    hash_table callees_ht;
    max_heap*  heap;

} structures;


int main(int argc, char *argv[]) {

    structures structs;
    int i, retval;
    long ht1_numOfEntries, ht2_numOfEntries, bucket_size;
    char *operations_fname, *config_fname, line[200], *buff, *key, *id;
    FILE *operations_file, *configuration_file;
    config SMS, domVC, intVC, domDU, intDU;
    CDR callersdata;
    Queue* q;

    SMS.calltype = sms;
    SMS.tarrif = '0';
    domVC.calltype = voice_call;
    domVC.tarrif = '1';
    intVC.calltype = voice_call;
    intVC.tarrif = '2';
    domDU.calltype = data_usage;
    domDU.tarrif = '1';
    intDU.calltype = data_usage;
    intDU.tarrif = '2';

    // if (argc < 9) {

    //     fprintf(stderr,
    //             "\n\tusage: ./werhauz -o [Operations] -h1 [Hashtable1NumOfEntries] -h2 [Hashtable2NumOfEntries] -s [BucketSize] {-c [config-file]}\n");
    //     fprintf(stderr, "\tPlease try again.\n");

    //     return -1;

    // }

    config_fname = operations_fname = NULL;
    ht1_numOfEntries = ht2_numOfEntries = bucket_size = -1;

    for (i = 1; i < argc; i+=2) {

        if (!strcmp(argv[i], "-o"))
            operations_fname = argv[i + 1];
        else if (!strcmp(argv[i], "-h1"))
            ht1_numOfEntries = atol(argv[i + 1]);
        else if (!strcmp(argv[i], "-h2"))
            ht2_numOfEntries = atol(argv[i + 1]);
        else if (!strcmp(argv[i], "-s"))
            bucket_size = atol(argv[i + 1]);
        else if (!strcmp(argv[i], "-c"))
            config_fname = argv[i + 1];

    }

    if (!operations_fname || ht1_numOfEntries < 0 || ht2_numOfEntries < 0 || bucket_size < 0 ||
        (!config_fname && argc > 9)) {

        fprintf(stderr, "\tSomething went wrong. Please try again.\n");
        return -1;

    } else {

        operations_file = fopen(operations_fname, "r");

        if (config_fname) {

            configuration_file = fopen(config_fname, "r");

            while (!feof(configuration_file)) {

                if (!fgets(line, sizeof(line), configuration_file)) {

                    fprintf(stderr, "\n\tError reading from configuration file \n");
                    return -1;

                } else {

                    fprintf(stderr, "%s\n",line );

                    if (strstr(line, "#"))
                        continue;
                    else {

                        switch (line[0]) {

                            case '0' :

                                strtok(line, ";");
                                buff = strtok(NULL, ";");

                                SMS.cost = atof(buff);

                                break;

                            case '1' :
                                if (strstr(line, ";")) {

                                    strtok(line, ";");

                                    switch (line[0]) {

                                        case '1' :

                                            strtok(NULL, ";");
                                            domVC.cost = atof(line);

                                            break;

                                        case '2' :
                                            strtok(NULL, ";");
                                            intVC.cost = atof(line);

                                            break;
                                        default:
                                            break;
                                    }

                                } else if (strstr(line, ":")) {

                                    strtok(line, ":");
                                    switch (line[0]) {

                                        case '1' :
                                            buff = strtok(NULL, ";");
                                            domVC.cost = atof(buff);

                                            break;

                                        case '2' :
                                            strtok(NULL, ";");
                                            intVC.cost = atof(line);

                                            break;
                                        default:
                                            break;
                                    }

                                }

                                break;

                            case '2' :
                                strtok(line, ";");
                                switch (line[0]) {

                                    case '1' :
                                        buff = strtok(NULL, ";");
                                        domDU.cost = atof(buff);

                                        break;

                                    case '2' :
                                        buff = strtok(NULL, ";");
                                        intDU.cost = atof(buff);

                                        break;
                                    default:
                                        break;
                                }
                            default:
                                break;
                        }
                    }
                }

            }
            fclose(configuration_file);
        } else {

            //fixed charges
            /*
                          0.1 (fixed)           #  SMS - fixed price

            1       1       0.5 (per minute)    #  Voice Call Domestic
            1       2       1   (per minute)    #  Voice Call Int.
            2       1       0.2 (per minute)    #  Data Call Domestic
            2       2       0.5 (per minute)    #  Data Call Int.
            */
            SMS.cost = 0.1;
            domVC.cost = 0.5;
            intVC.cost = 1;
            domDU.cost = 0.2;
            intDU.cost = 0.5;

        }
    }

    if (!(structs.callers_ht = HT_create(ht1_numOfEntries, bucket_size))) {

        perror("\tError ");
        return -1;

    }

    if (!(structs.callees_ht = HT_create(ht2_numOfEntries, bucket_size))) {

        perror("\tError ");
        return -1;

    }

    if (!(structs.heap = HP_create ())) {

        perror("\tError ");
        return -1;

    }

    q = create_queue();

    while (!feof(operations_file)) {

        if (!fgets(line, sizeof(line), operations_file)) {

            fprintf(stderr, "\n\tError reading from input file, line = %s \n", line);
            //fclose(operations_file);
            //return -1;
            break;
        } else {

            if (strstr(line, "insert")) {

                strtok(line, " ");
                buff = strtok(NULL, ";");
                callersdata.cdr_uniq_id = malloc((strlen(buff) + 1) * sizeof(char));
                strcpy(callersdata.cdr_uniq_id, buff);

                buff = strtok(NULL, ";");
                key = malloc((strlen(buff) + 1) * sizeof(char));
                strcpy(key, buff);

                buff = strtok(NULL, ";");
                callersdata.destination_number = malloc((strlen(buff) + 1) * sizeof(char));
                strcpy(callersdata.destination_number, buff);

                buff = strtok(NULL, ";");
                strcpy(callersdata.date, buff);

                buff = strtok(NULL, ";");
                strcpy(callersdata.init_time, buff);

                buff = strtok(NULL, ";");
                callersdata.duration = atoi(buff);

                buff = strtok(NULL, ";");
                switch (buff[0]) {

                    case '0':
                        callersdata.calltype = sms;
                        break;

                    case '1':
                        callersdata.calltype = voice_call;
                        break;

                    case '2':
                        callersdata.calltype = data_usage;
                        break;

                }

                buff = strtok(NULL, ";");
                callersdata.tarrif = buff[0];

                buff = strtok(NULL, "\n");
                callersdata.fault_condition = atoi(buff);

                //calculate cost
                if (callersdata.calltype == sms)
                    callersdata.cost = SMS.cost;
                else if (callersdata.calltype == voice_call) {

                    if (callersdata.tarrif == '1')
                        callersdata.cost = domVC.cost * callersdata.duration;
                    else if (callersdata.tarrif == '2')
                        callersdata.cost = intVC.cost * callersdata.duration;

                }
                else if (callersdata.calltype == data_usage) {

                    if (callersdata.tarrif == '1')
                        callersdata.cost = domDU.cost * callersdata.duration;
                    else if (callersdata.tarrif == '2')
                        callersdata.cost = intDU.cost * callersdata.duration;

                }

                retval = HT_insert(structs.callers_ht, key, callersdata);  
                HP_insert (structs.heap, key, callersdata.cost, q);              

                if (!retval) {

                    buff = malloc((strlen(key)+1)*sizeof(char));
                    strcpy(buff,key);
                    free (key);

                    key = malloc((strlen(callersdata.destination_number)+1)*sizeof(char));
                    strcpy(key, callersdata.destination_number);
                    free (callersdata.destination_number);

                    callersdata.destination_number = malloc((strlen(buff)+1)*sizeof(char));
                    strcpy(callersdata.destination_number,buff);

                    retval = HT_insert(structs.callees_ht, key, callersdata);
                    free(buff);
                    free(key);
                    free(callersdata.cdr_uniq_id);
                    free(callersdata.destination_number);

                }

            } else if (strstr(line, "delete")) {

                strtok(line, " ");
                id = strtok(line, " ");
                key = strtok(line, "\n");

                if (HT_remove(structs.callers_ht, id, key) == KEY_REMOVED) 
                    printf("Key (%s) from %s's log removed successfully\n",key, id );


            } else if (strstr(line, "find")) {

                Date date1, date2;  strcpy(date1," ");  strcpy(date2, " ");
                Time time1, time2;  strcpy(time1," ");  strcpy(time2, " ");
                
                int flag = 0, flag1 = 0;

                strtok(line, " ");
                key = strtok (NULL, " ");

                while (buff = strtok (NULL, " ")) {

                    if (strlen(buff) == STD_TIME_LEN-1) {

                        if (!flag) {
                            strcpy(time1, buff);
                            flag = 1;
                        }
                        else
                            strcpy(time2, buff);

                    }

                    if (strlen(buff) == STD_DATE_LEN-1) {

                        if (!flag1) {
                            strcpy(date1, buff);
                            flag1 = 1;
                        }
                        else
                            strcpy(date2, buff);

                    }

                }

                HT_find(structs.callers_ht, key, time1, time2, date1, date2);

            } else if (strstr(line, "lookup")) {

                Date date1, date2;  strcpy(date1," ");  strcpy(date2, " ");
                Time time1, time2;  strcpy(time1," ");  strcpy(time2, " ");
                

                int flag = 0, flag1 = 0;

                strtok(line, " ");
                key = strtok (NULL, " ");

                while (buff = strtok (NULL, " ")) {

                    if (strlen(buff) == STD_TIME_LEN-1) {

                        if (!flag) {
                            strcpy(time1, buff);
                            flag = 1;
                        }
                        else
                            strcpy(time2, buff);

                    }

                    if (strlen(buff) == STD_DATE_LEN-1) {

                        if (!flag1) {
                            strcpy(date1, buff);
                            flag1 = 1;
                        }
                        else
                            strcpy(date2, buff);

                    }

                }

                HT_lookup(structs.callees_ht, key, time1, time2, date1, date2);

            } else if (strstr(line, "topdest")) {

                strtok(line," ");
                buff = strtok(NULL,"\n");

                HT_topdest (structs.callers_ht, buff);

            } else if (strstr(line, "print")) {

                fprintf(stderr, "%s\n",line );

                strtok(line," ");
                buff = strtok(NULL,"\n");

                if (!strcmp(buff, "hashtable1"))
                    HT_print(structs.callers_ht, '1');
                else if (!strcmp(buff, "hashtable2"))
                    HT_print(structs.callees_ht, '2');
                else
                    fprintf(stderr, "%s\n", "\n\t I cannot recognise what i should print.\n");
                 

            } else if (strstr(line, "bye")) {

                HT_destroy(structs.callers_ht);
                HT_destroy(structs.callees_ht);

            } else {

                fprintf(stderr, "\n\tCommand not found. (%s)\n", line);

            }


        }

    }

    fclose(operations_file);
    HP_destroy(structs.heap);
    destroy_queue(q);

    return 0;
}
