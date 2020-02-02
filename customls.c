#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h> 

int printRow(char *filePath, char* fileName){

  char timeBuff[20]; // Buffer for time string
  char linkBuff[128];
  struct passwd *userData; // Struct for getting data from userID
  struct group *groupData; // Struct for getting data from groupID
  struct stat fileStat;
  if(lstat(filePath,&fileStat) < 0)    
      return 1;

  userData = getpwuid(fileStat.st_uid);
  groupData = getgrgid(fileStat.st_uid);

  strftime(timeBuff, 20, "%b %e %H:%M", localtime(&fileStat.st_mtime));

  // Permissions:
  putchar( (S_ISDIR(fileStat.st_mode)) ? 'd' : '-');
  putchar( (fileStat.st_mode & S_IRUSR) ? 'r' : '-');
  putchar( (fileStat.st_mode & S_IWUSR) ? 'w' : '-');
  putchar( (fileStat.st_mode & S_IXUSR) ? 'x' : '-');
  putchar( (fileStat.st_mode & S_IRGRP) ? 'r' : '-');
  putchar( (fileStat.st_mode & S_IWGRP) ? 'w' : '-');
  putchar( (fileStat.st_mode & S_IXGRP) ? 'x' : '-');
  putchar( (fileStat.st_mode & S_IROTH) ? 'r' : '-');
  putchar( (fileStat.st_mode & S_IWOTH) ? 'w' : '-');
  putchar( (fileStat.st_mode & S_IXOTH) ? 'x' : '-');
  putchar(' ');
  printf("%lu",fileStat.st_nlink); // Num of links
  putchar(' ');
  printf("%s", userData->pw_name); // Owner's username
  putchar(' ');
  printf("%s", groupData->gr_name); // Owner's groupname
  putchar(' ');
  printf("%ld", fileStat.st_size); // File size
  putchar(' ');
  printf("%s", timeBuff);
  putchar(' ');
  if(S_ISLNK(fileStat.st_mode)) {
    readlink(filePath, (char*)&linkBuff, 128);
    printf("%s -> ", fileName);
    printf("%s", linkBuff);
    putchar('\b');
  }
  else
    printf("%s", fileName);
  putchar('\n');
  //free(userData);
  //free(groupData);
  return 0;
}

int main(int argc, char **argv)
{
  int i;
  int numMinL=0; // Number of -l in arg list
  int pathsCount=0; // Quantity of paths
  char *paths[32]; // Arr of paths

  // If no arguments
  if(argc < 2) {
    printf("Please use -l argument\n");
    return 1;
  }   
  // Search for '-l' argument
  for(i=1;i<argc;i++){
    if((argv[i][0]=='-')&&(argv[i][1]=='l')){
      numMinL=i;
      break;
    }
  }
  // If no '-l' argument
  if(numMinL==0) { 
    printf("Please use -l argument\n");
    return 1;
  }
  // Saving all paths
  for(i=1;i<argc;i++){
    if(i!=numMinL)
      paths[pathsCount++]=argv[i];
  }

  DIR *d;
  char pathBuf[258];
  struct dirent *dir;

  for(i=0;i<pathsCount;i++){
    d = opendir(paths[i]);
    if (d){
      readdir(d);
      readdir(d); // Crutch to skip this and parent dir
      while ((dir = readdir(d)) != NULL) {
        sprintf(pathBuf, "%s/%s", paths[i], dir->d_name);
        printRow(pathBuf, dir->d_name);
      }
      closedir(d);
    }
    else {
      printf("ls: cannot access '%s': No such file or directory\n", paths[i]);
    }
  }
}

