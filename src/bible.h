// --------------------------------------------------------------- //
// Liam Harms                                                      //
// bible.h                                                         //
// October 2025                                                    //
// --------------------------------------------------------------- //

#include <stdlib.h>
#include <string.h>

typedef struct {
    char* book;
    int chapter;
    int verse_num;
    char* text;
} verse;

typedef struct {
    char* shorthand;
    char* title;
    int chapters;
} book;

typedef struct {
    char* shorthand;
    char* name;
    bool strongs;
    bool apocrypha;
} version;

typedef struct vllist {
    verse* verse;
    struct vllist* next;
} verse_llist;

verse* createVerse(const char* book, const int chapter, const int verse_num, const char* text) {
    verse* v = (verse*)malloc(sizeof(verse));
    if (v == NULL) {
        return NULL;
    }
    
    v->book = strdup(book);
    if (v->book == NULL) {
        free(v);
        return NULL;
    }
    
    v->text = strdup(text);
    if (v->text == NULL) {
        free(v->book);
        free(v);
        return NULL;
    }
    
    v->verse_num = verse_num;
    v->chapter = chapter;

    return v;
}
