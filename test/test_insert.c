#include "../ptrie.h"

extern NODE *root;

#define SAMPLE_DATA 100
#define URL_LENGTH  5000
#define ENTRY_NUM   10

#define URL_FILE    "URL.txt"
#define URL_SHORT   "URL_Short.txt"


int main(int argc, char *argv[])
{
    int status, i;
    ENTRY *en[ENTRY_NUM];
    ENTRY *result;
    char url[URL_LENGTH];
    FILE *fp;
    
    fp = fopen(URL_SHORT, "r");
    if (fp == NULL) {
        perror("URL_Short.txt@fopen");
        return -1;
    }

    i = 0;
    printf("Loading from %s and start to insert()\n", URL_SHORT);
    while(fgets(url, URL_LENGTH, fp) != NULL) { 
        en[i] = createENTRY(url, WHOLE, SAMPLE_DATA, CREATED);
 
        status = ptrie_insert(en[i]);        
        if (status == SUCCESS) {
            printf("%d:insert of [%s] completed.\n", i, en[i]->key);
            i++;
            printf("\n\n");
            continue;
        }
        else if (status == FAILURE) {
            printf("%d:FAILURE@insert():%s\n", i, en[i]->key);
            i++;
            break;
        }
        else {
            printf("Unknown Error@insert():%d\n", i);
        }
    }

    
    printf("Search()\n");
    for(i = 0; i < ENTRY_NUM; i++) {
        result = ptrie_search(root, en[i]);
        if (result == NULL) {
            printf("FAILURE key[%d]:%s@search()", i, en[i]->key);
        }
        else {
            printf("result->key:%s result->val:%u result->status:%d\n",
                                    result->key, result->val, result->status);
            printf("en->key:%s en->val:%u\n", en[i]->key, en[i]->val); 
            putchar('\n');
        }
    }

    
    for(i = 0; i < ENTRY_NUM; i++) {
        free(en[i]->key);
        free(en[i]);
    }

    return 0;
    
}
