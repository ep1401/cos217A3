#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"


struct StackNode {
   const void *pvValue;

   const char *pcKey;

   struct StackNode *psNextNode;
};

struct Stack {

   size_t values;
   size_t size; 
};

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

SymTable_T SymTable_new(void){
   SymTable_T *oSymTable;
   int i; 

   oSymTable = (SymTable_T)realloc(509 *
                                  sizeof(struct Stack));

   if (oSymTable == NULL)
      return NULL;

   oSymTable->values = 0;
   oSymTable->size = 509;

   for (i = 0; i < 509; i++) {
      oSymTable[i]->psFirstNode = NULL; 
   }

   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
   
}

size_t SymTable_getLength(SymTable_T oSymTable){

}

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue) {

}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){

}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){

}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){

}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){

}

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply) (const char *pcKey, void
                                   *pvValue, void *pvExtra),
                  const void *pvExtra){

}
