#include <stdio.h>
#include <string.h>

#define true 1
#define false 0

int convertASCIIHexToInt(char c) {
    if(c >=65 && c <= 70) { // c is a uppercase letter
        return c-55;
    } else if(c >=97 && c <= 102) { // c is a lowercase letter
        return c-87;
    } else if(c >=48 && c <= 57) { // c is a number
        return c-48;
    } else { // Not a hex digit
        return -1;
    }
}

int main(int argc, char **argv)
{
    // By default assume bin and xout are true
    int bin = true;
    int bout = false;
    int xin = false;
    int xout = true;

    for (int i = 1; i < argc; i++) {
        // First thing in this array is the program name, so start at the first flag.
        // Only need to check for bout and xin since bin and xout are the defaults
        if(strcmp(argv[i], "-bout") == 0) {
            bout = true;
            xout = false;
        } else if(strcmp(argv[i], "-xin") == 0) {
            xin = true;
            bin = false;
        }
    }

    if(bin) {
        // Set stdin to binary mode
        freopen(NULL, "rb", stdin);

        unsigned char buf[102400]; // Unsigned char buffer to prevent problems with printf
        size_t bytes_read = fread(buf, 1, sizeof(buf), stdin); // Read the file from stdin

        if(xout) {
            int counter = 0;
            for(int i=0; i < bytes_read; i++) {
                if(counter == 16) {
                    printf("\n");
                    counter = 0;
                }

                int tmp = buf[i];
                printf("%02x ", tmp);
                counter++; // Increase the counter by 1
            }
            printf("\n");
        } else if(bout) {
            for(int i=0; i < bytes_read; i++) {
                printf("%c", buf[i]);
            }
        }

    } else if(xin) {
        unsigned char buf[102400]; // Unsigned char buffer to prevent problems with printf
        size_t bytes_read = fread(buf, 1, sizeof(buf), stdin); // Read the file from stdin
        int ascii_coded_hex = true; // Assume it is ascii coded hex
        int i;

        // Make sure there is only 0-9, a-f, and A-F in the file
        for(i=0; i < bytes_read; i++) {
            if(buf[i] > 102) { // After a-f in ASCII
                ascii_coded_hex = false;
                break;
            } else if(buf[i] > 70) { // After A-F in ASCII
                if(buf[i] < 97) { // Before a-f in ASCII
                    ascii_coded_hex = false;
                    break;
                }
            } else if(buf[i] > 57) { // After 0-9 in ASCII
                if(buf[i] < 65) { // Before A-F in ASCII
                    ascii_coded_hex = false;
                    break;
                }
            } else if(buf[i] < 48 && buf[i] != 32 && buf[i] != 10) {
                // Before 0-9 in ASCII and not whitespace or newline
                ascii_coded_hex = false;
                break;
            }
        }

        if(ascii_coded_hex) {
            if(xout) {
                int counter = 0;
                int needSpace = false;
                for(i=0; i < bytes_read; i++) {
                    if(counter == 16) {
                        printf("\n");
                        counter = 0;
                    }

                    if(buf[i] == 32 || buf[i] == '\n') // Ignore whitespace, go to the next iteration
                        continue;

                    if(needSpace) {
                        printf("%c ", buf[i]);
                        needSpace = false;
                    } else {
                        printf("%c", buf[i]);
                        needSpace = true;
                    }
                    counter++; // Increase the counter by 1
                }
            } else if(bout) { // Convert from ASCII coded hex to binary
                for(i=0; i < bytes_read-2; i++) {
                    int done = false;
                    // Check for whitespace or newline, and skip it until a hex digit is found
                    while(buf[i] == 32 || buf[i] == '\n') {
                        i++;
                        if(i > bytes_read) {
                            done = true;
                            break;
                        }
                    }
                    if(done) { // No more things in the buffer to look at
                        continue;
                    }
                    // Get first hex digit and convert it to an integer, and move onto the next hex digit
                    int high = convertASCIIHexToInt(buf[i++]);

                    int onlyHigh = false;
                    if(i > bytes_read) {
                        onlyHigh = true;
                    }

                    // Check for whitespace or newline, and skip it until a hex digit is found
                    while(buf[i] == 32 || buf[i] == '\n') {
                        i++;
                        if(i > bytes_read) {
                            onlyHigh = true;
                            break;
                        }
                    }
                    // Get second hex digit and convert it to an integer
                    int low;
                    if(!onlyHigh) {
                        low = convertASCIIHexToInt(buf[i]);
                    }

                    // In integer form, the binary number will be high*16 + low, print this character
                    if(!onlyHigh) {
                        int binVal = high*16 + low;
                        printf("%c", binVal);
                    } else { // Assume the low is 0
                        int binVal = high*16;
                        printf("%c", binVal);
                    }
                }
            }
        } else {
            printf("error: '%c' is not a hexadecimal digit\n", buf[i]);
        }
    }

    return 0;
}
