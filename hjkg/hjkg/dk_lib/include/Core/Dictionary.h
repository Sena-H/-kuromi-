
/*-------------------------------------------------------------------------*/
/**
   @file    dictionary.h
   @author  N. Devillard
   @brief   Implements a dictionary for string variables.

   This module implements a simple dictionary object, i.e. a list
   of string/string associations. This object is useful to store e.g.
   informations retrieved from a configuration file (ini files).
*/
/*--------------------------------------------------------------------------*/

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/

#include"GlobalDef.h"
#include<stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
                                New types
 ---------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/**
  @brief    Dictionary object

  This object contains a list of string/string associations. Each
  association is identified by a unique string key. Looking up values
  in the dictionary is speeded up by the use of a (hopefully collision-free)
  hash function.
 */
/*-------------------------------------------------------------------------*/
typedef struct _dict_ {
    int             size ;     /** Number of entries in dictionary */
    size_t          capacity ;  /** Storage size */
    general     *   vals ;   /** List of general values */
    char        **  keys ;   /** List of string keys */
    unsigned     *  hash ;  /** List of hash values for keys */
} dictionary ;

typedef struct _Pair_
{
    char* key;
    general val;
}Pair;

/*---------------------------------------------------------------------------
                            Function prototypes
 ---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/**
  @brief    Compute the hash key for a string.
  @param    key     Character string to use for key.
  @return   1 unsigned int on at least 32 bits.

  This hash function has been taken from an Article in Dr Dobbs Journal.
  This is normally a collision-free function, distributing keys evenly.
  The key is stored anyway in the struct so that collision can be avoided
  by comparing the key itself in last resort.
 */
/*--------------------------------------------------------------------------*/
unsigned dict_hash(const char * key);

/*-------------------------------------------------------------------------*/
/**
  @brief    Create a new dictionary object.
  @param    size    Optional initial size of the dictionary.
  @return   1 newly allocated dictionary object.

  This function allocates a new dictionary object of given size and returns
  it. If you do not know in advance (roughly) the number of entries in the
  dictionary, give size=0.
 */
/*--------------------------------------------------------------------------*/
dictionary * dict_new(size_t size);

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete a dictionary object
  @param    d   dictionary object to deallocate.
  @return   void

  Deallocate a dictionary object and all memory associated to it.
 */
/*--------------------------------------------------------------------------*/
void dict_del(dictionary * vd);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get a value from a dictionary.
  @param    d       dictionary object to search.
  @param    key     Key to look for in the dictionary.
  @param    def     Default value to return if key not found.
  @return   1 pointer to internally allocated character string.

  This function locates a key in a dictionary and returns a pointer to its
  value, or the passed 'def' pointer if no such key can be found in
  dictionary. The returned character pointer points to data internal to the
  dictionary object, you should not try to free it or modify it.
 */
/*--------------------------------------------------------------------------*/
general dict_get(const dictionary * d, const char * key, general def);


/*-------------------------------------------------------------------------*/
/**
  @brief    Set a value in a dictionary.
  @param    d       dictionary object to modify.
  @param    key     Key to modify or add.
  @param    val     Value to add.
  @return   int     0 if Ok, anything else otherwise

  If the given key is found in the dictionary, the associated value is
  replaced by the provided one. If the key cannot be found in the
  dictionary, it is added to it.

  It is Ok to provide a NULL value for val, but NULL values for the dictionary
  or the key are considered as errors: the function will return immediately
  in such a case.

  Notice that if you dict_set a variable to NULL, a call to
  dict_get will return a NULL value: the variable will be found, and
  its value (NULL) is returned. In other words, setting the variable
  content to NULL is equivalent to deleting the variable from the
  dictionary. It is not possible (in this implementation) to have a key in
  the dictionary without value.

  This function returns non-zero in case of failure.
 */
/*--------------------------------------------------------------------------*/
int dict_set(dictionary * vd, const char * key, general val);

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete a key in a dictionary
  @param    d       dictionary object to modify.
  @param    key     Key to remove.
  @return   void

  This function deletes a key in a dictionary. Nothing is done if the
  key cannot be found.
 */
/*--------------------------------------------------------------------------*/
void dict_unset(dictionary * d, const char * key);


/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump
  @param    f   Opened file pointer.
  @return   void

  Dumps a dictionary onto an opened file pointer. Key pairs are printed out
  as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
  output file pointers.
 */
/*--------------------------------------------------------------------------*/
void dict_dump(const dictionary * d, FILE * out);

//#define Dict_foreach(key,value,dict)\
//for (int _i = 0,_cnt = 0; _i < (dict)->size; _i++,_cnt = 0)\
//	for(general key = (dict)->keys[_i],value = (dict)->vals[_i];_cnt < 1;_cnt++)

#define Dict_foreach(type,name,dict)\
for (int _i = 0,_cnt = 0; _i < (dict)->capacity; _i++,_cnt = 0)\
	for(type name = {(dict)->keys[_i],(dict)->vals[_i]};_cnt < 1 && (dict)->keys[_i] !=NULL;_cnt++)

#ifdef __cplusplus
}
#endif

#endif
