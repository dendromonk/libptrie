#include "ptrie.h"

NODE *root = NULL;

NODE* createLEAF(KEY *key, size_t start_point, DIGIT digit, DATA val) {

    NODE *leaf;
    size_t len;
    const KEY *src = key + start_point;

    leaf = malloc(sizeof(NODE));
    if (leaf == NULL) {
        perror("malloc in createLEAF()");
        return NULL;
    }
    else {

        leaf->type = LEAF;

        len = strnlen(src, MAX_KEY_LEN);
        leaf->Key = malloc(sizeof(KEY)*(len + 1));
        if (leaf->Key == NULL) {
            perror("malloc()createLEAF()");
            return NULL;
        }
        else {
            memcpy(leaf->Key, src, len);
            *(leaf->Key + len) = '\0';
            //strlcpy(leaf->Key, src, len + 1);
            leaf->LDigit = (DIGIT)digit; 
            leaf->Val = val;

            return leaf;
        }
    }

    return NULL;
}

int reSizeKEY(NODE *node, size_t start_point, unsigned char node_type) {
    
    KEY **target;
    KEY *oldKey, *tmp;
    size_t oldLen, newLen;

    target = REF_KEY(node);

    oldLen = strnlen(*target, MAX_KEY_LEN);
    oldKey = malloc(sizeof(KEY) * (oldLen + 1));
    if (oldKey == NULL) {
        perror("malloc@reSizeKEY()");
        return ERROR;
    }
    else {
        memcpy(oldKey, *target, oldLen);
        *(oldKey + oldLen) = '\0';
        //strlcpy(oldKey, *target, oldLen + 1);

        newLen = strnlen(*target + start_point, MAX_KEY_LEN);
        tmp = realloc(*target, sizeof(KEY) * (newLen + 1));
        if (tmp == NULL) {
            perror("realloc@reSizeKEY()");
            return ERROR;
        }
        else {
            *target = tmp;
            memcpy(*target, oldKey + start_point, newLen);
            *(*target + newLen) = '\0';

            free(oldKey);
            return SUCCESS;
        }
    }
}

unsigned int compare_letter(KEY *key1, KEY *key2, size_t len)
{
    unsigned int i;

    for (i = 0; i < len; i++) {
        if ( *(key1 + i) != *(key2 + i) ) {
            break;
        }
        else {
            continue;
        }
    }

    return i;
}

unsigned char compare_bit(KEY *key1, DIGIT bottom, KEY *key2, DIGIT top)
{
    KEY bitKey1, bitKey2, mask;
    int i, point;

    for (i = TOP(top); i >= BOTTOM(bottom); i--) {
        mask = 1 << i;
        bitKey1 = (*key1) & mask;
        bitKey2 = (*key2) & mask;
        if (bitKey1 != bitKey2) {
            break;
        }
        else {
            if (point > BOTTOM(bottom)) {
                point--;
            }
            continue;
        }
    }
    
    return (unsigned char)(++point);
}

