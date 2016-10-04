
typedef struct DICTIONARY DICTIONARY;
typedef struct ENTRY ENTRY;
typedef struct KEY KEY;

DICTIONARY* new_dictionary(int);
void add_entry( DICTIONARY* , char , ENTRY* );
void init_dictionary (DICTIONARY* );
void clear_dictionary(DICTIONARY*);
void destroy_dictionary(DICTIONARY*);
unsigned int index_entry(ENTRY*);
char value_entry(ENTRY*);
ENTRY* find_entry (DICTIONARY*,char);