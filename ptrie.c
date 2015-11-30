#include "ptrie.h"

NODE *root = NULL;

NODE* createLEAF(KEY *key, size_t start_point, DATA val) {

    NODE *leaf;
    size_t len;
    const char *src = key + start_point;

    leaf = malloc(sizeof(NODE));
    if (leaf == NULL) {
        perror("malloc in createLEAF()");
        return NULL;
    }
    else {

        leaf->type = LEAF;

        len = strnlen(key + start_point, MAX_KEY_LEN);
        leaf->Key = malloc(sizeof(KEY)*(len + 1));
        if (leaf->Key == NULL) {
            perror("malloc()createLEAF()");
            return NULL;
        }
        else {
            strlcpy(leaf->Key, key + start_point, len + 1);
            
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
    oldKey = malloc(sizeof(NODE) * (oldLen + 1));
    if (oldKey == NULL) {
        perror("malloc@reSizeKEY()");
        return ERROR;
    }
    else {
        strlcpy(oldKey, *target, oldLen + 1);

        newLen = strnlen(*target + start_point, MAX_KEY_LEN);
        tmp = realloc(*target, sizeof(KEY) * (newLen + 1));
        if (tmp == NULL) {
            perror("realloc@reSizeKEY()");
            return ERROR;
        }
        else {
            *target = tmp;
            strlcpy(*target, oldKey + start_point, newLen + 1);

            free(oldKey);
            return SUCCESS;
        }
    }
}

unsigned int compare_letter(KEY *key1, KEY *key2, size_t len)
{
    unsigned int i;

    i = 0;
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

unsigned char compare_bit(KEY *key1, KEY *key2, int thresh)
{
    KEY bitKey1, bitKey2, mask;
    int i;
    unsigned char digit;

    for (i = (MSD - 1); i >= (thresh - 1); i--) {
        mask = 1 << i;
        bitKey1 = (*key1) & mask;
        bitKey2 = (*key2) & mask;
        if (bitKey1 != bitKey2) {
            break;
        }
        else {
            continue;
        }
    }

    if (i < 0) {
        digit = 0;
    }
    else {
        digit = (unsigned char)++i;
    }

    return digit;
}

unsigned int compareSHARE(KEY *share, unsigned char baseDigit, KEY *key)
{
    unsigned  char digit;
    unsigned int num_letter, fullBit;
    int status;
    size_t len, len1, len2;

    len1 = strnlen(share, MAX_KEY_LEN) - 1;
    len2 = strnlen(key, MAX_KEY_LEN);
    if (len1 < len2) {
        len = len1;
    }
    else {
        len = len2;
    }

    num_letter = compare_letter(share, key, len);
    
    digit = compare_bit(share + num_letter, key + num_letter, baseDigit);

    fullBit = (unsigned int)digit + (BIT_UNIT * num_letter);

    return fullBit;
}



unsigned int compareLEAF(KEY *key1, KEY *key2)
{
    unsigned  char digit;
    unsigned int num_letter, fullBit;
    int status;
    size_t len, len1, len2;

    len1 = strnlen(key1, MAX_KEY_LEN);
    len2 = strnlen(key2, MAX_KEY_LEN);
    if (len1 < len2) {
        len = len1;
    }
    else {
        len = len2;
    }

    num_letter = compare_letter(key1, key2, len);
    if (num_letter < len) {
        digit = compare_bit(key1 + num_letter, key2 + num_letter, LSD);
    }
    else {
        digit = 0;
    }

    fullBit = (unsigned int)digit + (BIT_UNIT * num_letter);

    return fullBit;
}


int Initiate(ENTRY *en) {

    NODE *first;
    size_t len;

    len = strnlen(en->key, MAX_KEY_LEN);
    first = createLEAF(en->key, 0, en->val);
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
        strlcpy(share, key, num_letter + 1); 
        return share;
    }

    return NULL;
}

NODE* createINTERNAL(KEY *left, unsigned int num_letter, unsigned char digit) {

    NODE *new;
    
    new = malloc(sizeof(NODE));
    if (new == NULL) {
        perror("malloc@createINTERNAL()");
        return NULL;
    }
    else {
        new->type = INTERNAL;
        new->Digit = (unsigned char)(digit + 1);
        new->Share = createSHARE(left, num_letter + 1);

        new->Left = NULL;
        new->Right = NULL;

        return new;
    }
}

int createFork(NODE **pnode, NODE *node, KEY *key, DATA val) {
    NODE *new, *next;
    NODE **parent;
    KEY **target;
    unsigned int  num_letter, fullBit;
    unsigned char digit;
    int status;
    bool point;
    size_t len, tail_point;
    
    target = REF_KEY(node);
    len = strnlen(key, MAX_KEY_LEN);

    switch(node->type) {
        case LEAF: 
            fullBit = compareLEAF(*target, key);
            num_letter = fullBit / BIT_UNIT;
            digit = (unsigned char)fullBit % BIT_UNIT;
            if (digit == 0 && (len - num_letter) == 0) {
                status =  MATCHED;
                break;
            }
            else if (digit > 0 && (len - num_letter) > 0) {
                status =  UNMATCHED;
                break;
            }
            else {
                status = ERROR;
                break;
            }
        case INTERNAL:
            fullBit = compareSHARE(*target, node->Digit, key);
            num_letter = fullBit / BIT_UNIT;
            digit = (unsigned char)fullBit % BIT_UNIT;
            if ( (digit + 1) == node->Digit && (len - num_letter) > 0 ) {
                point = pullout_bit(key + num_letter, digit);
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
            
                key += num_letter;
                status = createFork(parent, next, key, val);

                return status;

            }
            else if (digit >= node->Digit && (len - num_letter) > 0) {
                /* createINTERNAL */
                status = UNMATCHED;
                break;
            }
            else {
                status = ERROR;
                break;
            }
        default:
            return FAILURE;
    }

    
   if (status == MATCHED) {
        return MATCHED;
   }
   else if (status == UNMATCHED) {
       new = createINTERNAL(*target, num_letter, digit);
       len = strnlen(new->Share, MAX_KEY_LEN);
       tail_point = len - 1;
       point = pullout_bit(key + tail_point, digit);

       status = reSizeKEY(node, tail_point, node->type);
       if (status == ERROR) {
            perror("reSizeKEY@createFork()");
            return ERROR;
        }
        else {
            switch(point) {
                case LEFT:
                    new->Left = createLEAF(key, tail_point, val);
                    new->Right = node;
                    break;
                case RIGHT:
                    new->Right = createLEAF(key, tail_point, val);
                    new->Left = node;
                    break;
            }

            *pnode = new;

            return APPEND;
        }
   }
   else {
       return ERROR;
   }

}


ENTRY* createENTRY(KEY *key, DATA val, int status) {
    
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
            strlcpy(en->key, key, MAX_KEY_LEN);
            en->val = val;
            en->status = status;

            return en;
        }
    }

    return NULL;
}


