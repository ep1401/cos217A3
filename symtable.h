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
   occupied by the input oSymTable which is of 
   type SymTable_T.  */
void SymTable_free(SymTable_T oSymTable);

/* Returns a value of type size_t that is the number
   of bindings in the input oSymTable which is of 
   type SymTable_T.  */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Takes in a SymTable_T value, oSymTable, as input which will be 
   modified to include the new binding if oSymtable does not 
   already contains pcKey which is passed as input to the 
   function as a const char pointer. This new binding will consist
   of key pcKey and value pvValue which is also passed as input as a 
   const void pointer. If  pcKey was not found already in 
   oSymTable, then a new binding will be added consisting 
   of key pcKey and value pvValue with an int 1 being returned.
   Otherwise if pcKey was found then oSymTable would remain
   unchanged and an int 0 would be returned.  
   If insufficient memory is available, then the 
   function leaves oSymTable unchanged and returns 0. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue);

/* Does not return a value. Takes in a SymTable_T value, 
   oSymTable, as input which will be modified if it contains
   pcKey which is passed as input to the function as a
   const char pointer. If oSymTable is found to contain the 
   pcKey, then the pvValue found with the oSymTable
   binding containing pcKey will be replaced by a new pvValue 
   which is also passed as input as a const void pointer and the
   old value will be returned. Otherwise oSymTable will remain
   unchanged and NULL will be returned.  */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue);

/* Takes in a SymTable_T value, oSymTable, as input which will not
   be modified within the function. If oSymTable is found to contain
   pcKey, which is passed as input to the function as a
   const char pointer, then the function will return an int 1.  
   Otherwise the int 0 will be returned. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Takes in a SymTable_T value, oSymTable, as input which will not
   be modified within the function. If oSymTable is found to contain
   pcKey, which is passed as input to the function as a
   const char pointer, then the function will return a void pointer
   of the value found within the binding containing pcKey.  
   Otherwise NULL will be returned if no such binding exists. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Takes in a SymTable_T value, oSymTable, as input. If oSymTable 
   is found to contain pcKey, which is passed as input to the
   function as a const char pointer, then the function will
   remove that binding from oSymTable and return the binding's 
   value as a void pointer. Otherwise the function will not change 
   oSymTable and NULL will be returned. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Takes in a SymTable_T value, oSymTable, as input. It then applies
   the function *pfApply which is also passed as input to each binding
   in OsymTable, passing pvExtra as an extra parameter.  pvExtra is
   passed into the function as well a const void pointer.
   No value is returned. */
void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply) (const char *pcKey, void
                                   *pvValue, void *pvExtra),
                  const void *pvExtra);

#endif
