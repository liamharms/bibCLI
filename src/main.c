// --------------------------------------------------------------- //
// Lord, I give this work to you. Guide it to work for your glory, //
// not for mine or any other man. By it, Lord, work your Kingdom   //
// and bless it, that its fruits may be derving of you. Amen.      //
// --------------------------------------------------------------- //
//                           B I B C L I                           //
//                             v 0.0.5                             //
// --------------------------------------------------------------- //
// Liam Harms                                                      //
// main.c                                                          //
// October 2025                                                    //
// --------------------------------------------------------------- //

#define DEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bibdb.h"

void print_help(const char *prog_name) {
    printf("Default usage: %s [OPTIONS] <book> <chapter> <verse> [verse_end]\n", prog_name);
    printf("Options:\n");
    printf("  -h,  --help                          Display this help message\n");
    printf("  -v,  --version <version>             Specify Bible version (default: KJV)\n");
    printf("  -n,  --number                        Show verse numbers\n");
    printf("  -N,  --show-strongs                  Show Strong's numbers (Version must support this)\n");
    printf("  -s,  --search  <keyword>             Search for keyword in verses\n");
    printf("  -V,  --versions [version]            List available versions or details about provided version\n");
    printf("  -S,  --strongs <strongs_number>      Get information about Strong's number\n");
    printf("Search Options:\n");
    printf("  -S, --strongs  <strongs_number>      Search verses for Strong's number\n");
    printf("  -O, --oldtestament                   Search in Old Testament\n");
    printf("  -A, --apocrypha                      Search only in Apocrypha (Version must support this)\n");
    printf("  -N, --newtestament                   Search only in New Testament\n");
    printf("  -r, --range <book> [<chapter_start> <verse_start> <chapter_end> <verse_end>]\n");
    printf("                                       search within specified range\n");
    printf("Usage:\n");
    printf("  %s Jhn 3 16                 Get John 3:16 in KJV\n", prog_name);
    printf("  %s -v ASV Ps 23 1 6         Get Psalm 23:1-6 in ASV\n", prog_name);
    printf("  %s -s baptism john          Search for 'baptism' and 'john'\n", prog_name);
}

void print_version_info(version* v) {
    if (v == NULL) {
        printf("No version information available.\n");
        return;
    }
    printf("Version: %s\n", v->shorthand);
    printf("Name: %s\n", v->name);
    printf("Strong's Numbers Supported: %s\n", v->strongs ? "Yes" : "No");
    printf("Includes Apocrypha: %s\n", v->apocrypha ? "Yes" : "No");
}

void print_strongs_info(strongsnum* s) {
    if (s == NULL) {
        printf("No Strong's information available.\n");
        return;
    }
    printf("ID: %c%d\n", s->lang, s->num);
    printf("Word: %s\n", s->word);
    if (s->transliteration != NULL) {
        printf("Transliteration: %s\n", s->transliteration);
    }
    printf("Definition: %s\n", s->definition);
    if (s->derivation != NULL) {
        printf("Derivation: %s\n", s->derivation);
    }
}

void print_verses(verse_llist* vl, int show_nums) {
    verse_llist* current = vl;
        while (current != NULL) {
            if (show_nums == 1) {
                printf("(%d) ", current->verse->verse_num);
            }
            if (show_nums == 2) {
                printf("(%s %d:%d) ", current->verse->book, current->verse->chapter, current->verse->verse_num);
            }
            printf("%s\n", current->verse->text);
            verse_llist* next = current->next;
            free(current->verse->book);
            free(current->verse->text);
            free(current->verse);
            free(current);
            current = next;
        }
}

