#include "UnixLs.h"

// -i option for inode number
void printInode(const char *filename){
    struct stat fileStat;
    if(lstat(filename, &fileStat) == 0){
        printf("%lu", (unsigned long)fileStat.st_ino);
    }else{
        perror("Error printing inode number");
    }
}

// -l option for long listing
void permissionMode(mode_t mode, char str[11]){
    strcpy(str, "----------");
    // File type
    if(S_ISDIR(mode)) str[0] = 'd'; // directory
    else if(S_ISLNK(mode)) str[0] = 'l'; // symbolic link
    else if(S_ISREG(mode)) str[0] = '-'; // regular file
    // User
    if(mode & S_IRUSR) str[1] = 'r'; 
    if(mode & S_IWUSR) str[2] = 'w';
    if(mode & S_IXUSR) str[3] = 'x';
    // Group
    if(mode & S_IRGRP) str[4] = 'r';
    if(mode & S_IWGRP) str[5] = 'w';
    if(mode & S_IXGRP) str[6] = 'x';
    // Others
    if(mode & S_IROTH) str[7] = 'r';
    if(mode & S_IWOTH) str[8] = 'w';
    if(mode & S_IXOTH) str[9] = 'x';
    str[10] = '\0';
}

void printSymbolicLink(const char *filename) {
    char linkTarget[1024];
    ssize_t len = readlink(filename, linkTarget, sizeof(linkTarget) - 1);
    if (len != -1) {
        linkTarget[len] = '\0';
        printf(" -> %s", linkTarget);
    }else{
        perror("Error printing symbolic link");
    }
}

void printLongListing(const char *path){
    struct stat fileStat;
    struct passwd *pw;
    struct group *grp;
    struct tm *time;
    char permission[11];
    char timeStr[20];

    if(lstat(path, &fileStat) == -1){
        perror("Error printing long listing");
        return;
    }
    permissionMode(fileStat.st_mode, permission);

    pw = getpwuid(fileStat.st_uid);
    grp = getgrgid(fileStat.st_gid);
    time = localtime(&fileStat.st_mtime);
    strftime(timeStr, 20, "%b %d %Y %H:%M", time);

    printf("%s ", permission);
    printf("%lu ", (unsigned long)fileStat.st_nlink);
    printf("%s ", pw? pw->pw_name : "Unknown");
    printf("%s ", grp? grp->gr_name : "Unknown");
    printf("%lu ", (unsigned long)fileStat.st_size);
    printf("%s ", timeStr);
    printf("%s", basename(path));

    if (S_ISLNK(fileStat.st_mode)) {
        printSymbolicLink(path);
    }

    printf("\n");
}

void listDirectory(const char *path, int inodeOption, int longOption){
    DIR *dirp = opendir(path);
    if(dirp == NULL){
        perror("Error opening directory");
        return;
    }

    struct dirent *dp;
    while((dp = readdir(dirp)) != NULL){
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, dp->d_name);
        if(inodeOption && !longOption){
            printInode(fullPath);
            printf(" %s\n", dp->d_name);
        }
        if(longOption && !inodeOption){
            printLongListing(fullPath);
        }
        if(inodeOption && longOption){
            printInode(fullPath);
            printf(" ");
            printLongListing(fullPath);
        }
    }
    closedir(dirp);
}

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Usage: %s [-i, -l, -il, -li] <path>\n", argv[0]);
        return 1;
    }  
    const char *path;

    int inodeOption = 0;
    int longOption = 0;
    for(int i = 1; i < argc - 1; i++){
        if(strcmp(argv[i], "-i") == 0){
            inodeOption = 1;
        }else if(strcmp(argv[i], "-l") == 0){
            longOption = 1;
        }else if(strcmp(argv[i], "-il") == 0 || strcmp(argv[i], "-li") == 0){
            inodeOption = 1;
            longOption = 1;
        }else{
            printf("Invalid option: %s\n", argv[i]);
            return 1;
        }
    }

    path = argv[argc - 1];

    listDirectory(path, inodeOption, longOption);
    return 0;
}