ENTRY* search(NODE *node, KEY *key) {
    
    KEY *target;
    unsigned int fullBit, num_letter;
    unsigned char digit;
    size_t len;
    NODE *next;
    ENTRY *result;
    bool point;
    int status;

    target = KEYCHAR(node);
    len = strnlen(key, MAX_KEY_LEN);
    
    switch(node->type) {
        case LEAF: 
            fullBit = compareLEAF(target, key);
            num_letter = fullBit / BIT_UNIT;
            digit = (unsigned char)fullBit % BIT_UNIT;
            if (digit == 0 && (len - num_letter) == 0) {
                status = MATCHED;
                break;
            }
            else if (digit > 0 && (len - num_letter) > 0) {
                status = UNMATCHED;
                break;
            }
            else {
                status = ERROR;
                break;
            }
        case INTERNAL:
            fullBit = compareSHARE(target, node->Digit, key);
            num_letter = fullBit / BIT_UNIT;
            digit = (unsigned char)fullBit % BIT_UNIT;
            if ( (digit + 1) == node->Digit && (len - num_letter) > 0 ) {
                point = pullout_bit(key + num_letter, digit);
                switch(point) {
                    case LEFT:
                        next = node->Left;
                        break;
                    case RIGHT:
                        next = node->Right;
                        break;
                }
            
                key += num_letter;
                result = search(next, key);

                return result;

            }
            else if (digit >= node->Digit && (len - num_letter) > 0) {
                /* createINTERNAL */
                status = UNMATCHED;
                break;
            }
            else {
                status = ERROR;
                break;
            }
        default:
            return NULL;
    }

    if (status == MATCHED) {
        result = createENTRY(node->Key, node->Val, MATCHED);
        return result;
    }
    else if (status == UNMATCHED) {
        switch(node->type) {
            case LEAF:
                result = createENTRY(node->Key, node->Val, UNMATCHED);
                return result;
            case INTERNAL:
                result = createENTRY(node->Share, node->Digit, UNMATCHED);
                return result;
        }
    }
    else {
        return NULL;
    }


    return NULL;
}

