#include "mfind.h"

Stack *s_stack;
char *s_name;
char s_type = 'f';
int nrthr = 1;
int thread_waiting = 0;
pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t stack_cond = PTHREAD_COND_INITIALIZER;;


char check_filetype(char *fullname)  {
  struct stat file_info;
  if (lstat(fullname, &file_info) < 0) {
    fprintf(stderr, "Path does not exist: %s", fullname);
    return 'u';
  }
  switch (file_info.st_mode & S_IFMT) {
    case S_IFDIR:
      return 'd';
    case S_IFREG:
      return 'f';
    case S_IFLNK:
      return 'l';
    default:
      return 'u';
  }
}

/* search() - Searches directories
* Iterates all directories for links/files/folders.
* returns: number of calls to opendir.
*/
int search(char *folder) {
  int i = 0;
  DIR *dp;
  char* sub_folder = NULL;
  char *fullname;
  struct dirent *dir_entry;
  struct stat file_info;
  if ((dp = opendir(folder)) != NULL) {
    while((dir_entry = readdir(dp))){
      i++;
      if((fullname = malloc((strnlen(folder, PATH_MAX)
                                 + strnlen(dir_entry->d_name, PATH_MAX)
                                 + 2)
                                 * sizeof(char))) == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
      }
      sprintf(fullname, "%s/%s", folder, dir_entry->d_name);
      switch (check_filetype(fullname)) {
        case 'd':
          if(dir_entry->d_name[0] != '.') {
            if((sub_folder = malloc((strnlen(folder, PATH_MAX)
                                       + strnlen(dir_entry->d_name, PATH_MAX)
                                       + 2)
                                       * sizeof(char))) == NULL){

              perror("malloc");
              exit(EXIT_FAILURE);
            }
            sprintf(sub_folder, "%s/%s", folder, dir_entry->d_name);
            pthread_mutex_lock(&stack_mutex);
            stack_push(s_stack, sub_folder);
            if (thread_waiting > 0) {
              pthread_cond_signal(&stack_cond);
              thread_waiting--;
            }
            pthread_mutex_unlock(&stack_mutex);
            if (strcmp(dir_entry->d_name, s_name) == 0 && s_type == 'd') {
              printf("Found directory '%s': %s\n", s_name, sub_folder);
            }
          }
          break;
        case 'f':
          if (strcmp(dir_entry->d_name, s_name) == 0 && s_type == 'f') {
            printf("Found file '%s': %s/%s\n", s_name, folder, s_name);
          }
          break;
        case 'l':
          if (strcmp(dir_entry->d_name, s_name) == 0 && s_type == 'l') {
            printf("Found link '%s': %s/%s\n", s_name, folder, s_name);
          }
          break;
        default:
          //fprintf(stderr, "Unknown file type\n");
          break;
      }
      free(fullname);
    }
    closedir(dp);
  }else {
    perror("Error in directory");
  }
  return i;
}

/* search() - Searches directories
* Iterates all directories for links/files/folders.
* returns: number of calls to opendir.
*/
void add_directories(int argc, char **argv, int optind) {
  char *directory = NULL;
  if (argc - optind < 2) {
    fprintf(stderr, "mfind [-t type] [-p nrthr] start1 [start2 ...] name\n");
    exit(EXIT_FAILURE);
  }
  struct stat sb;
  for (int i = optind; i < argc-1; i++) {
    if (argv[i][0] != '/') {
      if((directory = malloc((strnlen(argv[i], PATH_MAX) + 2)
                                                  * sizeof(char))) == NULL)  {
        perror("malloc");
        exit(EXIT_FAILURE);
      }
      sprintf(directory, "/%s", argv[i]);
      if ((stat(directory, &sb) && S_ISDIR(sb.st_mode)) == 0) {
        pthread_mutex_lock(&stack_mutex);
        stack_push(s_stack, strndup(directory, PATH_MAX));
        pthread_mutex_unlock(&stack_mutex);
      }else {
        fprintf(stderr, "Directory: %s does not exist\n", directory);
      }
      free(directory);
    }else {
      if ((stat(argv[i], &sb) && S_ISDIR(sb.st_mode)) == 0) {
        pthread_mutex_lock(&stack_mutex);
        stack_push(s_stack, strndup(argv[i], PATH_MAX));
        pthread_mutex_unlock(&stack_mutex);
      }else {
        fprintf(stderr, "Directory: %s does not exist\n", argv[i]);
      }
    }
  }
}

