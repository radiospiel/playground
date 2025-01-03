#include <cassert>
#include <string>

// a single number expansion has 10 characters. It is important that this is a "char[]"
#define EXPANSION_SIZE 10

class BcdNumber {
public:
    char bcd[EXPANSION_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char *_to_s = nullptr;

    void set(unsigned long number) {
        char *ptr = bcd + (EXPANSION_SIZE - 1);
        while (ptr > bcd) {
            *ptr = (char) (number % 10);
            number /= 10;
            ptr--;
        }
    }

    inline void add(const BcdNumber &bcdNumber) {
        for (unsigned long i = 0; i < EXPANSION_SIZE; ++i) {
            bcd[i] += (char) bcdNumber.bcd[i];
        }
    }

    std::string _inspect;
    const char* inspect() {
        _inspect= "[";
        for (unsigned long i = 0; i < EXPANSION_SIZE; ++i) {
            if (i > 0) {
                _inspect += ", ";
            }
            _inspect += inspect_digit(bcd[i]);
        }
            _inspect += "]";
        return _inspect.c_str();
    }
    std::string inspect_digit(char ch) {
        if (ch >= '0' && ch <= '9') {
            return "\"" + std::string(1, '0' + ch) + "\"";
        } else if (ch >= 0 && ch <= 9) {
            return std::string(1, '0' + ch);
        } else {
            return "?";
        }
    }

private:
    void applyCarryOver() {
        unsigned idx = EXPANSION_SIZE - 1;

        while (idx > 0) {
            while (bcd[idx] > 9) {
                bcd[idx] -= 10;
                bcd[idx-1] += 1;
            }

            idx--;
        }
    }

public:
    char *to_s(char* buf, unsigned buflen) {
        assert(buflen >= EXPANSION_SIZE);

        applyCarryOver();

        // find the first digit which is not `0`, but stop one char early just in case.
        char *s = bcd;
        while (s < bcd + EXPANSION_SIZE-1 && !*s) {
            ++s;
        }

        // start conversion there.
        char* d = buf;
        while (s < bcd + EXPANSION_SIZE) {
            *d++ = *s++ + (char)'0';
        }
        *d = '\0';
        return buf;
    }
};

// a multiplication table holds expansions calculated by iterating over all numbers between 0..15, and multiplying it with a factor.
struct MultiplicationTable {
    BcdNumber numbers[16];

    void setup(unsigned factor) {
        for (size_t i = 0; i < 16; ++i) {
            numbers[i].set(i * factor);
        }
    }
};

struct MultiplicationTables {
    MultiplicationTable tables[8];

    MultiplicationTables() {
        unsigned factor = 1;
        for (auto &table: tables) {
            table.setup(factor);
            factor *= 16;
        }
    }
};

static MultiplicationTables multiplicationTables;

char *table_utoa(unsigned long number, char *buf, size_t buflen) {
    BcdNumber sum;
    const unsigned rounds = 8;
    for (unsigned i = 0; i < rounds; ++i) {
        // get multiplication table for this round
        MultiplicationTable *pTable = multiplicationTables.tables + i;
        auto xdigit = number % 16;
        number /= 16;
        BcdNumber &bcdNumber = pTable->numbers[xdigit];
        sum.add(bcdNumber);
    }

    return sum.to_s(buf, buflen);
}
