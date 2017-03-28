#define     STD_DATE_LEN      9
#define     STD_TIME_LEN      6


typedef char Date[STD_DATE_LEN];
typedef char Time[STD_TIME_LEN];
typedef enum type {
    sms, voice_call, data_usage
} Type;


typedef struct CDR {

    char *cdr_uniq_id;
//    char*       originator_number;
    char *destination_number;
    Date date;
    Time init_time;
    int duration;
    Type calltype;
    char tarrif;
    int fault_condition;
    float cost;

} CDR;


typedef struct config {

    Type calltype;
    char tarrif;
    float cost;

} config;


#define OK  0