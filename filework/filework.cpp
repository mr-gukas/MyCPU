#include "filework.h"

void textCtor(TEXT * text, FILE* const source)
{
    ASSERT(source    != NULL)
    ASSERT(text      != NULL)

    readInBuf(text, source);

    matchLines(text);
}

void readInBuf(TEXT* text, FILE* source)
{
    ASSERT(text   != NULL)
    ASSERT(source != NULL)
    
    text->nChar = fileSize(source);
    text->buf   = (char *) calloc(text->nChar + 1, sizeof(char));
    
    fread(text->buf, sizeof(char), text->nChar, source);

}

size_t fileSize(FILE* file)
{
    ASSERT(file != NULL)

    struct stat buf = {};

    fstat(fileno(file), &buf);

    ASSERT(buf.st_size >= 0)

    return (size_t) buf.st_size;
}

void matchLines(TEXT* text)
{
    size_t   line        = 0;
    size_t   lineStart   = 0;
    
    countInText(text->buf, '\n', &(text->nLines));
    text->lines = (fileLines*) calloc(text->nLines + 1, sizeof(fileLines));
    
    for (size_t index = 1; index < text->nChar; index++)
    {
        if (*(text->buf + index) == '\n')
        {
            (*(text->buf + index)= '\0');

            (text->lines + line)->lineStart = text->buf + lineStart;
            (text->lines + line)->lineLen   = index - lineStart;
            (text->lines + line)->lineIndex = line;
            lineStart                       = index + 1;
            ++line;
        }

        if (lineStart > text->nChar)
        {
            break;
        }
    }

}
     
void countInText(char* text, char search, size_t* count)
{
    size_t index = 0;

    while (*(text + index))  
    {
        if (*(text + index) == search)
        {
            ++(*count);
        }
        ++index;
    }
}

void fillTheFile(FILE* processed, fileLines* data, size_t nLines, 
                 int (* comparator)(const void * elem1, const void * elem2))
{
    wrapMergeSort(data, 0, nLines - 1, sizeof(fileLines), comparator);
    
    for (size_t index = 0; index < nLines; ++index)
    {
        fputs(data[index].lineStart, processed);
        fputc('\n', processed);
    }
    
    fputs("\n*********************************************************\n", processed);
}

//-------------------------------------------------------------------------------------
void wrapMergeSort(void* const data, size_t left, size_t right, size_t size, 
               int (* comparator)(const void * elem1, const void * elem2))
{
    void* support = calloc(right+1, size);
    mergeSort(data, support, left, right, size, comparator);
    free(support);

}

void mergeSort(void* const data, void* const supporting, size_t left, size_t right, size_t size, 
               int (* comparator)(const void * elem1, const void * elem2))
{
    ASSERT(data       != NULL)
    ASSERT(supporting != NULL)
    
    char* dat = (char *) data;
    char* sup = (char *) supporting;

    if (right <= left)
    {
        return;     
    }

    size_t mid = (left + right) / 2;
          
    mergeSort(data, supporting, left,    mid  , size, comparator);
    mergeSort(data, supporting, mid + 1, right, size, comparator);

    size_t pointer_left  = left;
    size_t pointer_right = mid + 1;
    size_t index         = 0;

    for (index = left; index <= right; index++)
    {
        if (pointer_left == mid + 1)
        {      
            memcpy(sup + index * size, dat + pointer_right * size, size);
            pointer_right++ ;
        }
        else if (pointer_right == right + 1)
        {
            memcpy(sup + index * size, dat + pointer_left * size, size);
            pointer_left++;
        }
        else if (comparator((const void*) (dat + pointer_left * size), (const void*) (dat + pointer_right * size)) < 0)
        {
            memcpy(sup + index * size, dat + pointer_left * size, size);
            pointer_left++;
        }
        else
        {
            memcpy(sup + index * size, dat + pointer_right * size, size);
            pointer_right++;
        }
    }

    for (index = left; index <= right; index++)
    {
        memcpy(dat + index * size, sup + index * size, size);
    }
}
   
int leftLineCmp(const void* str1, const void* str2)
{
    ASSERT(str1 != NULL)
    ASSERT(str2 != NULL)

    const char* line1 = ((const fileLines*) str1)->lineStart;
    const char* line2 = ((const fileLines*) str2)->lineStart;

    while (*line1 && *line2)
    {
        line1 = leftEatPunct(line1);
        line2 = leftEatPunct(line2);

        if (*line1 && *line2 && toupper(*line1) != toupper(*line2))
        {
            return (toupper(*line1) - toupper(*line2));
        }
        else if (*line1 && *line2)
        {
            ++line1;
            ++line2;
        }
    }

    return *line1 - *line2; 
}

const char* leftEatPunct(const char* str)
{
    ASSERT(str!= NULL)

    while (*str && !isalpha(*str))
    {
        ++str;
    }

    return str;
}

int rightLineCmp(const void* str1, const void* str2)
{    
    ASSERT(str1 != NULL)
    ASSERT(str2 != NULL)

    const  fileLines* line1 = (const fileLines*) str1;
    const  fileLines* line2 = (const fileLines*) str2;
    
    int index1 = (int) line1->lineLen - 1;
    int index2 = (int) line2->lineLen - 1;
    
    while (index1 != -1 && index2 != -1)
    {
        rightEatPunct(line1->lineStart, &index1);
        rightEatPunct(line2->lineStart, &index2);

        if (index1 >= 0 && index2 >= 0 && toupper(*(line1->lineStart + index1)) != 
                                          toupper(*(line2->lineStart + index2)))
        {
            return toupper(*(line1->lineStart + index1)) - 
                   toupper(*(line2->lineStart + index2));
        }
        else if (index1 >= 0 && index2 >= 0)
        {
            --index1;
            --index2;
        }
    }

    return (index1 - index2); 
}
    
void rightEatPunct(const char* str, int* index)
{
    ASSERT(str != NULL)

    while (*index != -1  && !isalpha(*(str + (*index))))
    {
        --(*index);
    }
}

int sourceLineCmp(const void* str1, const void* str2)
{
    ASSERT(str1 != NULL)
    ASSERT(str2 != NULL)

    const  fileLines* line1 = (const fileLines*) str1;
    const  fileLines* line2 = (const fileLines*) str2;

    return (int) line1->lineIndex - (int) line2->lineIndex;
}

void textDtor(TEXT* text)
{
    ASSERT(text != NULL)

    free(text->buf); 
    free(text->lines);
}