int main(int argc, char *argv[]) {
    char* vrsn = "KJV"; // version. Had to shorten the name 'cause it already refers to a struct and I'm too lazy to change it.
    char** pos_argv = NULL;
    int pos_argc = 0;
    bool show_nums = false;

    if (argc==1) {
        printf("No arguments provided.\n");
        printf("See %s -h for help.\n", argv[0]);
        return 0;
    }

    // Handle options
    for (int i=1; i<argc; i++) {
        if(argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'h': {
                    print_help(argv[0]);
                    return 0;
                }
                case 'v': {
                    if (i + 1 < argc) {
                        vrsn = argv[++i];
                    } else {
                        fprintf(stderr, "Error: -v requires a version argument\n");
                        return 1;
                    }
                    break;
                }
                case 'n': {
                    show_nums = true;
                    break;
                }
                case 'N': {
                    // TODO:
                    fprintf(stderr, "Option not implemented (sorry)\n");
                    return 1;
                }
                case 's': {
                    // The search function of chaos
                    char** terms = NULL;
                    int num_terms = 0;

                    if (i + 1 >= argc || argv[i+1][0] == '-') {
                        fprintf(stderr, "Error: -s requires at least one search argument\n");
                        return 1;
                    }

                    // Right here we're collecting all the arguments before the next flag
                    while (++i < argc && argv[i][0] != '-') {
                        debug_print("processing search term\n", NULL);
                        char** temp = realloc(terms, (num_terms + 1) * sizeof(char*));
                        if (temp == NULL) {
                            fprintf(stderr, "Memory allocation failed\n");
                            free(terms);
                            return 1;
                        }
                        terms = temp;
                        terms[num_terms] = argv[i];
                        num_terms++;
                    }

                    verse_llist* verses = searchByText(vrsn, terms);
                    print_verses(verses, 2);
                    free(terms);

                    return 0;
                }
                case 'V': {
                    if (i + 1 < argc) {
                        version* v = versionInfo(argv[++i]);
                        print_version_info(v);
                        if (v) {
                            free(v->shorthand);
                            free(v->name);
                            free(v);
                        }
                    } else {
                        fprintf(stderr, "Error: -V requires a version argument\n");
                        return 1;
                    }
                    return 0;
                }
                case 'S': {
                    if (i + 1 < argc) {
                        strongsnum* s = queryStrongs(argv[++i]);
                        print_strongs_info(s);
                    } else {
                        fprintf(stderr, "Error: -S requires an argument\n");
                        return 1;
                    }
                    return 0;
                }
                default: {
                    fprintf(stderr, "Unknown option: %s\n", argv[i]);
                    print_help(argv[0]);
                    return 1;
                }
            }
        }
        else {
            // Collect positional arguments
            pos_argv = realloc(pos_argv, (pos_argc + 1) * sizeof(char*));
            if (pos_argv == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }
            pos_argv[pos_argc++] = argv[i];
        }
    }

    if (pos_argc < 3) {
        fprintf(stderr, "Not enough positional arguments provided\n");
        printf("See %s -h for help.\n", argv[0]);
        free(pos_argv);
        return 1;
    }

    // Validate version
    version* versioncheck = versionInfo(vrsn);
    if (versioncheck == NULL) {
        fprintf(stderr, "Invalid version specified: %s\n", vrsn);
        free(pos_argv);
        return 1;
    }
    vrsn = versioncheck->shorthand;
    free(versioncheck->name);
    free(versioncheck);

    // Validate book

    // Verse queries
    if (pos_argc == 3) {
        verse* v = queryVerse(vrsn, pos_argv[0], atoi(pos_argv[1]), atoi(pos_argv[2]));
        if (v != NULL) {
            if (show_nums) {
                printf("(%d) ", v->verse_num);
            }
            printf("%s\n", v->text);
            free(v->book);
            free(v->text);
            free(v);
        }
    }
    else if (pos_argc == 4) {
        verse_llist* verses = queryVerses(vrsn, pos_argv[0], atoi(pos_argv[1]), atoi(pos_argv[2]), atoi(pos_argv[3]));
        print_verses(verses, show_nums);
    }
    
    free(pos_argv);
    return 0;
}
