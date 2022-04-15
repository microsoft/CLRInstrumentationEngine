#define ERRHR(C) ((unsigned long)0x8013 | (unsigned long)C)
#define META_E_BAD_SIGNATURE ERRHR(0x1192)
#define CLDB_E_RECORD_NOTFOUND ERRHR(0x1130)