#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#define bool    unsigned char
#define KEY     char
#define DATA    unsigned int

#define LEAF            0 
#define INTERNAL        1

#define LEFT            0
#define RIGHT           1

#define ASCII           8

#define BIT_UNIT        ASCII
#define MSD             ASCII   /* Most Significant Digit */
#define LSD             1       /* Least Significant Digit */

#define MAX_KEY_LEN     255

#define SUCCESS         0
#define FAILURE         -1

#define ERROR           -2

#define MATCHED         1
#define APPEND          2

#define CREATED         3
#define UNMATCHED       4

#define DELETED         5

typedef struct node {
    bool type;
    union {

        struct in_node {
            /* Scope of "digit" is between 1(LSD) and 8(MSD) in
             * ASCII of BIT_UNIT.
             */
            unsigned char digit;
            KEY *share;
            struct node *left;
            struct node *right;
        } internal;

        struct leaf_node {
            KEY *key;
            DATA val;
        } leaf;

    } unit;
} NODE;


typedef struct {
    KEY *key;
    DATA val;
    int status;
} ENTRY;


/* For Macro of LEAF Node */
#define Key unit.leaf.key
#define Val unit.leaf.val

/* For Macro of INTERNAL Node */
#define Digit unit.internal.digit
#define Share unit.internal.share
#define Left  unit.internal.left
#define Right unit.internal.right

#define KEYCHAR(node) ((node->type) == LEAF ? (node->Key) : (node->Share))
#define REF_KEY(node) ((node->type) == LEAF ? &(node->Key) : &(node->Share))

NODE* createLEAF(KEY *key, size_t start_point, DATA val);
int reSizeKEY(NODE *node, size_t start_point, unsigned char node_type);
unsigned int compare_letter(KEY *key1, KEY *key2, size_t len);
unsigned char compare_bit(KEY *key1, KEY *key2, int thresh);
unsigned int compareSHARE(KEY *share, unsigned char baseDigit, KEY *key);
unsigned int compareLEAF(KEY *key1, KEY *key2);
int Initiate(ENTRY *en);
bool pullout_bit(KEY *key1, int digit);
KEY* createSHARE(KEY *key, int num_letter);
NODE* createINTERNAL(KEY *left, unsigned int num_letter, unsigned char digit);
int createFork(NODE **pnode, NODE *node, KEY *key, DATA val);
ENTRY* createENTRY(KEY *key, DATA val, int status);
ENTRY* search(NODE *node, KEY *key);
int restruct(NODE **pnode, NODE *node, NODE *rest);
int delete(NODE **pnode, NODE *node, KEY *key);
int insert(ENTRY *en);

