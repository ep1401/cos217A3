/* Contians the declarations for the ADT SymTable which provides
   the neccessary methods for a working symbol table. */

/* insures that the methods are only being defined once */
#ifndef HEADERFILE
#define HEADERFILE
#include <stddef.h>

/* a SympTable_T store bindings in a last in first out order */
typedef struct Table *SymTable_T;

/* Returns a new SymTable_T objects that contains
   no bindings, or NULL if insufficient memory is available.
   This function does not take any value as input. */
SymTable_T SymTable_new(void);

/* Does not return a value, but frees all memory
   occupied by the input oSymTable  */
void SymTable_free(SymTable_T oSymTable);

/* Returns a value of type size_t that is the number
   of bindings in the input oSymTable  */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Takes in a SymTable_T value, oSymTable, as input which will be
   modified to include the new binding if oSymtable does not
   already contain pcKey which is passed as input.
   This new binding will consist of the key pcKey and value
   pvValue which is also passed as input with an int 1 (TRUE)
   being returned. Otherwise if pcKey was found then
   oSymTable would remain unchanged and an int 0 (FALSE)
   would be returned. If insufficient memory is available, then the
   function leaves oSymTable unchanged and returns 0 (FALSE). */
int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue);

/* Takes in a SymTable_T value, oSymTable, as input which will
   be modified if it contains pcKey which is passed as input.
   If oSymTable is found to contain the pcKey, then the pvValue
   found within the oSymTable binding containing pcKey will be
   replaced by the new pvValue passed as input. The old binding's
   value will be returned as a void pointer. Otherwise if pcKey
   is not found then oSymTable will remain unchanged and NULL
   will be returned.  */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue);

/* Takes in a SymTable_T value, oSymTable, as input which will not
   be modified within the function. If oSymTable is found to contain
   the passed input pcKey, then the function will return an
   int 1 (TRUE). Otherwise the int 0 (FALSE) will be returned. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Takes in a SymTable_T value, oSymTable, as input which will not
   be modified within the function. If oSymTable is found to contain
   the passed input pcKey, then the function will return the value
   found within the binding containing pcKey as a void pointer.
   Otherwise if no such binding exist, NULL will be returned. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Takes in a SymTable_T value, oSymTable, as input. If oSymTable
   is found to contain pcKey, then the function will
   remove that binding from oSymTable and return the binding's
   value as a void pointer. Otherwise oSymTable will not be
   changed and NULL will be returned. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Takes in a SymTable_T value, oSymTable, as input. It then applies
   the function *pfApply, passed as input, to each binding
   in OsymTable, passing pvExtra, which is also passed as input,
   as an extra argument. That is for each binding within oSymTable
   call (*pfApply)(pcKey, pvValue, pvExtra). No value is returned. */
void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply) (const char *pcKey, void
                                   *pvValue, void *pvExtra),
                  const void *pvExtra);

#endif
