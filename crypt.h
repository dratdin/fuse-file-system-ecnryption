#include <openssl/evp.h>
#include <dirent.h>
#include <string.h>

#define BUF_SIZE 1024
#define PATH_SIZE 1000
#define PASS_SIZE 17

int crypt_file(const char *, const char *, unsigned char *);
int decrypt_file(const char *, const char *, unsigned char *);


int cnct(const char *, const char *, char []);
int crypt_dir(const char *, unsigned char *);
int decrypt_dir(const char *, unsigned char *);
