#include "../ptrie.h"

extern NODE *root;

#define TRIAL_NUM   3

int main(int argc, char *argv[])
{
    int status, i;
    ENTRY *en[TRIAL_NUM];
    ENTRY *result;
    
    KEY key1[] = "academ";
    KEY key2[] = "cache";
    KEY key3[] = "cable";

    DATA val1, val2, val3;
    
    val1 = 10;
    val2 = 20;
    val3 = 30;
 
    en[0] = createENTRY(key1, val1, CREATED);
    en[1] = createENTRY(key2, val2, CREATED);
    en[2] = createENTRY(key3, val3, CREATED);
 
    for(i = 0; i < TRIAL_NUM; i++) {
        status = insert(en[i]);        
        if (status == FAILURE) {
            printf("FAILURE@insert():%d\n", i);
            break;
        }
        else {
            printf("insert of [%s] completed.\n", en[i]->key);
            continue;
        }

    }

    for(i = 0; i < TRIAL_NUM; i++) {
        result = search(root, en[i]->key);
        if (result == NULL) {
            printf("FAILURE key[%d]:%s@search()", i, en[i]->key);
        }
        else {
            printf("result->key:%s result->val:%u result->status:%d\n",
                                    result->key, result->val, result->status);
            putchar('\n');
        }
    }



    for(i = 0; i < TRIAL_NUM; i++) {
        free(en[i]->key);
        free(en[i]);
    }

    free(root);
                
    return 0;
    
}