unsigned int compareSHARE(KEY *share, DIGIT shareDigit, ENTRY *en)
{
    KEY *key = en->key + en->num_letter;
    unsigned char keyDigit = en->digit;
    unsigned  char digit;
    unsigned int num_letter, fullBit;
    size_t len, lenShare, lenKey;

    lenShare = strnlen(share, MAX_KEY_LEN);
    lenKey = strnlen(key, MAX_KEY_LEN);
    if (lenShare < lenKey) {
        len = lenShare;
    }
    else {
        len = lenKey;
    }

    num_letter = 0;
    if (lenShare == 1) {
        digit = compare_bit(share, shareDigit, key, keyDigit);
    }
    else {
        switch(en->digit) {
            case WHOLE:
                num_letter = compare_letter(share, key, len);
                if ( (num_letter + 1) == lenShare && shareDigit != WHOLE ) {
                    digit = compare_bit(share + num_letter, shareDigit,
                                        key + num_letter,   MSD);
                    break;
                }
                else if (num_letter == len && shareDigit != WHOLE) {
                    num_letter--;
                    digit = compare_bit(share + num_letter, shareDigit,
                                        key + num_letter,   MSD);
                    break;
                }
                else if (num_letter < len) {
                    digit = compare_bit(share + num_letter, WHOLE,
                                        key + num_letter, MSD);
                    break;
                }
                else { /* if (num_letter == len && shareDigit == WHOLE &&
                              digit == WHOLE ) {
                        *
                        * all letter matched and shareDigit == WHOLE and 
                        * digit == WHOLE 
                        */
                    digit = WHOLE;
                    break;
                }
            default:
                digit = compare_bit(share, WHOLE, key, keyDigit);
                if (digit == WHOLE) {
                    num_letter = compare_letter(share + 1, key + 1, len - 1);
                    num_letter++;
                    if ( (num_letter + 1) == lenShare ) {
                        digit = compare_bit(share + num_letter, shareDigit,
                                            key + num_letter, MSD);
                        break;
                    }
                    else if ( (num_letter + 1) < lenShare ) {
                        digit = compare_bit(share + num_letter, WHOLE,
                                            key + num_letter, MSD);
                        break;
                    }
                    else if (num_letter == lenShare && shareDigit != WHOLE) {
                        num_letter--;
                        digit = compare_bit(share + num_letter, shareDigit,
                                            key + num_letter, MSD);
                        break;
                    }
                    else {
                        /* all strings and bits matched. */
                        digit = WHOLE;
                        break;
                    }
                }
                else {
                    /* new collision occurred. */
                    break;
                }
        }
    }
    
    fullBit = (unsigned int)digit + ((BIT_UNIT + 1) * num_letter);
       
    return fullBit;
}


int compareLEAF(KEY *leafKey, DIGIT top, ENTRY *en)
{
    KEY *key = en->key + en->num_letter;
    unsigned char keyDigit = en->digit;
    unsigned  char digit;
    int num_letter, fullBit;
    size_t len, lenLeaf, lenKey;

    if (top != keyDigit) {
        printf("compare consistency error.@compareLEAF():top:%#3x keyDigit:%#3x\n", top, keyDigit);
        return ERROR;
    }
    else {
        num_letter = 0;

        lenLeaf = strnlen(leafKey, MAX_KEY_LEN);
        lenKey = strnlen(key, MAX_KEY_LEN);
        if (lenLeaf < lenKey) {
            len = lenLeaf;
        }
        else {
            len = lenKey;
        }

        if (top == WHOLE) {
            num_letter = compare_letter(leafKey, key, len);
            if (num_letter < len) {
                digit = compare_bit(leafKey + num_letter, WHOLE,
                                    key + num_letter, MSD);
            }
            else {
                digit = WHOLE;
            }
        }
        else {
            digit = compare_bit(leafKey, WHOLE, key, top);
            if (digit == WHOLE) {
                num_letter =  compare_letter(leafKey + 1, key + 1, len - 1);
                num_letter++;
                if (num_letter < len) {
                    digit = compare_bit(leafKey + num_letter, WHOLE,
                                        key + num_letter, MSD);
                }
                else {
                    digit = WHOLE;
                    /* all strings and bits matched. */
                }
            }
            else {
                /* First String or bits unmatched. */
            }
        }

        fullBit = digit + ((BIT_UNIT + 1) * num_letter);

        return fullBit;
    }
}


int Initiate(ENTRY *en) {

    NODE *first;
    size_t len;

    len = strnlen(en->key, MAX_KEY_LEN);
    first = createLEAF(en->key, 0, WHOLE,  en->val);
    if (first == NULL) {
        return ERROR;
    }
    else {
        root = first;
    }

    return SUCCESS;
}

bool pullout_bit(KEY *key, int digit) {

    KEY mask, result;

    mask = 1 << (digit - 1);

    result = *key & mask;
    if (result > 0) {
        return RIGHT;
    }
    else {
        return LEFT;
    }
}

KEY* createSHARE(KEY *key, int num_letter) {

    KEY *share;

    share = malloc(sizeof(KEY) * (num_letter + 1));
    if (share == NULL) {
        perror("malloc@createSHARE()");
        share = NULL;
    }
    else {
        memcpy(share, key, num_letter);
        *(share + num_letter) = '\0';
        //strlcpy(share, key, num_letter + 1); 
        printf("  share:%s@createSHARE\n", share);
        return share;
    }

    return NULL;
}

