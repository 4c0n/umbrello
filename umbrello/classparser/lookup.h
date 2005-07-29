// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

// adapted to kdevelop by Roberto Raggi <roberto@kdevelop.org>

#ifndef _KJSLOOKUP_H_
#define _KJSLOOKUP_H_

#include <qstring.h>
#include <stdio.h>

  /**
   * An entry in a hash table.
   */
  struct HashEntry {
    /**
     * s is the key (e.g. a property name)
     */
    const char *s;
    /**
     * value is the result value (usually an enum value)
     */
    int value;
    /**
     * attr is a set for flags (e.g. the property flags, see object.h)
     */
    short int attr;
    /**
     * params is another number. For property hashtables, it is used to
     * denote the number of argument of the function
     */
    short int params;
    /**
     * next is the pointer to the next entry for the same hash value
     */
    const HashEntry *next;
  };

  /**
   * A hash table
   * Usually the hashtable is generated by the create_hash_table script, from a .table file.
   *
   * The implementation uses an array of entries, "size" is the total size of that array.
   * The entries between 0 and hashSize-1 are the entry points
   * for each hash value, and the entries between hashSize and size-1
   * are the overflow entries for the hash values that need one.
   * The "next" pointer of the entry links entry points to overflow entries,
   * and links overflow entries between them.
   */
  struct HashTable {
    /**
     * type is a version number. Currently always 2
     */
    int type;
    /**
     * size is the total number of entries in the hashtable, including the null entries,
     * i.e. the size of the "entries" array.
     * Used to iterate over all entries in the table
     */
    int size;
    /**
     * pointer to the array of entries
     * Mind that some entries in the array are null (0,0,0,0).
     */
    const HashEntry *entries;
    /**
     * the maximum value for the hash. Always smaller than size.
     */
    int hashSize;
  };

  /**
   * @short Fast keyword lookup.
   */
  class Lookup {
  public:
    /**
     * Find an entry in the table, and return its value (i.e. the value field of HashEntry)
     */
    static int find(const struct HashTable *table, const QString& s);
    static int find(const struct HashTable *table, const QChar *c, unsigned int len);

    /**
     * Find an entry in the table, and return the entry
     * This variant gives access to the other attributes of the entry,
     * especially the attr field.
     */
    static const HashEntry* findEntry(const struct HashTable *table,
                                      const QString &s);
    static const HashEntry* findEntry(const struct HashTable *table,
                                      const QChar *c, unsigned int len);

    /**
     * Calculate the hash value for a given key
     */
    static unsigned int hash(const QString &key);
    static unsigned int hash(const QChar *c, unsigned int len);
    static unsigned int hash(const char *s);
  };

#endif
