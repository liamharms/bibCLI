// --------------------------------------------------------------- //
// Liam Harms                                                      //
// bibdb.h                                                         //
// October 2025                                                    //
// --------------------------------------------------------------- //

#include <sqlite3.h>
#include <stdio.h>
#include <stdbool.h>
#include "bible.h"
#include "debug.h"

static int callback_one_verse(void* passin, int count, char **data, char **columns) {
    // Sets the passed in variable (must be a verse*) to the verse text
    
    // Data is of form
    // +----+---------+------+---------+-------+------+
    // | id | version | book | chapter | verse | text |
    // +----+---------+------+---------+-------+------+
    debug_print("callback_one_verse called\n", NULL);


    verse** v = (verse**)passin;
    verse* new_verse = createVerse(data[2], atoi(data[3]), atoi(data[4]), data[5]);
    if (new_verse == NULL) {
        return 1;  // Error creating verse
    }
    *v = new_verse;
    return 0;
}

static int callback_verses(void* passin, int count, char **data, char **columns) {
    // Sets the passed in variable (must be a verse_llist*) to the head node of an llist with the verses
    
    // Data is of form
    // +----+---------+------+---------+-------+------+
    // | id | version | book | chapter | verse | text |
    // +----+---------+------+---------+-------+------+
    debug_print("callback_verses called\n", NULL);

    verse_llist** head = (verse_llist**)passin;
    verse_llist* current = NULL;
    verse_llist* new_node = NULL;

    verse* v = createVerse(data[2], atoi(data[3]), atoi(data[4]), data[5]);
    if (v == NULL) {
        return 1;
    }
    
    // Create new node
    new_node = (verse_llist*)malloc(sizeof(verse_llist));
    if (new_node == NULL) {
        free(v->book);
        free(v->text);
        free(v);
        return 1;
    }
    new_node->verse = v;
    new_node->next = NULL;

    // If this is the first node
    if (*head == NULL) {
        *head = new_node;
        current = *head;
    } else {
        // Find the end of the list
        current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        // Add to the end of the list
        current->next = new_node;
    }

    return 0;
}

static int callback_versionInfo(void* passin, int count, char **data, char **columns) {
    // Print version info
    // Data is of form
    // +-----------+------+---------+-----------+
    // | shorthand | name | strongs | apocrypha |
    // +-----------+------+---------+-----------+
    debug_print("callback_versionInfo called\n", NULL);

    if (count < 4 || data == NULL) {
        return 1;
    }

    version** v = (version**)passin;
    if (v == NULL) {
        return 1;
    }

    version* new_version = (version*)malloc(sizeof(version));
    if (new_version == NULL) {
        return 1;
    }

    new_version->shorthand = strdup(data[0]);
    if (new_version->shorthand == NULL) {
        free(new_version);
        return 1;
    }

    new_version->name = strdup(data[1]);
    if (new_version->name == NULL) {
        free(new_version->shorthand);
        free(new_version);
        return 1;
    }

    if (data[2] == NULL || data[3] == NULL) {
        free(new_version->shorthand);
        free(new_version->name);
        free(new_version);
        return 1;
    }

    new_version->strongs = atoi(data[2]);
    new_version->apocrypha = atoi(data[3]);

    *v = new_version;
    return 0;
}

static int callback_one_strongs(void* passin, int count, char **data, char **columns) {
    // Sets the passed in variable (must be a strongsnum*) to the strongs number
    
    // Data is of form
    // +----+------+-----------------+------------+------------+
    // | id | word | transliteration | definition | derivation |
    // +----+------+-----------------+------------+------------+

    strongsnum** s = (strongsnum**)passin;
    strongsnum* newstrongs = (strongsnum*)malloc(sizeof(strongsnum));

    newstrongs->lang = data[0][0];
    newstrongs->num = atoi( memmove(data[0], data[0]+1, strlen(data[0])) );
    newstrongs->word = strdup(data[1]);
    if (data[2] != NULL) {
        newstrongs->transliteration = strdup(data[2]);
    } else {
        newstrongs->transliteration = NULL;
    }
    newstrongs->definition = strdup(data[3]);
    newstrongs->derivation = strdup(data[4]);

    *s = newstrongs;

    return 0;
}

sqlite3* db;
char* db_location = "src/db/bible.db";