NODE* createINTERNAL(KEY *left, unsigned int start_point, DIGIT digit) {

    NODE *new;
    
    new = malloc(sizeof(NODE));
    if (new == NULL) {
        perror("malloc@createINTERNAL()");
        return NULL;
    }
    else {
        new->type = INTERNAL;
        new->SDigit = (unsigned char)digit;
        new->Share = createSHARE(left, start_point);

        new->Left = NULL;
        new->Right = NULL;

        return new;
    }
}

int createFork(NODE **pnode, NODE *node, ENTRY *en) {
    NODE *new, *next;
    NODE **parent;
    KEY **target;
    KEY *key = en->key + en->num_letter;
    int  fullBit;
    unsigned char digit, leafDigit;
    int status;
    bool point;
    size_t lenKey, lenTarget, lenShare, num_letter;
    
    target = REF_KEY(node);
    lenTarget = strnlen(*target, MAX_KEY_LEN);

    lenKey = strnlen(key, MAX_KEY_LEN);

    switch(node->type) {
        case LEAF: 
            fullBit = compareLEAF(*target, node->LDigit, en);
            if (fullBit == ERROR) {
                return ERROR;
            }
            else {
                num_letter = fullBit / (BIT_UNIT + 1);
                digit = (unsigned char)(fullBit % (BIT_UNIT + 1));
                if ( digit == 0 ) { //&& (lenKey - num_letter) == 0 ) {
                    status = MATCHED;
                    break;
                }
                else if ( digit > 0 ) { //|| (lenKey - num_letter) > 0 ) {
                    status = UNMATCHED;
                    break;
                }
                
                else {
                    status = ERROR;
                    printf("ERROR@ptrie_insert():%s\n", *target);
                    break;
                }
            }
        case  INTERNAL:
            fullBit = compareSHARE(*target, node->SDigit, en);
            num_letter = fullBit / (BIT_UNIT + 1);
            digit = (unsigned char)(fullBit % (BIT_UNIT +  1));
            if ( digit == node->SDigit && (lenTarget - 1) == num_letter && 
                 (lenKey - num_letter) > 0 ) {
                if (digit > 0) {
                    point = pullout_bit(key + num_letter, digit);
                    if ( digit == LSD ) {
                        num_letter++;
                        digit = WHOLE;
                    }
                    else {
                        digit--;
                        /* digit basically smaller at -1 which is
                         * consistence. */
                    }
                }
                else { /* else if (digit == WHOLE) { */
                    num_letter++;
                    point = pullout_bit(key + num_letter, MSD);
                    digit = MSD - 1;
                }

                switch(point) {
                    case LEFT:
                        next = node->Left;
                        parent = &(node->Left);
                        break;
                    case RIGHT:
                        next = node->Right;
                        parent = &(node->Right);
                        break;
                }

                en->num_letter += num_letter;
                en->digit = digit;
                status = createFork(parent, next, en);

                return status;
            }
            else if ( digit == node->SDigit && (lenTarget - 1) == num_letter &&
                      (lenKey - num_letter) == 0 ) {
                status = MATCHED;
                break;
            }
            else if ( digit != node->SDigit && (lenTarget - 1) == num_letter &&
                      (lenKey - num_letter) > 0 ) {
                /* createINTERNAL */
                status = UNMATCHED;
                break;
            }
            else if ( (lenTarget - 1) > num_letter &&
                      (lenKey - num_letter) > 0 ) {
                status = UNMATCHED;
                break;
            }
            /*
            else if ( (num_letter < lenTarget) && (lenKey - num_letter) > 0 ) {
                * createINTERNAL *
                status = UNMATCHED;
                break;
            }
            */
            else {
                printf("Internal Unknown Error\ndigit:%#3x len - num_letter:%lu num_letter:%lu en->digit:%#3x\n", digit, lenKey - num_letter, num_letter, en->digit);
                printf("Internal->Share:%s Internal->SDigit:%#x en->key:%s en->digit:%#3x\n", node->Share, node->SDigit, en->key, en->digit);
                printf("fullBit:%u\n", fullBit);
                status = ERROR;
                break;
            }
        default:
            return ERROR;
    }

    
   if (status == MATCHED) {
        return MATCHED;
   }
   else if (status == UNMATCHED) {

       lenShare = num_letter + 1;

       new = createINTERNAL(*target, lenShare, digit);

       point = pullout_bit(key + num_letter, digit);

       if ( digit == LSD ) {
           leafDigit = WHOLE;
           num_letter++;
       }
       else {
           leafDigit = digit - 1;
       }

       status = reSizeKEY(node, num_letter, node->type);
       if (status == ERROR) {
            perror("reSizeKEY@createFork()");
            return ERROR;
        }
        else {
            switch(point) {
                case LEFT:
                    new->Left = createLEAF(key, num_letter, 
                                           leafDigit, en->val);
                    new->Right = node;
                    break;
                case RIGHT:
                    new->Right = createLEAF(key, num_letter,
                                            leafDigit, en->val);
                    new->Left = node;
                    break;
            }

            if (node->type == LEAF) {
                node->LDigit = leafDigit;
            }

            *pnode = new;

            return APPEND;
        }
   }
   else {
       printf("Unknown Error\n");
       return ERROR;
   }

}


