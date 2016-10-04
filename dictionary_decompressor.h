
typedef struct DICTIONARY DICTIONARY;
typedef struct ENTRY ENTRY;

DICTIONARY* new_dictionary(int);
void add_entry( DICTIONARY* , char , int );
void init_dictionary (DICTIONARY* );
void clear_dictionary(DICTIONARY*);
void destroy_dictionary(DICTIONARY*);
ENTRY* find_entry (DICTIONARY*,int);
char* find_code(DICTIONARY*,int);