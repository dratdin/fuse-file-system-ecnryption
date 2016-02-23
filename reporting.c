#include "reporting.h"

void clear_journal() {
	FILE *f = fopen("/usr/local/report-ffs.txt","w+");
	fclose(f);
}

void add_journal_note(const char *message) {
	FILE *f = fopen("/usr/local/report-ffs.txt","a+");
	fputs(message,f);
	fclose(f);
}
