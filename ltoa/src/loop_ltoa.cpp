#include <cassert>

char *loop_utoa(unsigned long number, char *buf, size_t buflen) {
    static const char digits[] = "0123456789";
    char *tmp = buf + buflen;

    *tmp-- = '\0';
    do {
        *tmp-- = digits[number % 10];
        assert(tmp >= buf);
    } while (number /= 10);
    return tmp + 1;
}

