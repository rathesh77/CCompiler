#define TABLE_LEVELS 10

char* table[TABLE_LEVELS][4] = {
    {"("},
    {"FN_CALL"},
    {"VAR"},
    {"*", "/"},
    {"+", "-"},
    {"<", ">", "<=", ">="},
    {"=="},
    {"ET"},
    {"OU"},
    {"="},
};