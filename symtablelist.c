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
   struct StackNode *psFirstNode;
   size_t values;
};

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct Stack));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->psFirstNode = NULL;
   oSymTable->values = 0;
   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;

   assert(oSymTable != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {
      psNextNode = psCurrentNode->psNextNode;
      free(psCurrentNode);
   }
   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
   assert(oSymTable != NULL);
   return oSymTable->values;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue){
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   if(!SymTable_contains(oSymTable, pcKey)){
      struct StackNode *psNewNode;
      char *pcKeySave;
      psNewNode = (struct StackNode*)malloc(sizeof(struct StackNode));
      if (psNewNode == NULL)
         return 0;
      pcKeySave = malloc(strlen(pcKey) + 1);
      if (pcKeySave == NULL)
         return 0;
      pcKeySave = strcpy(pcKeySave, pcKey);

      psNewNode->pvValue = pvValue;
      psNewNode->pcKey = pcKeySave;
      psNewNode->psNextNode = oSymTable->psFirstNode;
      oSymTable->psFirstNode = psNewNode;
      oSymTable->values++;
      return 1;
   }
   return 0;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {

      if (strcmp(psCurrentNode->pcKey, pcKey) == 0) {
         const void *pvValueSave = psCurrentNode->pvValue;
         psCurrentNode->pvValue = pvValue;
         return (void*)pvValueSave;
      }

      psNextNode = psCurrentNode->psNextNode;
   }
   return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {
      if (strcmp(psCurrentNode->pcKey, pcKey) == 0)
         return 1;
      psNextNode = psCurrentNode->psNextNode;
   }
   return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {
      if (strcmp(psCurrentNode->pcKey, pcKey) == 0)
         return (void*)psCurrentNode->pvValue;
      psNextNode = psCurrentNode->psNextNode;
   }
   return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;
   struct StackNode *psPreviousNode;
   const void *nodeValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   psPreviousNode = NULL;

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {

      if (strcmp(psCurrentNode->pcKey, pcKey) == 0) {
         if (psPreviousNode == NULL) {
            oSymTable->psFirstNode = psCurrentNode->psNextNode;
         }
         else {
            psPreviousNode->psNextNode = psCurrentNode->psNextNode;
         }
         nodeValue = psCurrentNode->pvValue;
         free(psCurrentNode);
         oSymTable->values--;
         return (void*)nodeValue;
      }

      psNextNode = psCurrentNode->psNextNode;
   }
   return NULL;
}

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply) (const char *pcKey, void
                                   *pvValue, void *pvExtra),
                  const void *pvExtra){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {

      pfApply(psCurrentNode->pcKey,(void*)psCurrentNode->pvValue,
              (void*)pvExtra);

      psNextNode = psCurrentNode->psNextNode;
   }
}