verse* queryVerse(const char* vrsn, const char* book, const int chapter, const int verse_num) {
    // Returns verse from SQL query
    // -----------------------------------
    // Arguments must exactly match SQL
    debug_print("function call: queryVerse(version=%s, book=%s, chapter=%d, verse_num=%d)\n", vrsn, book, chapter, verse_num);

    verse* v = NULL;
    char* err_msg = NULL;
    char sql[256];
    
    // Open DB
    int rc = sqlite3_open(db_location, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }
    
    // Create SQL statement
    snprintf(sql, sizeof(sql), "SELECT * FROM bible_text WHERE version='%s' AND book='%s' AND chapter=%d AND verse=%d LIMIT 1;", 
             vrsn, book, chapter, verse_num);
    
    // Execute SQL statement with text as the data variable
    rc = sqlite3_exec(db, sql, callback_one_verse, &v, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return NULL;
    }
    
    sqlite3_close(db);
    
    if (v == NULL) {
        fprintf(stderr, "No verse found\n");
        return NULL;
    }
    
    return v;
}

verse_llist* queryVerses(const char* vrsn, const char* book, const int chapter, const int verse_num1, const int verse_num2) {
    // Returns verse llist from SQL query
    // -----------------------------------
    // Arguments must exactly match SQL
    debug_print("function call: queryVerses(version=%s, book=%s, chapter=%d, verse_num1=%d, verse_num2=%d)\n", vrsn, book, chapter, verse_num1, verse_num2);

    verse_llist* head = NULL;
    char* err_msg = NULL;
    char sql[256];
    
    // Open DB
    int rc = sqlite3_open(db_location, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // Create SQL statement
    snprintf(sql, sizeof(sql), "SELECT * FROM bible_text WHERE version='%s' AND book='%s' AND chapter=%d AND verse BETWEEN %d AND %d;", 
             vrsn, book, chapter, verse_num1, verse_num2);

    rc = sqlite3_exec(db, sql, callback_verses, &head, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_close(db);
    return head;
}

version* versionInfo(const char* v_shorthand) {
    // Returns validated version shorthand or NULL if not found
    debug_print("function call: versionInfo(v_shorthand=%s)\n", v_shorthand);

    if (v_shorthand == NULL) {
        fprintf(stderr, "Version shorthand cannot be NULL\n");
        return NULL;
    }

    char* err_msg = NULL;
    char sql[256];
    version* v = NULL;

    // Open DB
    int rc = sqlite3_open(db_location, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        if (db != NULL) {
            sqlite3_close(db);
        }
        return NULL;
    }

    // Create SQL statement
    snprintf(sql, sizeof(sql), "SELECT shorthand, name, strongs, apocrypha FROM versions WHERE UPPER(shorthand) = UPPER('%s') LIMIT 1;", v_shorthand);

    rc = sqlite3_exec(db, sql, callback_versionInfo, &v, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        if (err_msg != NULL) {
            sqlite3_free(err_msg);
        }
        sqlite3_close(db);
        return NULL;
    }
    debug_print("Version info retrieved: %s\n", v != NULL ? v->shorthand : "NULL");

    sqlite3_close(db);
    
    if (v == NULL) {
        fprintf(stderr, "No version found for '%s'\n", v_shorthand);
        return NULL;
    }
    
    return v;
}

verse_llist* searchByText(const char* vrsn, char** search_terms) {
    // Return verse llist from text search
    debug_print("function call: searchByText\n", vrsn);

    verse_llist* head = NULL;
    char* err_msg = NULL;
    char sql[512];

    // Open DB
    int rc = sqlite3_open(db_location, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // Create SQL statement
    int len = snprintf(sql, sizeof(sql), "SELECT * FROM bible_text WHERE version='%s'", 
             vrsn);
    
    int i = 0;
    if (search_terms == NULL) {
        fprintf(stderr, "No search terms provided\n");
        return NULL;
    }
    
    for (; search_terms[i] != NULL; i++) {
        len += snprintf(sql + len, sizeof(sql) - len, " AND UPPER(text) LIKE UPPER('%%%s%%')",
                search_terms[i]);
        if (len >= sizeof(sql)) {
            fprintf(stderr, "SQL query too long\n");
            return NULL;
        }
    }

    rc = sqlite3_exec(db, sql, callback_verses, &head, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_close(db);
    return head;
}

strongsnum* queryStrongs(const char* strongs) {

    debug_print("function call: queryStrongs(strongs=%s)\n",strongs);

    strongsnum* s = NULL;
    char* err_msg = NULL;
    char sql[256];

    // Open DB
    int rc = sqlite3_open(db_location, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // Create SQL statement
    snprintf(sql, sizeof(sql), "SELECT * FROM strongs WHERE UPPER(id) = UPPER('%s') LIMIT 1;", 
             strongs);
    
    // Execute SQL statement with text as the data variable
    rc = sqlite3_exec(db, sql, callback_one_strongs, &s, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return NULL;
    }
    
    sqlite3_close(db);
    
    if (s == NULL) {
        fprintf(stderr, "No strongs number found\n");
        return NULL;
    }
    
    return s;
}
