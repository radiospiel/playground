#include <cstdio>

char *sprintf_utoa(unsigned long number, char *buf, size_t buflen) {
    snprintf(buf, buflen, "%ld", number);
    return buf;
}
