#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



/*
int writeToDotOut(char *bufferToPrint, int fdToPrint) {

    
    size_t len = strlen(bufferToPrint);
    size_t done = 0;

    while (len > done) {
        ssize_t bytes_written = write(fdToPrint, bufferToPrint + done, len - done);

        if (bytes_written < 0) {
            perror("write error");
            return EXIT_FAILURE;
        }
        done += (size_t)bytes_written;
    }

    return 0;
}
*/
