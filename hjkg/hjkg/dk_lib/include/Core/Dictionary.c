/*-------------------------------------------------------------------------*/
/**
   @file    dictionary.c
   @author  N. Devillard
   @brief   Implements a dictionary for string variables.

   This module implements a simple dictionary object, i.e. a list
   of string/string associations. This object is useful to store e.g.
   informations retrieved from a configuration file (ini files).
*/
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/
#include "dictionary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Maximum value capacity for integers and doubles. */
#define MAXVALSZ    1024

/** Minimal allocated number of entries in a dictionary */
#define DICTMINSZ   128

/** Invalid keys token */
#define DICT_INVALID_KEY    ((char*)-1)

/*---------------------------------------------------------------------------
                            Private functions
 ---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/**
  @brief    Duplicate a string
  @param    s String to duplicate
  @return   Pointer to a newly allocated string, to be freed with free()

  This is a replacement for strdup(). This implementation is provided
  for systems that do not have it.
 */
/*--------------------------------------------------------------------------*/
static char * xstrdup(const char * s)
{
    char * t ;
    size_t len ;
    if (!s)
        return NULL ;

    len = strlen(s) + 1 ;
    t = (char*) malloc(len) ;
    if (t) {
        memcpy(t, s, len) ;
    }
    return t ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Double the capacity of the dictionary
  @param    d Dictionary to grow
  @return   This function returns non-zero in case of failure
 */
/*--------------------------------------------------------------------------*/
static int dict_grow(dictionary * d)
{
    char        ** new_val ;
    char        ** new_key ;
    unsigned     * new_hash ;

    new_val  = (char**) calloc(d->capacity * 2, sizeof *d->vals);
    new_key  = (char**) calloc(d->capacity * 2, sizeof *d->keys);
    new_hash = (unsigned*) calloc(d->capacity * 2, sizeof *d->hash);
    if (!new_val || !new_key || !new_hash) {
        /* An allocation failed, leave the dictionary unchanged */
        if (new_val)
            free(new_val);
        if (new_key)
            free(new_key);
        if (new_hash)
            free(new_hash);
        return -1 ;
    }
    /* Initialize the newly allocated space */
    memcpy(new_val, d->vals, d->capacity * sizeof(char *));
    memcpy(new_key, d->keys, d->capacity * sizeof(char *));
    memcpy(new_hash, d->hash, d->capacity * sizeof(unsigned));
    /* Delete previous data */
    free(d->vals);
    free(d->keys);
    free(d->hash);
    /* Actually update the dictionary */
    d->capacity *= 2 ;
    d->vals = new_val;
    d->keys = new_key;
    d->hash = new_hash;
    return 0 ;
}

/*---------------------------------------------------------------------------
                            Function codes
 ---------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief    Compute the hash keys for a string.
  @param    keys     Character string to use for keys.
  @return   1 unsigned int on at least 32 bits.

  This hash function has been taken from an Article in Dr Dobbs Journal.
  This is normally a collision-free function, distributing keys evenly.
  The keys is stored anyway in the struct so that collision can be avoided
  by comparing the keys itself in last resort.
 */
/*--------------------------------------------------------------------------*/
unsigned dict_hash(const char * keys)
{
    size_t      len ;
    unsigned    hash ;
    size_t      i ;

    if (!keys)
        return 0 ;

    len = strlen(keys);
    for (hash=0, i=0 ; i<len ; i++) {
        hash += (unsigned)keys[i] ;
        hash += (hash<<10);
        hash ^= (hash>>6) ;
    }
    hash += (hash <<3);
    hash ^= (hash >>11);
    hash += (hash <<15);
    return hash ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Create a new dictionary object.
  @param    capacity    Optional initial capacity of the dictionary.
  @return   1 newly allocated dictionary object.

  This function allocates a new dictionary object of given capacity and returns
  it. If you do not know in advance (roughly) the number of entries in the
  dictionary, give capacity=0.
 */
/*-------------------------------------------------------------------------*/
dictionary * dict_new(size_t capacity)
{
    dictionary  *   d ;

    /* If no capacity was specified, allocate space for DICTMINSZ */
    if (capacity<DICTMINSZ) capacity=DICTMINSZ ;

    d = (dictionary*) calloc(1, sizeof *d) ;

    if (d) {
        d->capacity = capacity ;
        d->vals  = (char**) calloc(capacity, sizeof *d->vals);
        d->keys  = (char**) calloc(capacity, sizeof *d->keys);
        d->hash = (unsigned*) calloc(capacity, sizeof *d->hash);
    }
    return d ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete a dictionary object
  @param    d   dictionary object to deallocate.
  @return   void

  Deallocate a dictionary object and all memory associated to it.
 */
/*--------------------------------------------------------------------------*/
void dict_del(dictionary * d)
{
    size_t  i ;

    if (d==NULL) return ;
    for (i=0 ; i<d->capacity ; i++) {
        if (d->keys[i]!=NULL)
            free(d->keys[i]);
        if (d->vals[i]!=NULL)
            free(d->vals[i]);
    }
    free(d->vals);
    free(d->keys);
    free(d->hash);
    free(d);
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Get a value from a dictionary.
  @param    d       dictionary object to search.
  @param    keys     Key to look for in the dictionary.
  @param    def     Default value to return if keys not found.
  @return   1 pointer to internally allocated character string.

  This function locates a keys in a dictionary and returns a pointer to its
  value, or the passed 'def' pointer if no such keys can be found in
  dictionary. The returned character pointer points to data internal to the
  dictionary object, you should not try to free it or modify it.
 */
/*--------------------------------------------------------------------------*/
general dict_get(const dictionary * d, const char * keys, general def)
{
    unsigned    hash ;
    size_t      i ;

    hash = dict_hash(keys);
    for (i=0 ; i<d->capacity ; i++) {
        if (d->keys[i]==NULL)
            continue ;
        /* Compare hash */
        if (hash==d->hash[i]) {
            /* Compare string, to avoid hash collisions */
            if (!strcmp(keys, d->keys[i])) {
                return d->vals[i] ;
            }
        }
    }
    return def ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Set a value in a dictionary.
  @param    d       dictionary object to modify.
  @param    keys     Key to modify or add.
  @param    vals     Value to add.
  @return   int     0 if Ok, anything else otherwise

  If the given keys is found in the dictionary, the associated value is
  replaced by the provided one. If the keys cannot be found in the
  dictionary, it is added to it.

  It is Ok to provide a NULL value for vals, but NULL values for the dictionary
  or the keys are considered as errors: the function will return immediately
  in such a case.

  Notice that if you dict_set a variable to NULL, a call to
  dict_get will return a NULL value: the variable will be found, and
  its value (NULL) is returned. In other words, setting the variable
  content to NULL is equivalent to deleting the variable from the
  dictionary. It is not possible (in this implementation) to have a keys in
  the dictionary without value.

  This function returns non-zero in case of failure.
 */
/*--------------------------------------------------------------------------*/
int dict_set(dictionary * d, const char * keys, general val)
{
    size_t         i ;
    unsigned       hash ;

    if (d==NULL || keys==NULL) return -1 ;

    /* Compute hash for this keys */
    hash = dict_hash(keys) ;
    /* Find if value is already in dictionary */
    if (d->size>0) {
        for (i=0 ; i<d->capacity ; i++) {
            if (d->keys[i]==NULL)
                continue ;
            if (hash==d->hash[i]) { /* Same hash value */
                if (!strcmp(keys, d->keys[i])) {   /* Same keys */
                    /* Found a value: modify and return */
                    if (d->vals[i]!=NULL)
                        free(d->vals[i]);
                    //d->vals[i] = (vals ? xstrdup(vals) : NULL);
                    d->vals[i] = val;
                    /* Value has been modified: return */
                    return 0 ;
                }
            }
        }
    }
    /* Add a new value */
    /* See if dictionary needs to grow */
    if (d->size==d->capacity) {
        /* Reached maximum capacity: reallocate dictionary */
        if (dict_grow(d) != 0)
            return -1;
    }

    /* Insert keys in the first empty slot. Start at d->size and wrap at
       d->capacity. Because d->size < d->capacity this will necessarily
       terminate. */
    for (i=d->size ; d->keys[i] ; ) {
        if(++i == d->capacity) i = 0;
    }
    /* Copy keys */
    d->keys[i]  = xstrdup(keys);
    //d->vals[i]  = (vals ? xstrdup(vals) : NULL) ;
    d->vals[i] = val;
    d->hash[i] = hash;
    d->size ++ ;
    return 0 ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete a keys in a dictionary
  @param    d       dictionary object to modify.
  @param    keys     Key to remove.
  @return   void

  This function deletes a keys in a dictionary. Nothing is done if the
  keys cannot be found.
 */
/*--------------------------------------------------------------------------*/
void dict_unset(dictionary * d, const char * keys)
{
    unsigned    hash ;
    size_t      i ;

    if (keys == NULL || d == NULL) {
        return;
    }

    hash = dict_hash(keys);
    for (i=0 ; i<d->capacity ; i++) {
        if (d->keys[i]==NULL)
            continue ;
        /* Compare hash */
        if (hash==d->hash[i]) {
            /* Compare string, to avoid hash collisions */
            if (!strcmp(keys, d->keys[i])) {
                /* Found keys */
                break ;
            }
        }
    }
    if (i>=d->capacity)
        /* Key not found */
        return ;

    free(d->keys[i]);
    d->keys[i] = NULL ;
    if (d->vals[i]!=NULL) {
        free(d->vals[i]);
        d->vals[i] = NULL ;
    }
    d->hash[i] = 0 ;
    d->size -- ;
    return ;
}

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
void dict_dump(const dictionary * d, FILE * out)
{
    size_t  i ;

    if (d==NULL || out==NULL) return ;
    if (d->size<1) {
        fprintf(out, "empty dictionary size");
        return ;
    }
    for (i=0 ; i<d->capacity ; i++) {
        if (d->keys[i]) {
            fprintf(out, "%20s\t[%#llx]\tsize",
                    d->keys[i],
                    (uint64_t)d->vals[i]);
        }
    }
    return ;
}
