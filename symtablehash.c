/* implements the SymTable Hash Table version*/

/* includes libraries*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/* array that stores the available bucket sizes */
static const size_t bucketArray[] = {509, 1021, 2039, 4093, 8191,
   16381, 32749, 65521};

/* stores the length of bucketArray */
static const size_t numBucketCounts = sizeof(bucketArray)
   /sizeof(bucketArray[0]);

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

/* Table is a structure that contains a pointer to a pointer
   allowing access to all of the Bindings
   within the Table. It also stores the number of
   elements contained within the Table and the
   index to bucketArray that represents the bucket size */
struct Table {
   /* The number of elements contained within
      the Table */
   size_t tableInputs;

   /* pointer to a pointer of all the bindings*/
   struct Binding **buckets;

   /* idex to the bucketArray containing the bucket size*/
   int bucketIndex;
};

/* Hash function ensuring that bindings inserted in the table
   are evenely distrubeted throughout the buckets. The function
   takes in a key, pcKey, and finds the appropriate
   bucket to insert the pcKey in. The bucket that pcKey is inserted
   within is determined through calculation with the
   input uBucketCount. It returns a hash code of type size_t for
   pcKey that is between 0 and uBucketCount - 1 inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   /* ensures no null input */
   assert(pcKey != NULL);

   /* calulates the hash value */
   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/* Takes in an SymTable_T as input, oSymTable, that will be
   modified to increase in size. The new size will be
   equal to the value located in the next increased index of
   bucketArray. oSymTable will be modified to include the
   new buckets with all of its previous bindings being
   rehashed. oSymTable will be returned */
static SymTable_T SymTable_resize(SymTable_T oSymTable) {
   struct Binding **newBucket;
   struct Binding *psCurrentBinding;
   struct Binding *psNextBinding;
   size_t hash;
   /* for loop variable */
   size_t i;

   size_t newSize;

   /* ensures no null input */
   assert(oSymTable != NULL);

   /* ensures that the bucketArray is not indexed out of its bounds.
      While this is checked within the SymTable_put function it is
      beneficial to also include here to ensure that we
      do not try to access an array index that we do not have. */
   if ((size_t)oSymTable->bucketIndex == numBucketCounts - 1) {
      return oSymTable;
   }

   /* gets the new size of the oSymTable */
   newSize = bucketArray[oSymTable->bucketIndex + 1];

   /* intilizes the size of newBucket to fit the
      the new number of buckets and sets all bindings to NULL */
   newBucket = (struct Binding**)
      calloc(newSize, sizeof(struct Binding*));

   /* checks to see if calloc failed */
   if (newBucket == NULL)
      return oSymTable;

   /* iterates through oSymTable until the end is reached. */
   for (i = 0; i < bucketArray[oSymTable->bucketIndex]; i++) {
      for (psCurrentBinding = oSymTable->buckets[i];
           psCurrentBinding != NULL;
           psCurrentBinding = psNextBinding) {

         /* determines the new hash value of each binding */
         hash = SymTable_hash(psCurrentBinding->pcKey, newSize);

         /* maintains access to the next binding */
         psNextBinding = psCurrentBinding->psNextBinding;

         /* adds the current binding to the new correct bucket */
         psCurrentBinding->psNextBinding = newBucket[hash];

         /* puts the current binding at the start of the list
            of the hashed bucket */
         newBucket[hash] = psCurrentBinding;
      }
   }

   /* increments bucketIndex to be the index of the new size*/
   oSymTable->bucketIndex++;
   
   /* frees the old buckets memory */
   free(oSymTable->buckets);

   /* sets oSymTable->buckets to be the new buckets created */
   oSymTable->buckets = newBucket;

   return oSymTable;
}

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;

   /* intilizes the size of oSymTable to be the same size as
      the Table struct */
   oSymTable = (SymTable_T)malloc(sizeof(struct Table));

   /* checks to see if malloc failed */
   if (oSymTable == NULL)
      return NULL;

   /* sets the number of inputs to be 0 intially representing
      an empty SymTable_T */
   oSymTable->tableInputs = 0;

   /* sets the bucketIndex to be 0 representing
      the starting bucket size */
   oSymTable->bucketIndex = 0;

   /* intilizes the size of oSymTable->buckets to fit the
      the intial number of buckets and sets all bindings to NULL */
   oSymTable->buckets = (struct Binding**)
      calloc(bucketArray[0], sizeof(struct Binding*));

   /* checks to see if calloc failed */
   if (oSymTable->buckets == NULL)
      return NULL;

   return oSymTable;
}

