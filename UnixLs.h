#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>

void printInode(const char *filename);
void printLongListing(const char *filename);