#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct table_entry {
    char* key;
    uint16_t value;
    struct table_entry* next;
} table_entry;

typedef struct hash_table {
    int size;
    struct table_entry** table;
} hash_table;

int hash_func(hash_table* table, char* key);
hash_table* create_table(int table_size);
void put(hash_table* table, char* key, uint16_t value);
uint16_t get(hash_table* table, char* key);
int num_entries(hash_table* table);
void print_table(hash_table* table);
void delete_table(hash_table* table);

#endif