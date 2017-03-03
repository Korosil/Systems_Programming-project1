#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <sys/types.h>

#define     STD_DATE_LEN      9
#define     STD_TIME_LEN      6


typedef char Date [STD_DATE_LEN];
typedef char Time [STD_TIME_LEN];
typedef enum type {DIRECT_CALL, DATA_USAGE, SMS} callType;


typedef struct CDR {

    char*       cdr_uniq_id;
    char*       originator_number;
    char*       destination_number;
    Date        date;
    Time        init_time;
    int         duration;
    callType    type;
    float       tarrif;
    int         fault_condition;
	
} CDR;