#include "users_controller.h"

int open_user_file(const char *userName) {
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT,userName,fileName);

	FILE *userFile = fopen(fileName,"r");
	if(userFile == NULL) {
		return 0;
	}
	fclose(userFile);
	return 1;
}

void create_user_file(const char *userName, unsigned char *userPass) {
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT,userName,fileName);

	FILE *userFile = fopen(fileName,"w");
	fprintf(userFile,"%s",userPass);
	fclose(userFile);
}

void add_user_dir(const char *userName,const char *dirPath) {
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT,userName,fileName);

	FILE *userFile = fopen(fileName,"a");
	fprintf(userFile,"%s\n",dirPath);
	fclose(userFile);
}

void crypt_user_dirs(const char *userName, unsigned char *pass) {
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT,userName,fileName);

	FILE *userFile = fopen(fileName,"r");
	if(userFile == NULL) {
		add_journal_note("	Error of getting open file:");
		add_journal_note(fileName);
		return;
	}
	fclose(userFile);

	char dirPath[UPATH_SIZE];
	int n = fget_count_str(fileName);
	for(int i = 2; i < n+1; i++) {
		if(!fget_str(fileName,i,dirPath)){
			add_journal_note("	Error of getting string from:");
			add_journal_note(fileName);
			return;
		}
		crypt_dir(dirPath,pass);
	}
}


void decrypt_user_dirs(const char *userName, unsigned char *pass) {
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT,userName,fileName);

	FILE *userFile = fopen(fileName,"r");
	if(userFile == NULL) {
		return;
	}
	fclose(userFile);

	char dirPath[UPATH_SIZE];
	for(int i = fget_count_str(fileName); i > 1; i--) {
		fget_str(fileName,i,dirPath);
		decrypt_dir(dirPath,pass);
	}
}


int fget_count_str(const char *fileName) {
	FILE *file = fopen(fileName,"r");
	if(file == NULL) {
		return -1;
	}

	int number_str = 0;
	char str[UPATH_SIZE];
	while(!feof(file)) {
		fgets(str,sizeof(str),file);
		number_str++;
	}
	fclose(file);

	return --number_str;
}


int fget_str(const char *fileName, int number_str, char buffer[]) {
	FILE *file = fopen(fileName,"r");
	if(file == NULL) {
		return 0;
	}

	char str[UPATH_SIZE];
	while(number_str > 0) {
		fgets(str,sizeof(str),file);
		number_str--;
	}
	strcpy(buffer,str);
	buffer[strlen(buffer)-1] = '\0';
	fclose(file);
	return 1;
}

int rename_user_dir(const char *userName, const char *from, const char *to) 
{
	add_journal_note(from);
	add_journal_note("---->");
	add_journal_note(to);
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT,userName,fileName);

	FILE *userFile = fopen(fileName,"r");
	if(userFile == NULL) {
		add_journal_note("	Error of open file:");
		add_journal_note(fileName);
		return 1;
	}
	fclose(userFile);


	char tempName[UPATH_SIZE];
	cnct(USERS_ROOT,"TemP.TemP",tempName);
	FILE *tempFile = fopen(tempName,"w");

	char buffer[UPATH_SIZE];
	int n = fget_count_str(fileName);
	for(int i = 1; i < n + 1; i++) {
		if(!fget_str(fileName, i, buffer)) {
			add_journal_note("	Error of getting string from:");
			add_journal_note(fileName);
			return 2;
		}

		if(!strcmp(buffer,from))
			fprintf(userFile,"%s\n",to);
		else 
			fprintf(userFile,"%s\n",buffer);

	}
	fclose(tempFile);

	remove(fileName);
	rename(tempName,fileName);
	return 0;
}


int input_pass(unsigned char pass[]) 
{
	struct termios initialrsettings, newrsettings;
 	tcgetattr(fileno(stdin), &initialrsettings);
 	newrsettings = initialrsettings;
 	newrsettings.c_lflag &= ~ECHO;
 	if (tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings) != 0) {
 		fprintf(stderr, "termios:	Could not set attributesn");
 	} else {
  		fgets(pass, PASSWORD_SIZE, stdin);
  		tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
 	}
 	return 0;
}


int check_pass(const char *userName, unsigned char *userPass)
{
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT,userName,fileName);

	unsigned char buffer[PASSWORD_SIZE];
	FILE *userFile = fopen(fileName,"r");
	if(userFile == NULL) {
		return 0;
	}
	fgets(buffer, PASSWORD_SIZE, userFile);
	fclose(userFile);
	return !strcmp(buffer,userPass);
}


int registration(char userName[], unsigned char userPass[]) 
{
	puts("  Input name");
	fflush(stdin);
	fgets(userName,PASSWORD_SIZE,stdin);
	userName[strlen(userName)-1] = '\0';

	puts("  Input password (max 16 symbols)");
	fflush(stdin);
	input_pass(userPass);

	if(!open_user_file(userName)) {
	    printf("  Account created %s\n", userName);
	    create_user_file(userName,userPass);
	    return 1;
	} else {
		printf("  Name %s is bisy, try again",userName);
		return 0;
	}
}


int logon(char userName[], unsigned char userPass[]) 
{
	puts("  Input name");
	fflush(stdin);
	fgets(userName,PASSWORD_SIZE,stdin);
	userName[strlen(userName)-1] = '\0';

	puts("  Input pass (max 16 symbols)");
	fflush(stdin);
	input_pass(userPass);
	
	int is_open = open_user_file(userName);
	if(is_open)
		decrypt_user_file(userName);

	if(is_open && check_pass(userName,userPass)) {
	    printf("  Welcome back %s\n",userName);
	    decrypt_user_dirs(userName,userPass);
	    return 1;
	} else {
		crypt_user_file(userName);
		printf("  Not correct name or password\n");
		return 0;
	}
}


char getch() 
{
  struct termios new,old;
  tcgetattr(0,&old);
  new = old;
  new.c_lflag &= ~(ICANON|ECHO);
  tcsetattr(0,TCSANOW,&new);   
  char ch = getchar();
  tcsetattr(0,TCSANOW,&old);
  return ch;
}

int crypt_user_file(const char *userName) {
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT, userName, fileName);

	unsigned char config_key[17] = "CONFIG-9O7U-IOP3";
	crypt_file(USERS_ROOT, fileName, config_key);
	return 0;
}

int decrypt_user_file(const char *userName) {
	char fileName[UPATH_SIZE];
	cnct(USERS_ROOT, userName, fileName);

	unsigned char config_key[17] = "CONFIG-9O7U-IOP3";
	decrypt_file(USERS_ROOT, fileName, config_key);
	return 0;
}