ENTRY* createENTRY(KEY *key, DIGIT digit, DATA val, int status) {
    
    ENTRY *en;
    size_t len;
    
    len = strnlen(key, MAX_KEY_LEN);
    en = malloc(sizeof(ENTRY));
    if (en == NULL) {
        perror("malloc@createENTRY()");
        return NULL;
    }
    else {
        en->key = malloc(sizeof(KEY) * (len + 1));
        if (en->key == NULL) {
            perror("malloc@createENTRY()");
            return NULL;
        }
        else {
            memcpy(en->key, key, len);
            *(en->key + len) = '\0';
            //strlcpy(en->key, key, len + 1);
            en->num_letter = 0;
            en->val = val;
            en->digit = (unsigned char)digit;
            en->status = status;

            return en;
        }
    }

    return NULL;
}


ENTRY* ptrie_search(NODE *node, ENTRY *en) {
    
    KEY *target, *key;
    int fullBit;
    unsigned char digit;
    size_t lenKey, lenTarget, num_letter;
    NODE *next;
    ENTRY *result;
    bool point;
    int status;
    
    if (node == NULL) {
        return NULL;
    }
    else {
    key = en->key + en->num_letter;
    target = KEYCHAR(node);
    lenTarget = strnlen(target, MAX_KEY_LEN);

    lenKey = strnlen(key, MAX_KEY_LEN);

    num_letter = 0;
    switch(node->type) {
        case LEAF: 
            fullBit = compareLEAF(target, node->LDigit, en);
            if (fullBit == ERROR) {
                return NULL;
            }
            else {
                num_letter = fullBit / (BIT_UNIT + 1);
                digit = (unsigned char)(fullBit % (BIT_UNIT + 1));
                if ( digit == 0) {// && (lenKey - num_letter) == 0 ) {
                    status = MATCHED;
                    break;
                }
                else if ( digit > 0) { // || (lenKey - num_letter) > 0 ) {
                    status = UNMATCHED;
                    break;
                }
                else {
                    status = ERROR;
                    printf("ERROR@ptrie_search():%s\n", target);
                    break;
                }
            }
        case INTERNAL:
            fullBit = compareSHARE(target, node->SDigit, en);
            num_letter = fullBit / (BIT_UNIT + 1);
            digit = (unsigned char)(fullBit % (BIT_UNIT + 1));
            if ( digit == node->SDigit && (lenTarget - 1) == num_letter &&
                 (lenKey - num_letter) > 0 ) {
                if (digit > 0) {
                    point = pullout_bit(key + num_letter, digit);
                    if ( digit == LSD ) {
                        num_letter++;
                        digit = WHOLE;
                    }
                    else {
                        digit--;
                    }
                }
                else { // if (digit == WHOLE) {
                    num_letter++;
                    point = pullout_bit(key + num_letter, MSD);
                    digit = MSD - 1;
                }

                switch(point) {
                    case LEFT:
                        next = node->Left;
                        break;
                    case RIGHT:
                        next = node->Right;
                        break;
                }
            
                en->num_letter += num_letter;
                en->digit = digit;
                result = ptrie_search(next, en);

                return result;

            }
            else if ( digit == node->SDigit && (lenTarget - 1) == num_letter &&
                      (lenKey - num_letter) == 0 ) {
                status = MATCHED;
                break;
            }
            else if ( digit != node->SDigit && (lenTarget - 1) == num_letter &&
                      (lenKey - num_letter) > 0 ) {
                /* createINTERNAL */
                status = UNMATCHED;
                break;
            }
            else if ( num_letter < lenTarget &&
                      (lenKey - num_letter) > 0 ) {
                /* createINTERNAL */
                status = UNMATCHED;
                break;
            }
            else {
                status = ERROR;
                printf("Internal Unknown Error@ptrie_search()\n");
                break;
            }
        default:
            status = ERROR;
            break;
    }

    if (status != ERROR) {
        switch(node->type) {
            case LEAF:
                result = createENTRY(node->Key, node->LDigit, node->Val, status);
                break;
            case INTERNAL:
                result = createENTRY(node->Share, node->SDigit, 0, status);
                break;
        }

        return result;
    }
    else {
        return NULL;
    }
    }


    return NULL;
}

