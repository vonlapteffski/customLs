#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h> 
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define PATH_BUF_LENGTH 4096

int print_row(char *file_path, char* file_name){

  char time_buff[20]; // Buffer for time string
  char link_buff[PATH_BUF_LENGTH];
  struct passwd user_data; // Struct for getting data from userID
  struct group group_data; // Struct for getting data from groupID
  struct stat file_stat;

  if(lstat(file_path,&file_stat) < 0)    
      return 1;

  user_data = *getpwuid(file_stat.st_uid);
  group_data = *getgrgid(file_stat.st_uid);

  strftime(time_buff, 20, "%b %e %H:%M", localtime(&file_stat.st_mtime));

  // Permissions:
  putchar( (S_ISDIR(file_stat.st_mode)) ? 'd' : '-');
  putchar( (file_stat.st_mode & S_IRUSR) ? 'r' : '-');
  putchar( (file_stat.st_mode & S_IWUSR) ? 'w' : '-');
  putchar( (file_stat.st_mode & S_IXUSR) ? 'x' : '-');
  putchar( (file_stat.st_mode & S_IRGRP) ? 'r' : '-');
  putchar( (file_stat.st_mode & S_IWGRP) ? 'w' : '-');
  putchar( (file_stat.st_mode & S_IXGRP) ? 'x' : '-');
  putchar( (file_stat.st_mode & S_IROTH) ? 'r' : '-');
  putchar( (file_stat.st_mode & S_IWOTH) ? 'w' : '-');
  putchar( (file_stat.st_mode & S_IXOTH) ? 'x' : '-');
  putchar(' ');
  printf("%lu",file_stat.st_nlink); // Num of links
  putchar(' ');
  printf("%s", user_data.pw_name); // Owner's username
  putchar(' ');
  printf("%s", group_data.gr_name); // Owner's groupname
  putchar(' ');
  printf("%ld", file_stat.st_size); // File size
  putchar(' ');
  printf("%s", time_buff);
  putchar(' ');

  if(S_ISLNK(file_stat.st_mode)) { // If dir is symbolic link
    readlink(file_path, (char*)&link_buff, PATH_BUF_LENGTH);
    printf("%s -> ", file_name);
    printf("%s", link_buff);
    putchar('\b');
  }
  else
    printf("%s", file_name);

  putchar('\n');
  return 0;
}

void open_dir(char* dirname) {
  DIR *d;
  char *path_buf;
  struct dirent *dir;
  int errnum;

  d = opendir(dirname);

  if (d) { // Open dir success

    while ((dir = readdir(d)) != NULL) {

      path_buf = malloc(sizeof(dirname) + sizeof(dir->d_name) + 2);
      sprintf(path_buf, "%s/%s", dirname, dir->d_name);
      print_row(path_buf, dir->d_name);
    }

    closedir(d);
    free(path_buf);
  }
  else {
    errnum = errno;
    fprintf(stderr, "ls: %s: %s\n", dirname, strerror(errnum));
  }
}

int main(int argc, char **argv)
{
  int i;
  int num_min_l=0; // Number of -l in arg list

  // If no arguments
  if(argc < 2) {
    printf("Please use -l argument\n");
    return 1;
  }   
  // Search for '-l' argument
  for(i = 1;i < argc;i++) {
    if(strcmp(argv[i], "-l") == 0){
      num_min_l = i;
      break;
    }
  }
  // If no '-l' argument
  if(num_min_l == 0) { 
    printf("Please use -l argument\n");
    return 1;
  }

  for(i = 1;i < argc;i++){ // Opening dirs by paths

    if(i == num_min_l) continue; // Skipping -l as path

    open_dir(argv[i]);
  }

  if(i == 2) open_dir("."); // When no arguments excepts -l
}

