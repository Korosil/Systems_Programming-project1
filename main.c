#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <sys/types.h>

#include    "header.h"
#include    "HashTable.h"

typedef struct {

    hash_table callers_ht;
    hash_table callees_ht;

}structures;


int main (int argsc, char *args []) {

    structures data;
    printf("Hello, World ! \n");
    return 0;
}