int restoreShare(KEY  *restKey,   NODE *node,
                 NODE *rest,      bool collBit)
{
    size_t end_point, lenShare;

    lenShare = strnlen(node->Share, MAX_KEY_LEN);

    end_point = lenShare - 1;

    memcpy(restKey, node->Share, lenShare);
    *(restKey + lenShare) = '\0';

    *(restKey + end_point) &= MASK(node->SDigit);
    /* Restore a part of Collision bit. */
    switch(collBit) {
        case 0:
            *(restKey + end_point) |= (1 << (node->SDigit - 1));
            break;
        case 1:
            *(restKey + end_point) &= PINBIT(node->SDigit);
            break;
        default:
            return ERROR;
    }

    return SUCCESS;
}

int ptrie_merge(NODE **pnode, DIGIT pSDigit,
                NODE *node,   NODE *rest,    KEY *restPart)
{
    KEY *newKey, *target;
    KEY **refShare;
    NODE **refRight, **refLeft;
    NODE *leaf;
    DIGIT leafDigit;
    size_t lenTarget, lenNew, overlap;

    target = KEYCHAR(rest);
    lenTarget = strnlen(target, MAX_KEY_LEN); 
    if (node->SDigit == LSD) {
        overlap = 0;
    }
    else { /* if (node->SDigit > LSD) { */
        overlap = 1;
    }

    lenNew = lenTarget + lenTarget - overlap;
    newKey = malloc(sizeof(KEY) * (lenNew + 1));
    if (newKey == NULL) {
        return ERROR;
    }
    else {
        memcpy(newKey, restPart, lenTarget);
        memcpy(newKey + lenTarget - overlap, target, lenTarget);
        *(newKey + lenNew) = '\0';
    }
    
    switch(rest->type) {
        case LEAF:
            if ( *pnode == root) {
                leafDigit = WHOLE;
            }
            else {
                leafDigit = pSDigit - 1;
            }

            leaf = createLEAF(newKey, 0, leafDigit, rest->Val);
            if (leaf == NULL) {
                return ERROR;
            }
            else {
                free(node->Share);
                free(node);
                *pnode = leaf;

                return DELETED;
            }
        case INTERNAL:
            free(node->Share);
            refShare = &(node->Share);
            refRight = &(node->Right);
            refLeft = &(node->Left);

            *refShare = newKey;
            *refRight = rest->Right;
            *refLeft = rest->Left;

            node->SDigit = rest->SDigit;

            free(rest->Share);
            free(rest);

            return DELETED;
    }

    return ERROR;

}