/* search() - Searches directories
* Iterates all directories for links/files/folders.
* returns: number of calls to opendir.
*/
void *main_thread(void *thr_ptr) {
  if(((long*)thr_ptr) != 0)  {
    fprintf(stderr, "Thread error\n");
  }
  int i = 0;
  int status;
  char *folder;
  while(thread_waiting >= 0) {
    pthread_mutex_lock(&stack_mutex);
    while (stack_is_empty(s_stack)) {
      thread_waiting++;
      if (thread_waiting >= nrthr) {
        pthread_cond_broadcast(&stack_cond);
        thread_waiting = -1;
        fprintf(stderr, "Thread: %ld, Reads: %d\n", pthread_self(), i);
        pthread_mutex_unlock(&stack_mutex);
        return 0;
      }
      pthread_cond_wait(&stack_cond, &stack_mutex);
      if (thread_waiting < 0) {
        fprintf(stderr, "Thread: %ld Reads: %d\n", pthread_self(), i);
        pthread_mutex_unlock(&stack_mutex);
        return 0;
      }
    }
    folder = strndup(stack_top(s_stack), PATH_MAX);
    stack_pop(s_stack);
    pthread_mutex_unlock(&stack_mutex);
    i += search(folder);
    free(folder);
  }
  pthread_exit(&status);
}

/* search() - Searches directories
* Iterates all directories for links/files/folders.
* returns: number of calls to opendir.
*/
void init_vars(int argc, char **argv)  {
  char *err;
  int opt;
  while ((opt = getopt(argc, argv, "t:p:")) != -1) {
    switch (opt) {
      case 't':
        if (strcmp(optarg, "l") == 0) {
          s_type = 'l';
        }
        else if (strcmp(optarg, "d") == 0) {
          s_type = 'd';
        }
        else if (strcmp(optarg, "f") == 0) {
          s_type = 'f';
        }else {
          fprintf(stderr, "Incorrect type\n");
        }
        break;
      case 'p':
        nrthr = (int)strtol(optarg, &err, 10);
        if(nrthr < 1){
          fprintf(stderr,
                  "mfind [-t type] [-p nrthr] start1 [start2 ...] name\n");
          exit(EXIT_FAILURE);
        }
        break;
      default:
        fprintf(stderr,
                "mfind [-t type] [-p nrthr] start1 [start2 ...] name\n");
        exit(EXIT_FAILURE);
    }
  }
  if (argc == 1) {
    fprintf(stderr, "mfind [-t type] [-p nrthr] start1 [start2 ...] name\n");
    exit(EXIT_FAILURE);
  }
  if (nrthr < 1) {
    fprintf(stderr, "mfind [-t type] [-p nrthr] start1 [start2 ...] name\n");
    exit(EXIT_FAILURE);
  }
  s_stack = stack_empty();
  s_name = argv[argc - 1];
}

/* search() - Searches directories
* Iterates all directories for links/files/folders.
* returns: number of calls to opendir.
*/
int main(int argc, char **argv) {
  init_vars(argc, argv);
  add_directories(argc, argv, optind);
  pthread_t threads[nrthr];
  threads[0] = pthread_self();
  for(int thr = 1; thr < nrthr; thr++){
    if(pthread_create(&threads[thr], NULL, &main_thread, NULL)){
      perror("pthread");
      exit(EXIT_FAILURE);
    }
  }
  main_thread(NULL);
  for (int thr = 0; thr < nrthr; thr++) {
    pthread_join(threads[thr], NULL);
  }
  stack_free(s_stack);
  return 0;
}
