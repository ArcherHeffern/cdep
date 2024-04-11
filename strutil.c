#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "strutil.h"

ssize_t strip(char **line, size_t length) {
    // Strips
    while ((**line) == ' ' && length > 0) {
        (*line)++;
        length--;
    }
    while ((*(*line + length - 1) == '\n' || *(*line + length - 1) == ' ') && length > 0) {
        *(*line + length - 1) = 0;
        length--;
    }
    return length;
}



// int main() {
//     char *str = "__remotes__\n";
//     char buffer[25]; 
//     strcpy(buffer, str);
//     int len = strlen(buffer);
//     char *s = buffer;
//     int new_length = strip(&s, len);
//     printf("%s\n", s);
//     printf("Length: %d\n", new_length);
// }