void SymTable_free(SymTable_T oSymTable){
   struct Binding *psCurrentBinding;
   struct Binding *psNextBinding;
   /* for loop variable */
   size_t i;

   /* ensures no null input */
   assert(oSymTable != NULL);

   /* iterates through oSymTable freeing all memory */
   for (i = 0; i < bucketArray[oSymTable->bucketIndex]; i++) {
      for (psCurrentBinding = oSymTable->buckets[i];
           psCurrentBinding != NULL;
           psCurrentBinding = psNextBinding) {
         /* maintains access to the next binding */
         psNextBinding = psCurrentBinding->psNextBinding;

         /* frees all memory wihtin the binding */
         free((void *) psCurrentBinding->pcKey);
         free(psCurrentBinding);
      }
   }
   /* frees the memory created for the oSymTable structure
      and the buckets */
   free(oSymTable->buckets);
   free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){
   /* ensures no null input */
   assert(oSymTable != NULL);

   /* returns the number of inputs wihtin oSymTable */
   return oSymTable->tableInputs;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue) {
   struct Binding *psNewBinding;
   char *pcKeySave;
   size_t hash;

   /* ensures no null input where unexpected.
      pvValue can be NULL */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* checks if oSymTable already contains pcKey */
   if(SymTable_contains(oSymTable, pcKey)){
      /* returns 0 representing that pcKey was already found within
         oSymTable so no new binding was added*/
      return 0;
   }

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

   /* increments the number of inputs stored in oSymTable */
   oSymTable->tableInputs++;

   /* checks if oSymtable needs to be resized. This also checks to
      see if we have reached the max number of buckets
      preventing SymTable_resize from being called for every binding
      inserted after the table has already reached its max length */
   if ((oSymTable->tableInputs
        > bucketArray[oSymTable->bucketIndex])
       && ((size_t)oSymTable->bucketIndex != numBucketCounts - 1)){
      /* attempts to resize if enough memory is availiable */
      oSymTable = SymTable_resize(oSymTable);
   }

   /* calculates the hash value to determine which bucket
      the binding will be inserted in */
   hash = SymTable_hash(pcKey,
                        bucketArray[oSymTable->bucketIndex]);

   /* saves the value and defensive key into the binding */
   psNewBinding->pvValue = pvValue;
   psNewBinding->pcKey = pcKeySave;

   /* sets the first binding within the oSymTable bucket
      to be the binding just created */
   psNewBinding->psNextBinding = oSymTable->buckets[hash];
   oSymTable->buckets[hash] = psNewBinding;

   return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){
   struct Binding *psCurrentBinding;
   size_t hash;

   /* ensures no null input where unexpected.
      pvValue can be NULL */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* calculates the hash value to determine which bucket
      to search within */
   hash = SymTable_hash(pcKey, bucketArray[oSymTable->bucketIndex]);

   /* iterates through the oSymTable bucket until the binding
      with pcKey is found or the end is reached */
   for (psCurrentBinding = oSymTable->buckets[hash];
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
   size_t hash;

   /* ensures no null input */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* calculates the hash value to determine which bucket
      to search within */
   hash = SymTable_hash(pcKey, bucketArray[oSymTable->bucketIndex]);

   /* iterates through the oSymTable bucket until the binding
      with pcKey is found or the end is reached */
   for (psCurrentBinding = oSymTable->buckets[hash];
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
   size_t hash;

   /* ensures no null input */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* calculates the hash value to determine which bucket
      to search within */
   hash = SymTable_hash(pcKey, bucketArray[oSymTable->bucketIndex]);

   /* iterates through the oSymTable bucket until the binding
      with pcKey is found or the end is reached */
   for (psCurrentBinding = oSymTable->buckets[hash];
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
   size_t hash;

   /* ensures no null input */
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* calculates the hash value to determine which bucket
      to search within */
   hash = SymTable_hash(pcKey, bucketArray[oSymTable->bucketIndex]);

   psPreviousBinding = NULL;

   /* iterates through the oSymTable bucket until the binding
      with pcKey is found or the end is reached */
   for (psCurrentBinding = oSymTable->buckets[hash];
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
            oSymTable->buckets[hash]
               = psCurrentBinding->psNextBinding;
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
   /* for loop variable */
   size_t i;

   /* ensures no null input where unexpected.
      pvExtra can be NULL */
   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   /* iterates through oSymTable until the end is reached */
   for (i = 0; i <  bucketArray[oSymTable->bucketIndex]; i++) {
      for (psCurrentBinding = oSymTable->buckets[i];
           psCurrentBinding != NULL;
           psCurrentBinding = psCurrentBinding->psNextBinding) {
         /* applies function *pfApply to each binding in
            oSymtable passing pvExtra as an extra parameter*/
         (*pfApply)(psCurrentBinding->pcKey,
                    (void*)psCurrentBinding->pvValue,
                    (void*)pvExtra);
      }
   }
}
