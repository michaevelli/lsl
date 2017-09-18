#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MAXDIRNAME 100
#define MAXFNAME   200
#define MAXNAME    20
#define MAXENTRIES 100
#define MONTHS 60*60*24*30*3

char *rwxmode(mode_t, char *);
char *username(uid_t, char *);
char *groupname(gid_t, char *);
int cmp (const void * a, const void * b);

int main(int argc, char *argv[])
{
   char errno;
   // string buffers for various names
   char dirname[MAXDIRNAME];
   char uname[MAXNAME+1];
   char gname[MAXNAME+1];
   char mode[MAXNAME+1];

   // collect the directory name, with "." as default
   if (argc < 2)
      strcpy(dirname, ".");
   else
      strcpy(dirname, argv[1]);

   // check that the name really is a directory
   struct stat info;
   if (stat(dirname, &info) < 0)
      { perror(argv[0]); exit(EXIT_FAILURE); }
   if ((info.st_mode & S_IFMT) != S_IFDIR)
      { fprintf(stderr, "%s: Not a directory\n",argv[0]); exit(EXIT_FAILURE); }

   // open the directory to start reading
   DIR *df;
   df = opendir(dirname);
   if(df == NULL){
      perror(&errno);
      exit(EXIT_FAILURE);
   }

   // read directory entries, store in array to sort alphabetically later
   struct dirent *entry;
   entry = readdir(df);
   char entries[MAXENTRIES][7][MAXDIRNAME];
   for(int i = 0; i < MAXENTRIES; i++){
      for(int j = 0; j < 2; j++){
         strcpy(entries[i][j],"");
      }
   }  

   int counter = 0;
   int blocks = 0;
   while(entry != NULL){
      //ignore .
      if(entry->d_name[0] == '.'){
         entry = readdir(df);
         continue;
      }
      struct stat *info = malloc(sizeof(struct stat));
      //prepend file path
      char file[MAXDIRNAME] = {0};
      strcat(file, dirname);
      strcat(file, "/");
      strcat(file, entry->d_name);

      int suc = lstat(file, info);
      if(suc == 0){
         //read info
         mode_t ModeInfo = (info->st_mode);
         uid_t OwnerUID = (info->st_uid);
         gid_t GroupGid = (info->st_gid);
         off_t Size = (info->st_size);
         char* ObjectName = entry->d_name;

         blocks += (info->st_blocks);
         
         nlink_t links = (info->st_nlink);
         //create time string from last modified date
         time_t tim = (info->st_mtime);
         time_t now;
         time(&now);
         char time_s[14] = "";
         strftime(time_s, 14, "%b %d %H:%M", localtime(&tim));
         if(now - tim > MONTHS){
            strftime(time_s, 14, "%b %d  %Y", localtime(&tim));
         }
         if(time_s[4] == '0'){
            time_s[4] = ' ';
         }

         //format permissions, user/group names
         rwxmode(ModeInfo, mode);
         username(OwnerUID, uname);
         groupname(GroupGid, gname);         

         //store info in array
         sprintf(entries[counter][1], "%s",
            mode);
         sprintf(entries[counter][2], "%lu",
            links);
         sprintf(entries[counter][3], "%s",
            uname);
         sprintf(entries[counter][4], "%s",
            gname);
         sprintf(entries[counter][5], "%lld",
            (long long)Size);
         if(mode[0] != 'l'){
            sprintf(entries[counter][6], "%s %s",
               time_s,
               ObjectName);                             
         } else {
            char buf[MAXDIRNAME] = {0};
            ssize_t len;
            len = readlink(file, buf, sizeof(buf)-1);
            buf[len] = '\0';
            sprintf(entries[counter][6], "%s %s -> %s",
               time_s,
               ObjectName,
               buf);                             
         }
         //store file name (used for sorting)
         sprintf(entries[counter][0], "%s", ObjectName);
      } else {
         perror(&errno);
         exit(EXIT_FAILURE);
      }
      free(info);
      entry = readdir(df);
      counter++;
   }

   // close
   closedir(df);

   //sort
   qsort(entries, counter, 7*MAXDIRNAME, cmp);

   //determine max length of variable length strings excluding last
   int lengths[4] = {0};
   for(int i = 0; i < counter; i++){
      int length[4] = {strlen(entries[i][2]),
          strlen(entries[i][3]),
          strlen(entries[i][4]),
          strlen(entries[i][5])};
      for(int j = 0; j < 4; j++){
         if(length[j] > lengths[j]){
            lengths[j] = length[j];
         }
      }
   }

   //print output
   printf("total %d\n", blocks/2);
   for(int i = 0; i < counter; i++){
      for(int j = 1; j < 7; j++){
         //print buffer blankspace
         if(j >= 2 && j <= 5){
            for(int k = strlen(entries[i][j]); k < lengths[j-2]; k++){
               printf(" ");
            }
         }
         printf("%s", entries[i][j]);
         if(j < 6){
            printf(" ");
         }
      }
      printf("\n");
   }

   return EXIT_SUCCESS;
}

int cmp (const void * a, const void * b){
   return strcmp(a, b);
}

// convert octal mode to -rwxrwxrwx string
char *rwxmode(mode_t mode, char *str)
{
   str[10] = '\0';
   str[0] = '?';
   //determine type of file
   if((S_IFDIR & mode) == S_IFDIR){
      str[0] = 'd';
   } else if((S_IFLNK & mode) == S_IFLNK){
      str[0] = 'l';
   } else if((S_IFREG & mode) == S_IFREG){
      str[0] = '-';
   }
   //masking method to determine rwx perms
   int mask = 1;
   for(int i = 9; i > 0; i--){
      if((mask & mode) == mask){
         str[i] = '1';
         if(i % 3 == 1){
            str[i] = 'r';
         } else if(i % 3 == 2){
            str[i] = 'w';
         } else {
            str[i] = 'x';
         }
      } else {
         str[i] = '-';
      }
      mask = mask << 1;
   }
   return str;
}

// convert user id to user name
char *username(uid_t uid, char *name)
{
   struct passwd *uinfo = getpwuid(uid);
   if (uinfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)uid);
   else
      snprintf(name, MAXNAME, "%s", uinfo->pw_name);
   return name;
}

// convert group id to group name
char *groupname(gid_t gid, char *name)
{
   struct group *ginfo = getgrgid(gid);
   if (ginfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)gid);
   else
      snprintf(name, MAXNAME, "%s", ginfo->gr_name);
   return name;
}
