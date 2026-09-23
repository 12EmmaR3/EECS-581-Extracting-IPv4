/* 
 * Reads lines of text from the user and extracts a single valid IPv4
 * address (optionally followed by a :port) embedded anywhere in the line.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define LINE_BUF_SIZE 4096

/* ---------------------------------------------------------------------
 * A field is valid when:
 *   - it has between 1..maxLen digits,
 *   - it has no leading zero unless the whole field is exactly "0",
 *   - its numeric value does not exceed maxValue.
 * ------------------------------------------------------------------- */
static int parseField(const char* tok, size_t len, size_t* i, int maxLen,
                       long maxValue, int* value) {
    size_t start = *i;
    long val = 0;
    int digitCount = 0;

    while (*i < len && isdigit((unsigned char)tok[*i])) {
        val = val * 10 + (tok[*i] - '0');
        digitCount++;
        (*i)++;
        /* Stop  once we have too many digits; the field
         * is invalid regardless, so stop accumulating (avoids overflow). */
        if (digitCount > maxLen) {
            return 0;
        }
    }

    if (digitCount == 0) {
        return 0; /* empty field (e.g. "..", ":", leading/trailing dot) */
    }
    if (digitCount > 1 && tok[start] == '0') {
        return 0; /* disallowed leading zero */
    }
    if (val > maxValue) {
        return 0; /* out of range */
    }

    *value = (int)val;
    return 1;
}

/* Parses a full candidate token 
 * The token must consist ONLY of
 * digits, '.', and ':' ]
 * and must match therequired grammar */
static int parseAddressToken(const char* tok, size_t len,
                              unsigned long* outAddress, int* outPort) {
    size_t i = 0;
    int octets[4];
    int k;
    int port = -1;

    for (k = 0; k < 4; k++) {
        if (!parseField(tok, len, &i, 3, 255, &octets[k])) {
            return 0;
        }
        if (k < 3) {
            if (i >= len || tok[i] != '.') {
                return 0;
            }
            i++;
        }
    }

    if (i < len) {
        if (tok[i] != ':') {
            return 0; /* stray character right after the address */
        }
        i++;
        if (!parseField(tok, len, &i, 5, 65535, &port)) {
            return 0; /* colon present but port invalid -> whole match fails */
        }
        if (i != len) {
            return 0; /* trailing garbage inside the token after the port */
        }
    }

    *outAddress = ((unsigned long)octets[0] << 24) |
                  ((unsigned long)octets[1] << 16) |
                  ((unsigned long)octets[2] << 8) |
                  ((unsigned long)octets[3]);
    *outPort = port;
    return 1;
}

static int isAddrChar(char c) {
    return isdigit((unsigned char)c) || c == '.' || c == ':';
}

/* ---------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------- */
int extractIPv4(const char* str, unsigned long* outAddress, int* outPort) {
    size_t n = strlen(str);
    size_t pos = 0;

    while (pos < n) {
        /* Skip garbage characters. */
        if (!isAddrChar(str[pos])) {
            pos++;
            continue;
        }

        /* Found the start of a maximal run of address-like characters. */
        size_t runStart = pos;
        while (pos < n && isAddrChar(str[pos])) {
            pos++;
        }
        size_t runLen = pos - runStart;

        unsigned long addr = 0;
        int port = -1;
        if (parseAddressToken(str + runStart, runLen, &addr, &port)) {
            *outAddress = addr;
            *outPort = port;
            return 1;
        }
        /* Token failed validation as a whole; move on to the next run. */
    }

    *outAddress = 0;
    *outPort = -1;
    return 0;
}

/* ---------------------------------------------------------------------
 * Driver
 * ------------------------------------------------------------------- */
static void printResult(int found, unsigned long address, int port) {
    if (!found) {
        printf("No valid IPv4 address found.\n");
        return;
    }

    unsigned int a = (address >> 24) & 0xFF;
    unsigned int b = (address >> 16) & 0xFF;
    unsigned int c = (address >> 8) & 0xFF;
    unsigned int d = address & 0xFF;

    if (port == -1) {
        printf("Extracted IPv4 address: %u.%u.%u.%u (decimal value: %lu, port: none)\n",
               a, b, c, d, address);
    } else {
        printf("Extracted IPv4 address: %u.%u.%u.%u (decimal value: %lu, port: %d)\n",
               a, b, c, d, address, port);
    }
}

int main(void) {
    char line[LINE_BUF_SIZE];

    while (1) {
        printf("Enter a line of text (or END to quit): ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            break; /* EOF on input */
        }

        /* Strip trailing newline (and a possible preceding carriage return). */
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }

        if (strcmp(line, "END") == 0) {
            printf("Program terminated.\n");
            break;
        }

        unsigned long address = 0;
        int port = -1;
        int found = extractIPv4(line, &address, &port);
        printResult(found, address, port);
    }

    return 0;
}