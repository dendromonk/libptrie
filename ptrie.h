#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#define bool    unsigned char
#define KEY     unsigned char
#define DATA    unsigned int
#define DIGIT   unsigned char

#define LEAF            0 
#define INTERNAL        1

#define LEFT            0
#define RIGHT           1

#define ASCII           8

#define BIT_UNIT        ASCII
#define MSD             ASCII       /* Most Significant Digit */
#define LSD             1           /* Least Significant Digit */
#define WHOLE           0           /* It means '9' in nonary. */

#define MAX_KEY_LEN     5000

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
            /* It means digit of critical(unmatched) bit.
             * Scope of "digit" is between 0(WHOLE) and 8(MSD) in ASCII.
             * And value of "WHOLE" represents that all bits effective.
             * (EX.)If (critical)digit == 0x01 then
             * bits of(8 digit ~ 2digit) is agreement.
             */
            unsigned char digit;    
            KEY *share;
            struct node *left;
            struct node *right;
        } internal;

        struct leaf_node {
            KEY *key;
            /* It means first digit of effective bit. */
            DIGIT digit;
            DATA val;
        } leaf;

    } unit;
} NODE;


typedef struct {
    KEY *key;
    unsigned int num_letter;
    DATA val;
    unsigned char digit;            /* it means top_digit. Top digit of key and
                                       top digit of Share engaes. */
    int status;
} ENTRY;


/* For Macro of LEAF Node */
#define Key unit.leaf.key
#define Val unit.leaf.val
#define LDigit unit.leaf.digit

/* For Macro of INTERNAL Node */
#define SDigit unit.internal.digit
#define Share unit.internal.share
#define Left  unit.internal.left
#define Right unit.internal.right

#define KEYCHAR(node) ((node->type) == LEAF ? (node->Key) : (node->Share))
#define REF_KEY(node) ((node->type) == LEAF ? &(node->Key) : &(node->Share))

#define TOP(x) (x == WHOLE ? (BIT_UNIT - 1) : (x - 1))
#define BOTTOM(x) (x - 1)

#define MASK(digit)  (255 - ((1 << digit) - 1))

#define PINBIT(digit) (255 - (1 << (digit - 1)))   
 
int reSizeKEY(NODE *node, size_t start_point, unsigned char node_type);
unsigned int compare_letter(KEY *key1, KEY *key2, size_t len);
unsigned char compare_bit(KEY *key1, DIGIT bottom, KEY *key2, DIGIT top);
unsigned int compareSHARE(KEY *share, DIGIT shareDigit, ENTRY *en);
int compareLEAF(KEY *leafkey, DIGIT leafDigit, ENTRY *en);
int Initiate(ENTRY *en);
bool pullout_bit(KEY *key1, int digit);
KEY* createSHARE(KEY *key, int num_letter);
NODE* createLEAF(KEY *key, size_t start_point, DIGIT digit, DATA val);
NODE* createINTERNAL(KEY *left, unsigned int start_point, DIGIT digit);
int createFork(NODE **pnode, NODE *node, ENTRY *en);
ENTRY* createENTRY(KEY *key, DIGIT digit, DATA val, int status);
ENTRY* ptrie_search(NODE *node, ENTRY *en);
int ptrie_restruct(NODE **pnode, DIGIT pSDigit,  NODE *node,
                   ENTRY *en,    KEY *restKey);
int ptrie_merge(NODE **pnode, DIGIT pSDigit, NODE *node,
                NODE *rest,   KEY *restPart);
int ptrie_delete(NODE **pnode, NODE *node, ENTRY *en);
int ptrie_insert(ENTRY *en);

