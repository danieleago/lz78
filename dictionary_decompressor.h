
typedef struct DICTIONARY DICTIONARY;

DICTIONARY* new_dictionary(int);
void add_entry( DICTIONARY* , char , int );
void init_dictionary (DICTIONARY* );
void clear_dictionary(DICTIONARY*);
void destroy_dictionary(DICTIONARY*);