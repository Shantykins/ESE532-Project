#include "hashtable.h"

hash_table* create_table(int table_size) {
    hash_table* table = (hash_table*) malloc(sizeof(hash_table));
    table->size = table_size;
    table->table = (table_entry**) malloc(sizeof(table_entry*) * table_size);
    int i;
    for (i = 0; i < table_size; i++) {
        (table->table)[i] = NULL;
    }
    return table;
}

// This hash function uses "djb2". See the link below.
// http://www.cse.yorku.ca/~oz/hash.html
int hash_func(hash_table* table, char* key) {
    unsigned long hash = 5381;
    int c = *key++;
    while (c) {
        hash = ((hash << 5) + hash) + c;
        c = *key++;
    }
    return hash % table->size;
}

void put(hash_table* table, char* key, uint16_t value) {
    int hash = hash_func(table, key);

    // Create a new node
    table_entry* new_entry = (table_entry*) malloc(sizeof(table_entry));
    new_entry->key = (char*) malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(new_entry->key, key);
    new_entry->value = value;
    new_entry->next = NULL;
    
    table_entry* ptr = (table->table)[hash];
    if (ptr == NULL) {
        (table->table)[hash] = new_entry;
    } else {
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = new_entry;
    }
}

// Returns the value associated with the key, or
// -1 to indicate that the value was not found.
uint16_t get(hash_table* table, char* key) {
    int hash = hash_func(table, key);

    table_entry* ptr = (table->table)[hash];
    if (ptr != NULL) {
        while (ptr != NULL) {
            if (strcmp(ptr->key, key) == 0) {
                return ptr->value;
            }
            ptr = ptr->next;
        }
    }
    return -1;
}

int num_entries(hash_table* table) {
    int n = 0;
    int i;
    for (i = 0; i < table->size; i++) {
        table_entry* ptr = (table->table)[i];
        while (ptr != NULL) {
            n++;
            ptr = ptr->next;
        }
    }
    return n;
}

// For debugging purposes
void print_table(hash_table* table) {
    printf("Number of buckets: %d\n", table->size);
    int i;
    for (i = 0; i < table->size; i++) {
        printf("Bucket #%d:\n", i);
        table_entry* ptr = (table->table)[i];
        while (ptr != NULL) {
            printf("%s => %d\n", ptr->key, ptr->value);
            ptr = ptr->next;
        }
    }
    printf("END OF HASH TABLE\n");
}

void delete_table(hash_table* table) {
    int i;
    for (i = 0; i < table->size; i++) {
        table_entry* ptr = (table->table)[i];
        while (ptr != NULL) {
            table_entry* next = ptr->next;
            free(ptr->key);
            free(ptr);
            ptr = next;
        }
    }
    free(table->table);
    free(table);
}

// int main (int argc, char** argv) {
//     hash_table* hash = create_table(7);
//     put(hash, "Hello", 156);
//     put(hash, "My", 13);
//     put(hash, "Name", 3);
//     put(hash, "Is", 1923);
//     put(hash, "Ryan", 1);
//     print_table(hash);
//     delete_table(hash);
//     return 0;
// }