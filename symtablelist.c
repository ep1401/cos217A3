/* implements the SymTable Linked List version*/

/* includes libraries*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/* Each item is stored in a Binding.
   Bindings are linked to form a Table*/
struct Binding {
   /* stores the value of the binding */
   const void *pvValue;

   /* stores the key of the binding */
   const char *pcKey;

   /* The address of the next Binding*/
   struct Binding *psNextBinding;
};

/* Table is a structure that points to the
   first Binding. It also stores the number of
   elements contained within the Table */
struct Table {
   /* The address of the first Binding*/
   struct Binding *psFirstBinding;

   /* The number of elements contained within
      the Table */
   size_t tableInputs;
};

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;

   /* intilizes the size of oSymTable to be the same size as
      the Table struct */
   oSymTable = (SymTable_T)malloc(sizeof(struct Table));

   /* checks to see if malloc failed */
   if (oSymTable == NULL)
      return NULL;

   /* sets the first node to be NULL intially representing
      an empty SymTable_T */
   oSymTable->psFirstBinding = NULL;

   /* sets the number of inputs to be 0 intially representing
      an empty SymTable_T */
   oSymTable->tableInputs = 0;

   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
   struct Binding *psCurrentBinding;
   struct Binding *psNextBinding;

   /* ensures no null input */
   assert(oSymTable != NULL);

   /* iterates through oSymTable freeing all memory */
   for (psCurrentBinding = oSymTable->psFirstBinding;
        psCurrentBinding != NULL;
        psCurrentBinding = psNextBinding) {
      /* maintains access to the next binding */
      psNextBinding = psCurrentBinding->psNextBinding;

      /* frees all memory wihtin the binding */
      free((void *)psCurrentBinding->pcKey);
      free(psCurrentBinding);
   }
   /* frees the memory created for the oSymTable structure */
   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
   /* ensures no null input */
   assert(oSymTable != NULL);

   /* returns the number of inputs wihtin oSymTable */
   return oSymTable->tableInputs;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue){
   /* ensures no null input where unexpected */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* checks if oSymTable already contains pcKey */
   if(!SymTable_contains(oSymTable, pcKey)){
      struct Binding *psNewBinding;
      char *pcKeySave;

      /* allocates memory for which the new Binding will reside */
      psNewBinding = (struct Binding*)malloc(sizeof(struct Binding));

      /* checks to see if malloc failed */
      if (psNewBinding == NULL)
         /* returns 0 representing that their was
            insufficeint memory */
         return 0;

      /* allocates memory for which the defensive key will reside */
      pcKeySave = malloc(strlen(pcKey) + 1);

      /* checks to see if malloc failed */
      if (pcKeySave == NULL)
         /* returns 0 representing that their was
            insufficeint memory */
         return 0;

      /* copies the key into allocated memory allowing a
         defensive copy to be stored */
      strcpy(pcKeySave, pcKey);

      /* saves the value and defensive key into the binding */
      psNewBinding->pvValue = pvValue;
      psNewBinding->pcKey = pcKeySave;

      /* sets the first binding within oSymTable to be the binding
         just created */
      psNewBinding->psNextBinding = oSymTable->psFirstBinding;
      oSymTable->psFirstBinding = psNewBinding;

      /* increments the number of inputs stored in oSymTable */
      oSymTable->tableInputs++;

      /* returns 1 representing that a new binding was added*/
      return 1;
   }
   /* returns 0 representing that pcKey was already found within
      oSymTable so no new binding was added*/
   return 0;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){
   struct Binding *psCurrentBinding;

   /* ensures no null input where unexpected */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* iterates through oSymTable until the binding with pcKey is
      found or the end is reached */
   for (psCurrentBinding = oSymTable->psFirstBinding;
        psCurrentBinding != NULL;
        psCurrentBinding = psCurrentBinding->psNextBinding) {

      /* checks if the inputed pcKey is equal to the pcKey within
         the bounding */
      if (strcmp(psCurrentBinding->pcKey, pcKey) == 0) {
         /* replaces the binding's value with pvValue */
         const void *pvValueSave = psCurrentBinding->pvValue;
         psCurrentBinding->pvValue = pvValue;

         /* returns the old pvValue */
         return (void*)pvValueSave;
      }
   }
   return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   struct Binding *psCurrentBinding;

   /* ensures no null input */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* iterates through oSymTable until the binding with pcKey is
      found or the end is reached */
   for (psCurrentBinding = oSymTable->psFirstBinding;
        psCurrentBinding != NULL;
        psCurrentBinding = psCurrentBinding->psNextBinding) {
      /* checks if the inputed pcKey is equal to the pcKey within
         the bounding */
      if (strcmp(psCurrentBinding->pcKey, pcKey) == 0)
         /* returns 1 representing that pcKey was found */
         return 1;
   }
   /* returns 0 representing that pcKey was not found */
   return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   struct Binding *psCurrentBinding;

   /* ensures no null input */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* iterates through oSymTable until the binding with pcKey is
      found or the end is reached */
   for (psCurrentBinding = oSymTable->psFirstBinding;
        psCurrentBinding != NULL;
        psCurrentBinding = psCurrentBinding->psNextBinding) {
      /* checks if the inputed pcKey is equal to the pcKey within
         the bounding */
      if (strcmp(psCurrentBinding->pcKey, pcKey) == 0)
         /* returns the binding of the binding whose key is pcKey */
         return (void*)psCurrentBinding->pvValue;
   }
   /* returns NULL representing that no such binding exists*/
   return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   struct Binding *psCurrentBinding;
   struct Binding *psPreviousBinding;

   /* ensures no null input */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   psPreviousBinding = NULL;

   /* iterates through oSymTable until the binding with pcKey is
      found or the end is reached */
   for (psCurrentBinding = oSymTable->psFirstBinding;
        psCurrentBinding != NULL;
        psCurrentBinding = psCurrentBinding->psNextBinding) {
      /* checks if the inputed pcKey is equal to the pcKey within
         the bounding */
      if (strcmp(psCurrentBinding->pcKey, pcKey) == 0) {
         const void *bindingValue;

         /* checks to see if the previous binding is NULL
            representing that this is the first binding
            in oSymTable */
         if (psPreviousBinding == NULL) {
            /* updates the first binding withing oSymTable to
               be the one immediately following the first*/
            oSymTable->psFirstBinding =
               psCurrentBinding->psNextBinding;
         }
         else {
            /* removes the current binding by linking the previous
               binding with the next binding */
            psPreviousBinding->psNextBinding =
               psCurrentBinding->psNextBinding;
         }
         /* stores the removed bindings value */
         bindingValue = psCurrentBinding->pvValue;

         /* decrements the number of inputs within oSymTable */
         oSymTable->tableInputs--;

         /* frees all memory within the current binding */
         free((void *)psCurrentBinding->pcKey);
         free(psCurrentBinding);

         return (void*)bindingValue;
      }
      psPreviousBinding = psCurrentBinding;
   }
   return NULL;
}

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply) (const char *pcKey, void
                                   *pvValue, void *pvExtra),
                  const void *pvExtra){
   struct Binding *psCurrentBinding;

   /* ensures no null input where unexpected */
   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   /* iterates through oSymTable until the end is reached */
   for (psCurrentBinding = oSymTable->psFirstBinding;
        psCurrentBinding != NULL;
        psCurrentBinding = psCurrentBinding->psNextBinding) {

      /* applies function *pfApply to each binding in oSymtable
         passing pvExtra as an extra parameter*/
      (*pfApply)(psCurrentBinding->pcKey,
                 (void*)psCurrentBinding->pvValue,
                 (void*)pvExtra);
   }
}
