/*

Copyright(c) 2007. Victor M. Alvarez [plusvic@gmail.com].

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef _YARA_H 
#define _YARA_H

#include <stdio.h>
#include <pcre.h>


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define STRING_FLAGS_FOUND                      0x01
#define STRING_FLAGS_REFERENCED					0x02
#define STRING_FLAGS_HEXADECIMAL                0x04
#define STRING_FLAGS_NO_CASE                    0x08
#define STRING_FLAGS_ASCII                      0x10
#define STRING_FLAGS_WIDE                       0x20
#define STRING_FLAGS_REGEXP                     0x40
#define STRING_FLAGS_FULL_WORD                  0x80
#define STRING_FLAGS_ANONYMOUS                  0x100

#define IS_HEX(x)       (((x)->flags) & STRING_FLAGS_HEXADECIMAL)
#define IS_NO_CASE(x)   (((x)->flags) & STRING_FLAGS_NO_CASE)
#define IS_ASCII(x)     (((x)->flags) & STRING_FLAGS_ASCII)
#define IS_WIDE(x)      (((x)->flags) & STRING_FLAGS_WIDE)
#define IS_REGEXP(x)    (((x)->flags) & STRING_FLAGS_REGEXP)
#define IS_FULL_WORD(x) (((x)->flags) & STRING_FLAGS_FULL_WORD)
#define IS_ANONYMOUS(x) (((x)->flags) & STRING_FLAGS_ANONYMOUS)

#define RULE_FLAGS_MATCH                        0x01
#define RULE_FLAGS_PRIVATE                      0x02
#define RULE_FLAGS_GLOBAL						0x04
#define RULE_FLAGS_REQUIRE_PE_FILE 	            0x08

#ifndef ERROR_SUCCESS 
#define ERROR_SUCCESS                           0
#endif

#define ERROR_INSUFICIENT_MEMORY                1
#define ERROR_DUPLICATE_RULE_IDENTIFIER         2
#define ERROR_INVALID_CHAR_IN_HEX_STRING        3
#define ERROR_MISMATCHED_BRACKET                4
#define ERROR_SKIP_AT_END                       5
#define ERROR_INVALID_SKIP_VALUE                6
#define ERROR_UNPAIRED_NIBBLE                   7
#define ERROR_CONSECUTIVE_SKIPS                 8
#define ERROR_MISPLACED_WILDCARD_OR_SKIP        9
#define ERROR_UNDEFINED_STRING                  10
#define ERROR_UNDEFINED_RULE                    11
#define ERROR_COULD_NOT_OPEN_FILE               12
#define ERROR_INVALID_REGULAR_EXPRESSION        13
#define ERROR_SYNTAX_ERROR                      14
#define ERROR_DUPLICATE_TAG_IDENTIFIER			15
#define ERROR_UNREFERENCED_STRING				16
#define ERROR_DUPLICATE_STRING_IDENTIFIER		17
#define ERROR_CALLBACK_ERROR            		18
#define ERROR_MISPLACED_OR_OPERATOR             19
#define ERROR_INVALID_OR_OPERATION_SYNTAX       20
#define ERROR_SKIP_INSIDE_OR_OPERATION          21  
#define ERROR_NESTED_OR_OPERATION               22 
#define ERROR_MISPLACED_ANONYMOUS_STRING        23
#define ERROR_COULD_NOT_MAP_FILE                24
#define ERROR_ZERO_LENGTH_FILE                  25
#define ERROR_INVALID_ARGUMENT                  26
      

typedef struct _MATCH
{   
    unsigned int    offset;   
	unsigned int	length;
    struct _MATCH* next;
    
} MATCH;


typedef struct _REGEXP
{
    pcre* regexp;
    pcre_extra* extra;
    
} REGEXP;


typedef struct _STRING
{
    int             flags;
    char*           identifier;
    unsigned int    length;
    unsigned char*  string;
    
    union {
        unsigned char*  mask;
        REGEXP re;
    };  
    
    MATCH*         	matches;        
    struct _STRING* next;
    
} STRING;


typedef struct _TAG
{
	char*			identifier;
	struct _TAG*	next;
	
} TAG;


typedef struct _TERM
{
    int				type;
    struct _TERM*   next;           

} TERM;


typedef struct _RULE
{
    char*           identifier;
    int             flags;
    STRING*         string_list_head;
	TAG*			tag_list_head;
    TERM*           condition;
    struct _RULE*   next;
    
} RULE;


typedef struct _STRING_LIST_ENTRY
{
	STRING* string;
	struct _STRING_LIST_ENTRY* next;
	
} STRING_LIST_ENTRY;


typedef struct _RULE_LIST
{
    RULE* head; 
    RULE* tail;
        
} RULE_LIST;


typedef struct _HASH_TABLE
{
    STRING_LIST_ENTRY*  hashed_strings[256][256];
    STRING_LIST_ENTRY*  non_hashed_strings;
    int                 populated;
        
} HASH_TABLE;


typedef int (*YARACALLBACK)(RULE* rule, unsigned char* buffer, unsigned int buffer_size, void* data);
typedef void (*YARAREPORT)(const char* file_name, int line_number, const char* error_message);


typedef struct _YARA_CONTEXT
{  
    int             last_result;
    YARAREPORT      error_report_function;
    int             errors;
    int             last_error;
    int             last_error_line;
    const char*     file_name;
    
    RULE_LIST       rule_list;
    HASH_TABLE      hash_table;
    STRING*         current_rule_strings;  
    int             inside_for;
    
    char            last_error_extra_info[256];
    
    char 		    lex_string_buf[256];
    char*		    lex_string_buf_ptr;
    unsigned short  lex_string_buf_len;

} YARA_CONTEXT;


RULE*       lookup_rule(RULE_LIST* rules, char* identifier);
STRING*     lookup_string(STRING* string_list_head, char* identifier);
TAG*        lookup_tag(TAG* tag_list_head, char* identifier);

void                yr_init();
YARA_CONTEXT*       yr_create_context();
void                yr_destroy_context(YARA_CONTEXT* context);

int         yr_compile_file(FILE* rules_file, YARA_CONTEXT* context);
int         yr_compile_string(const char* rules_string, YARA_CONTEXT* context);

int         yr_scan_mem(unsigned char* buffer, unsigned int buffer_size, YARA_CONTEXT* context, YARACALLBACK callback, void* user_data);
int         yr_scan_file(const char* file_path, YARA_CONTEXT* context, YARACALLBACK callback, void* user_data);

char*       yr_get_error_message(YARA_CONTEXT* context, char* buffer, int buffer_size);

#endif