int restruct(NODE **pnode, NODE *node, NODE *rest) {
    KEY *newKey, *target;
    KEY **refShare;
    NODE **refRight, **refLeft;
    NODE *leaf;
    size_t lenKey, lenShare;

    target = KEYCHAR(rest);
    lenKey = strnlen(target, MAX_KEY_LEN); 
    lenShare = strnlen(node->Share, MAX_KEY_LEN);
    if (node->Digit > 0) {
        lenShare--;
        newKey = createSHARE(node->Share, lenShare + lenKey);
        if (newKey == NULL) {
            return ERROR;
        }
        else {
            memcpy(newKey + lenShare, target, lenKey + 1);
        }
    }
    else {
        newKey = createSHARE(node->Share, lenShare + lenKey);
        if (newKey == NULL) {
            return ERROR;
        }
        else {
            strncat(newKey, target, lenKey);
        }
    }
    
    switch(rest->type) {
        case LEAF:
            leaf = createLEAF(newKey, 0, rest->Val);
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
            refShare = &(node->Share);
            refRight = &(node->Right);
            refLeft = &(node->Left);

            *refShare = newKey;
            *refRight = rest->Right;
            *refLeft = rest->Left;

            free(rest->Share);
            free(rest);

            return DELETED;
    }

    return ERROR;

}

int delete(NODE **pnode, NODE *node, KEY *key) {
    
    KEY *target, *newKey;
    KEY **refKEY;
    unsigned int fullBit, num_letter;
    unsigned char digit;
    size_t len, lenShare, lenKey;
    NODE *next, *rest;
    NODE **parent;
    ENTRY *result;
    bool point;
    int status;

    len = strnlen(key, MAX_KEY_LEN);
    
    switch(node->type) {
        case LEAF: 
            target = node->Key;
            fullBit = compareLEAF(target, key);
            num_letter = fullBit / BIT_UNIT;
            digit = (unsigned char)fullBit % BIT_UNIT;
            if (digit == 0 && (len - num_letter) == 0) {
                free(target);
                free(node);
                return MATCHED;
            }
            else if (digit > 0 && (len - num_letter) > 0) {
                return UNMATCHED;
            }
            else {
                return ERROR;
            }
        case INTERNAL:
            target = KEYCHAR(node);
            fullBit = compareSHARE(target, node->Digit, key);
            num_letter = fullBit / BIT_UNIT;
            digit = (unsigned char)fullBit % BIT_UNIT;
            if ( (digit + 1) == node->Digit && (len - num_letter) > 0 ) {
                point = pullout_bit(key + num_letter, digit);
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
            
                key += num_letter;
                status = delete(parent, next, key);
                if (status == MATCHED) {
                    status = restruct(pnode, node, rest);
                    return status;
                }
                else {
                    return status;
                }

            }
            else if (digit >= node->Digit && (len - num_letter) > 0) {
                /* createINTERNAL */
                return UNMATCHED;
            }
            else {
                return ERROR;
            }
    }

    return ERROR;
}

 
int insert(ENTRY *en) {

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
        status = createFork(&root, root, en->key, en->val);
        if (status == ERROR) {
            return FAILURE;
        }
        else {
            return SUCCESS;
        }
    }

}

