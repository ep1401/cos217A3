#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

enum{BUCKET1 = 509, BUCKET2 = 1021, BUCKET3 = 2039, BUCKET4 = 4093,
   BUCKET5 = 8191, BUCKET6 = 16381, BUCKET7 = 32749,
   BUCKET8 = 65521};

struct StackNode {
   const void *pvValue;

   const char *pcKey;

   struct StackNode *psNextNode;
};

struct Stack {
   size_t values;
   int size;
   struct StackNode **bucket;
   int bucketIndex;
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

static SymTable_T SymTable_resize(SymTable_T oSymTable) {
   struct StackNode **newBucket;
   int bucketArray[] = {BUCKET1, BUCKET2, BUCKET3, BUCKET4, BUCKET5,
      BUCKET6, BUCKET7, BUCKET8};
   int i;
   int newSize;

   assert(oSymTable != NULL);

   if (oSymTable->size == BUCKET8)
      return oSymTable;


   newSize = bucketArray[oSymTable->bucketIndex + 1];

   newBucket = (struct StackNode**)
      malloc(newSize * sizeof(struct StackNode));

   if (newBucket == NULL)
      return NULL;

   for (i = 0; i < newSize; i++) {
      newBucket[i] = NULL;
   }

   for (i = 0; i < oSymTable->size; i++) {
      struct StackNode *currentIndex = oSymTable->bucket[i];
      size_t hash;
      struct StackNode *psCurrentNode;
      struct StackNode *psNextNode;

      for (psCurrentNode = oSymTable->bucket[i];
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {

         hash = SymTable_hash(psCurrentNode->pcKey, newSize);
         currentIndex->psNextNode = newBucket[hash];
         newBucket[hash] = currentIndex;
         psNextNode = psCurrentNode->psNextNode;
      }
   }
   
   free(oSymTable->bucket);
   oSymTable->bucket = newBucket;
   oSymTable->size = newSize;

   return oSymTable;

}

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   size_t i;

   oSymTable = (SymTable_T)malloc(sizeof(struct Stack));

   if (oSymTable == NULL)
      return NULL;

   oSymTable->values = 0;
   oSymTable->size = BUCKET1;
   oSymTable->bucketIndex = 0;

   oSymTable->bucket = (struct StackNode**)
      malloc(BUCKET1 * sizeof(struct StackNode));

   for (i = 0; i < BUCKET1; i++) {
      oSymTable->bucket[i] = NULL;
   }

   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;
   int i;

   assert(oSymTable != NULL);

   for (i = 0; i < oSymTable->size; i++) {
      for (psCurrentNode = oSymTable->bucket[i];
           psCurrentNode != NULL;
           psCurrentNode = psNextNode) {
         psNextNode = psCurrentNode->psNextNode;
         free(psCurrentNode);
      }
   }

   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
   assert(oSymTable != NULL);
   return oSymTable->values;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue) {
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   if(!SymTable_contains(oSymTable, pcKey)){
      struct StackNode *psNewNode;
      char *pcKeySave;
      size_t hash;
      psNewNode = (struct StackNode*)malloc(sizeof(struct StackNode));
      if (psNewNode == NULL)
         return 0;
      pcKeySave = malloc(strlen(pcKey) + 1);
      if (pcKeySave == NULL)
         return 0;
      pcKeySave = strcpy(pcKeySave, pcKey);
      oSymTable->values++;

      if ((int) oSymTable->values > oSymTable->size){
         oSymTable = SymTable_resize(oSymTable);
      }
      hash = SymTable_hash(pcKey, oSymTable->size);

      psNewNode->pvValue = pvValue;
      psNewNode->pcKey = pcKeySave;
      psNewNode->psNextNode = oSymTable->bucket[hash];
      oSymTable->bucket[hash] = psNewNode;
      return 1;
   }
   return 0;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->size);

   for (psCurrentNode = oSymTable->bucket[hash];
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
   size_t hash;


   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->size);

   for (psCurrentNode = oSymTable->bucket[hash];
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
   size_t hash;


   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->size);


   for (psCurrentNode = oSymTable->bucket[hash];
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
   size_t hash;


   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->size);


   psPreviousNode = NULL;

   for (psCurrentNode = oSymTable->bucket[hash];
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {

      if (strcmp(psCurrentNode->pcKey, pcKey) == 0) {
         if (psPreviousNode == NULL) {
            oSymTable->bucket[hash] = psCurrentNode->psNextNode;
         }
         else {
            psPreviousNode->psNextNode = psCurrentNode->psNextNode;
         }
         nodeValue = psCurrentNode->pvValue;
         free(psCurrentNode);
         oSymTable->values--;
         return (void*)nodeValue;
      }
      psPreviousNode = psCurrentNode;
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
   size_t hash;


   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (psCurrentNode = oSymTable->bucket[0];
        psCurrentNode != NULL;
        psCurrentNode = psNextNode) {


      pfApply(psCurrentNode->pcKey,(void*)psCurrentNode->pvValue,
              (void*)pvExtra);


      psNextNode = psCurrentNode->psNextNode;
   }
}
