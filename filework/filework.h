#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef NODEBUG
    #define ASSERT(condition)                                   \
    if (!(condition)){                                           \
        fprintf(stderr, "Error in %s:\n"                          \
                        "FILE: %s\n"                               \
                        "LINE: %d\n"                                \
                        "FUNCTION: %s\n",                            \
               #condition, __FILE__, __LINE__, __PRETTY_FUNCTION__);  \
        abort();}
#else
    #define ASSERT(condition) ;
#endif

/// A data structure that contains pointer on a begin of a line in a characters array, line's length, line's index
struct fileLines
{
    char* lineStart = NULL;///< a pointer on the begin of line
    size_t lineLen = 0;///< length of line
    size_t lineIndex = 0;///< index of the line in the source text
};

///A data structure that contain pointer on a text, count of characters, pointer of fileLines array, count of lines
struct TEXT
{
    char * buf = NULL;///<text
    size_t nChar = 0;///<count of characters
    fileLines * lines= NULL;///<fileLines array
    size_t nLines = 0;///<count of lines
};

/**
 * @brief A function for updating poem
 * @param [in] text - pointer on the struct TEXT
 * @param [in] source - pointer on file with source text
 * @return Return nothing
 */
void textCtor(TEXT * text, FILE* const source);

/**
 * @brief A function that compares strings in alphabetical order from begin the of the line to the end
 * @param [in] str1 - the first string
 * @param [in] str2 - the second string
 * @return 0              - if strings are the same
 *         positive value - if str1 is after str2 in alphabetical order
 *         negative value - if str1 is before str2 in alphabetical order
 */
int leftLineCmp(const void* str1, const void* str2);

/**
 * @brief A function that skips non-letter characters in a string from begin the of the line to the end
 * @param [in] str - a pointer on the string
 * @return Pointer to a letter character in the source string
 */
const char* leftEatPunct(const char* str);

/**
 * @brief A function that count how many characters are in the file
 * @param [in] file - a pointer on the file
 * @return Count of characters
 */
size_t fileSize(FILE* const file);

/**
 * @brief A universal mergesort function
 * @param [in] data - array that we need to sort
 * @param [in] supporting - supporting array 
 * @param [in] left - index of begin of the array
 * @param [in] right - index of end of the array
 * @param [in] size - size of every element of data
 * @param [in] comparator - pointer on the function, that compare two strings in selected order
 * @return Return nothing
 */
void mergeSort(void* const data, void* const supporting, size_t left, size_t right, size_t size, int (* comparator)(const void * elem1, const void * elem2));

/**
 * @brief A function that compares strings in alphabetical order from end of the line to the begin
 * @param [in] str1 - the first string
 * @param [in] str2 - the second string
 * @return 0              - if strings are the same
 *         positive value - if str1 is after str2 in alphabetical order (end->begin)
 *         negative value - if str1 is before str2 in alphabetical order (end->begin)
 */
int rightLineCmp(const void* str1, const void* str2);

/**
 * @brief A function that compares strings by their index in source text 
 * @param [in] str1 - the first string
 * @param [in] str2 - the second string
 * @return 0              - if strings are the same
 *         positive value - if str1 is after str2 
 *         negative value - if str1 is before str2 
 */
int sourceLineCmp(const void* str1, const void* str2);

/**
 * @brief A function that skips non-letter characters in a string from the end of the line to the begin
 * @param [in] str - a pointer on the string
 * @return Pointer to a letter character in the source string
 */
void rightEatPunct(const char* str, int* len);

/**
 * @brief A function that fills lines in the file in selected order 
 * @param [in] processed - pointer on the file, what we have to fill
 * @param [in] data - pointer on the array of fileLines
 * @param [in] support - supporting array for mergesort
 * @param [in] nLines - count of the lines
 * @param [in] comparator - pointer on the function, that compare two strings in selected order
 * @return Return nothing
 */
void fillTheFile(FILE* processed, fileLines* data, size_t nLines, int (* comparator)(const void * elem1, const void * elem2) = sourceLineCmp);

/**
 * @brief A function that counts how many times a character occurs in the text
 * @param [in] buf - pointer on the text
 * @param [in] search - the searchable character
 * @param [in] count - pointer on the counter
 * @return Return nothing
 */
void countInText(char* buf, char search, size_t* count);

/**
 * @brief A function that read TEXT structure by reading the source file 
 * @param [in] text - pointer on a new TEXT structure
 * @param [in] source - pointer on a FILE with source text
 * @return Return nothing
 */
void readInBuf(TEXT* text, FILE* source);

/**
 * @brief A function for finding line's start and end in the text 
 * @param [in] text - source text
 * @return Return nothing 
 */
void matchLines(TEXT* text);

void helperRun(void);

void textDtor(TEXT* text); 

void wrapMergeSort(void* const data, size_t left, size_t right, size_t size, 
               int (* comparator)(const void * elem1, const void * elem2));