int ptrie_restruct(NODE **pnode, DIGIT pSDigit, NODE *node,
                   ENTRY *en,    KEY *restKey)
{
    KEY *target, *key, *restPart;
    unsigned int fullBit, num_letter;
    unsigned char digit;
    size_t lenKey, lenTarget;
    NODE *next, *rest;
    NODE **parent;
    bool point;
    int status;

    key = en->key + en->num_letter;
    lenKey = strnlen(key, MAX_KEY_LEN);
    
    target = KEYCHAR(node);
    lenTarget = strnlen(target, MAX_KEY_LEN); 
    num_letter = 0;

    switch(node->type) {
        case LEAF: 
            fullBit = compareLEAF(target, node->LDigit, en);
            if (fullBit == ERROR) {
                return FAILURE;
            }
            else {
                num_letter = fullBit / (BIT_UNIT + 1);
                digit = (unsigned char)(fullBit % (BIT_UNIT + 1));
                if ( digit == 0 && (lenKey - num_letter) == 0 ) {
                    free(target);
                    free(node);
                    printf("done to free()@ptrie_restruct()\n");

                    return MATCHED;
                }
                else if ( digit > 0 || (lenKey - num_letter) > 0 ) {
                    return UNMATCHED;
                }
                else {
                    printf("ERROR@ptrie_restruct():%s\n", target);
                    return ERROR;
                }
            }
        case INTERNAL:
            fullBit = compareSHARE(target, node->SDigit, en);
            num_letter = fullBit / (BIT_UNIT + 1);
            digit = (unsigned char)(fullBit % (BIT_UNIT + 1));
 
            if ( digit == node->SDigit && (lenTarget - 1) == num_letter &&
                 (lenKey - num_letter) > 0 ) {
                if (digit > 0) {
                    point = pullout_bit(key + num_letter, digit);
                    if (digit == LSD) {
                        num_letter++;
                        digit = WHOLE;
                    }
                    else {
                        digit--;
                    }
                }
                else { /* if (digit == 0) { */
                    num_letter++;
                    point = pullout_bit(key + num_letter, MSD);
                    digit = MSD - 1;
                }
#ifdef DEBUG
                printf("point:%#3x@ptrie_restruct()\n", point);
#endif
                status = restoreShare(restKey, node,
                                      rest,    point);
                if (status == ERROR) {
                    perror("restoreShare()@ptrie_restruct()\n");
                    return FAILURE;
                }
#ifdef DEBUG
                printf("restKey:%s restKey + en->num_letter:%s lenShare:%lu@ptrie_restruct()\n", restKey, restKey + en->num_letter, lenTarget);
#endif


                switch(point) {
                    case LEFT:
                        next = node->Left;
                        parent = &(node->Left);
                        rest = node->Right;
                        break;
                    case RIGHT:
                        next = node->Right;
                        parent = &(node->Right);
                        rest = node->Left;
                        break;
                }
            
                en->num_letter += num_letter;
                en->digit = digit;
                status = ptrie_restruct(parent, pSDigit, next,
                                        en,     restKey);
                if (status == MATCHED) {
                    restPart = restKey + en->num_letter;
                    status = restoreShare(restPart, node,
                                        rest,     point);
                    if (status == ERROR) {
                        perror("restoreShare()@ptrie_restruct()\n");
                        return FAILURE;
                    }
                    else {
                        status = ptrie_merge(pnode, pSDigit, node,
                                             rest,  restPart);
                        if (status == ERROR) {
                            return status;
                        }
                        else {
                            return status;
                        }
                    }
                }

            }
            else if ( digit != node->SDigit && (lenTarget - 1) == num_letter &&
                      (lenKey - num_letter) > 0 ) {
                return UNMATCHED;
            }
            else if ( (lenTarget - 1) > num_letter &&
                      (lenKey - num_letter) > 0 ) {
                return UNMATCHED;
            }
            else {
                return FAILURE;
            }
    }

    return FAILURE;
}

int ptrie_delete(NODE **pnode, NODE *node, ENTRY *en)
{
    int status;
    KEY *restKey;
    size_t len;

    status = FAILURE;
    switch(node->type) {
        case LEAF:
            free(node->Key);
            free(node);

            *pnode = NULL;

            status = DELETED;
            break;
        case INTERNAL:
            len = strnlen(en->key, MAX_KEY_LEN);
            restKey = malloc(sizeof(KEY) * (len + 1));
            if (restKey == NULL) {
                perror("malloc()@ptrie_delete()\n");
                return FAILURE;
            }
            else {
                status = ptrie_restruct(pnode, (*pnode)->SDigit, node,
                                        en,     restKey);
                free(restKey);
                break;
            }
    }

    return status;
}

 
int ptrie_insert(ENTRY *en) {

    int status;

    if (root == NULL) {
        status = Initiate(en);
        if (status == ERROR) {
            return FAILURE;
        }
        else {
            return SUCCESS;
        }
    }
    else {
        status = createFork(&root, root, en);
        if (status == ERROR) {
            return FAILURE;
        }
        else {
            return SUCCESS;
        }
    }

}
