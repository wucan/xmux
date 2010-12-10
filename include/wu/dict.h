#ifndef _DICT_H_
#define _DICT_H_


typedef struct _DICT DICT;

typedef struct _WORD_ITEM {
	char *word;
	char *trans;
} WORD_ITEM;

DICT * dict_new (char *name);
void dict_destroy (DICT *dict);
int dict_register_worditem (DICT *dict, WORD_ITEM *worditem);
WORD_ITEM * dict_find_worditem (DICT *dict, char *word);


#endif /* _DICT_H_ */

