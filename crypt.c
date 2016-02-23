#include "crypt.h"

//********************************************************************************************
//**-------------------------------------FILE CRYPTION--------------------------------------**
//********************************************************************************************
int crypt_file(const char *path, const char *fileName, unsigned char *key) {

	int outlen, inlen;
	
	FILE *file, *temp;
	file = fopen(fileName,"r+b");
	if(file == NULL) {
		return -2;
	}
	
	char tempName[PATH_SIZE];
	cnct(path,"TemP.TemP",tempName);
	temp = fopen(tempName,"wb");

	unsigned char iv[8] = "DAJ-7l2"; /* вектор инициализации */ 
	unsigned char inbuf[BUF_SIZE], outbuf[BUF_SIZE]; 
	
	EVP_CIPHER_CTX ctx;
	const EVP_CIPHER * cipher;

	EVP_CIPHER_CTX_init(&ctx);
	cipher = EVP_bf_cbc();

	EVP_EncryptInit(&ctx, cipher, key, iv);

	while(1) {
		inlen = fread(inbuf, 1, BUF_SIZE, file);
		if(inlen <= 0) break;
		if(!EVP_EncryptUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
			return 1;
		fwrite(outbuf, 1, outlen, temp);
	}

	if(!EVP_EncryptFinal(&ctx, outbuf, &outlen)) 
		return 1; 
	fwrite(outbuf, 1, outlen, temp); 
	EVP_CIPHER_CTX_cleanup(&ctx);

	fclose(file);
	fclose(temp);

	remove(fileName);
	rename(tempName,fileName);

	return 0;
}

int decrypt_file(const char *path, const char *fileName, unsigned char *key) {
	int outlen, inlen;
	
	FILE *file, *temp;
	file = fopen(fileName,"r+b");
	if(file == NULL) {
		return -2;
	}
	char tempName[PATH_SIZE];
	cnct(path,"TemP.TemP",tempName);
	temp = fopen(tempName,"wb");

	unsigned char iv[8] = "DAJ-7l2"; /* вектор инициализации */ 
	unsigned char inbuf[BUF_SIZE], outbuf[BUF_SIZE]; 
	EVP_CIPHER_CTX ctx;
	const EVP_CIPHER * cipher;

	EVP_CIPHER_CTX_init(&ctx);
	cipher = EVP_bf_cbc();

	EVP_DecryptInit(&ctx, cipher, key, iv);

	while(1) {
		inlen = fread(inbuf, 1, BUF_SIZE, file);
		if(inlen <= 0) break;
		if(!EVP_DecryptUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
			return 1;
		fwrite(outbuf, 1, outlen, temp);
	}

	if(!EVP_DecryptFinal(&ctx, outbuf, &outlen)) 
		return 1; 
	fwrite(outbuf, 1, outlen, temp); 
	EVP_CIPHER_CTX_cleanup(&ctx);

	fclose(file);
	fclose(temp);

	remove(fileName);
	rename(tempName,fileName);
	
	return 0;
}

int cnct(const char *dirName, const char *fileName, char buffer[]) {
	strcpy(buffer,dirName);
	strncat(buffer,"/",2);
	strncat(buffer,fileName,strlen(fileName)+2);
	return 0;
}

//*********************************************************************************************
//**------------------------------------DIR CRYPTION-----------------------------------------**
//*********************************************************************************************
int crypt_dir(const char *dirName, unsigned char *key) {
    
    char buffer[PATH_SIZE];

    DIR *dir;
    struct dirent *entry;

    dir = opendir(dirName);
    if (!dir) {
        perror("diropen");
        exit(1);
    };

    while ( (entry = readdir(dir)) != NULL) {
        if(entry->d_type != DT_DIR && strcmp(entry->d_name,".DS_Store")) {
        	cnct(dirName,entry->d_name,buffer);
        	crypt_file(dirName, buffer,key);
        }
        if(entry->d_type == DT_DIR && strcmp(entry->d_name,"..") && strcmp(entry->d_name,".")) {
        	cnct(dirName,entry->d_name,buffer);
        	crypt_dir(buffer, key);
        }
    }

    closedir(dir);
    return 0;
}

int decrypt_dir(const char *dirName, unsigned char *key) {
    char buffer[PATH_SIZE];

    DIR *dir;
    struct dirent *entry;

    dir = opendir(dirName);
    if (!dir) {
        perror("diropen");
        exit(1);
    };

    while ( (entry = readdir(dir)) != NULL) {
        if(entry->d_type != DT_DIR && strcmp(entry->d_name,".DS_Store")) {
        	cnct(dirName,entry->d_name,buffer);
        	decrypt_file(dirName,buffer,key);
        }
        if(entry->d_type == DT_DIR && strcmp(entry->d_name,"..") && strcmp(entry->d_name,".")) {
        	cnct(dirName,entry->d_name,buffer);
        	decrypt_dir(buffer, key);
        }
    }

    closedir(dir);
    return 0;
}
