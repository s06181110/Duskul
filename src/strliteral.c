/* Duskul version 0.1.3,  2018.05.31,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE      64

static const char **literalTable = NULL;
static int literal_capacity = 0;
static int literal_count = 0;

int addLiteralString(const char *s) {
    if (literal_count >= literal_capacity) {
        literal_capacity += TABLE_SIZE;
        literalTable = realloc(literalTable,
                               sizeof(const char *) * literal_capacity);
    }
    // 重複する文字列を登録しないようにもできる。現在は順番に複製しているだけ。
    int x = literal_count;
    literalTable[literal_count++] = strdup(s);
    return x;
}

const char *literalString(int index) {
    return literalTable[index];
}
