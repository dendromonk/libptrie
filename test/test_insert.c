#include "../ptrie.h"

extern NODE *root;

#define TRIAL_NUM   3

int main(int argc, char *argv[])
{
    int status, i;
    ENTRY *en[TRIAL_NUM];
    ENTRY *result;
    
    KEY *key[TRIAL_NUM] = {"academe", "cable", "cache"};

    DATA val[TRIAL_NUM] = {10, 20, 30};
    
    for(i = 0; i < TRIAL_NUM; i++) {
        en[i] = createENTRY(key[i], val[i], CREATED);
    }
 
    for(i = 0; i < TRIAL_NUM; i++) {
        status = insert(en[i]);        
        if (status == SUCCESS) {
            printf("insert of [%s] completed.\n", en[i]->key);
            continue;
        }
        else if (status == FAILURE) {
            printf("FAILURE@insert():%d\n", i);
            break;
        }
        else {
            printf("Unknown Error@insert():%d\n", i);
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
        status = delete(&root, root, en[i]->key);
        if (status == DELETED || MATCHED) {
            printf("delete of [%s] completed.\n", en[i]->key);
            continue;
        }
        else if (status == ERROR) {
            printf("FAILURE@delete():%s\n", en[i]->key);
            break;
        }
        else {
            printf("Unknown Error@delete():%s status:%d\n", en[i]->key, status);
            break;
        }
    }
    
    for(i = 0; i < TRIAL_NUM; i++) {
        free(en[i]->key);
        free(en[i]);
    }

    return 0;
    
}
