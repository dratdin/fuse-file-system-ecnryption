#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypt.h"

#define USERS_ROOT ("/usr/local")
#define UPATH_SIZE 1000
#define PASSWORD_SIZE 17

int open_user_file(const char *userName);
void create_user_file(const char *userName, unsigned char *userPass);
void add_user_dir(const char *userName,const char *dirPath);
void crypt_user_dirs(const char *userName, unsigned char *);
void decrypt_user_dirs(const char *userName, unsigned char *);

int fget_count_str(const char *fileName);
int fget_str(const char *fileName,int number_str, char buffer[]);

int rename_user_dir(const char *userName,const char *from, const char *to);

int input_pass(unsigned char pass[]);
int check_pass(const char *userName, unsigned char *pass);

int logon(char userName[], unsigned char userPass[]);
int registration(char userName[], unsigned char userPass[]);

char getch();

int crypt_user_file(const char *userName);
int decrypt_user_file(const char *userName);
