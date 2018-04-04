#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

extern char **environ;

#define CWD_BUF_LEN 128  // CWD buffer size
#define LINE_BUF_LEN 128 // input line buffer size

char **parsecmd(char*, int*, int(*)[]);
void changedir(char*);
void help(void);

int main (int argc, char **argv){
  char *input;                     // line to interpret
  char *cwd = malloc(CWD_BUF_LEN); // current working directory
  char **cmds;                     // parsed list of cmds
  int cmdcount;                    // count of the cmds entered
  int redirect[4];                 // indices of redirects in order: <, >, >>, |
  size_t size = LINE_BUF_LEN;
  int background;                  // run in background if =1
  int fin,fout;                    // input and output redirect files
  int stdin_copy = dup(0);         // copy of stdin - needed when redirects complete
  int stdout_copy = dup(1);        // copy of stdout - needed when redirects complete
  FILE *bat;                       // batch file
  printf("\n\n");

  if(argc > 1){ // BATCH MODE INIT
    bat = fopen(argv[1],"r"); // open batch file for reading
    if(!bat){
      printf("Couldn't open %s for reading! Exiting...\n",argv[1]);
      return 1;
    }else
      printf("Starting to execute %s!\n", argv[1]);
  }

  // --------- MAIN LOOP ----------
  while(1){
    input = NULL; // FUTURE: support history w/ arrow keys
    getcwd(cwd, CWD_BUF_LEN);
    printf("~%s is coolguy~%s>", getlogin(), cwd);

    int r;
    for(r = 0; r < 4; r++) // initialize redirect indices to 0
      redirect[r] = 0;

    if(argc > 1){ // BATCH MODE
      if(getline(&input,&size,bat) == -1){ // get input line from batch file
        printf("Finished batch. Quitting...\n\n");
        break;
      }
    }else if(getline(&input,&size,stdin) == 1) // if nothing entered, print prompt again
      continue;

    cmds = parsecmd(input,&cmdcount,&redirect);     // parse input line into char**

    background = 0;
    if(strcmp(cmds[cmdcount-1],"&") == 0){ // checks for '&' and removes it from cmdlist
      background = 1;
      cmds[cmdcount-1] = NULL;
    }

    // I/O REDIRECTION
    fin = 0;
    fout = 0;
    if(redirect[0]){        // input redir
      cmds[redirect[0]] = NULL; // omit token so commands exec properly
      fin = open(cmds[redirect[0]+1],O_RDONLY); // open specified file for reading
      dup2(fin,0);          // replace stdin with copy of file stream
      close(fin);           // close original file
    }
    if(redirect[1]){        // output redir
      cmds[redirect[1]] = NULL;
      fout = open(cmds[redirect[1]+1],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR|S_IXUSR);
      dup2(fout,1);
      close(fout);
    }else if(redirect[2]){  // append
      cmds[redirect[2]] = NULL;
      fout = open(cmds[redirect[2]+1],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR|S_IXUSR);
      dup2(fout,1);
      close(fout);
    }else if(redirect[3]){  // pipe
      cmds[redirect[3]] = NULL;
    }

    // COMMANDS
    if(strcmp(cmds[0],"cd") == 0)          // change directory
      changedir(cmds[1]);
    else if(strcmp(cmds[0],"help") == 0)   // help
      help();
    else if(strcmp(cmds[0],"clear") == 0){ // clear
      printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); // 25 +
      printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); // 25 +
      printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); // 25 = 75 lines
    }
    else if(strcmp(cmds[0],"echo") == 0){  // this probably could be its own function
      int i;
      for(i = 1; i < cmdcount; i++)
        printf("%s ", cmds[i]);
      printf("\n");
    }
    else if(strcmp(cmds[0],"environ") == 0){ // print list of environment variables
      char **env;
      for(env = environ; *env; env++)
        printf("%s\n", *env);
    }
    else if(strcmp(cmds[0],"euphoric") == 0) // this is sarcastic
      printf("\"In this moment, I am euphoric. Not because of any phony god's blessing. But because, I am enlightened by my intelligence.\"\n   -Neil Degrasse Tyson\n");
    else if(strcmp(cmds[0],"thanks") == 0)   // show some love for coolshell
      printf("cs~>No problem, buddy.\n");
    else if(strcmp(cmds[0],"pause") == 0){   // pause for Enter key
      printf("cs~>Press Enter key to continue...");
      getchar();
    }
    else if(strcmp(cmds[0],"exit") == 0 || strcmp(cmds[0],"quit") == 0){ // exit
      printf("\n\n");
      return 0;
    }
    else{ // external commands
      pid_t pid = fork();
      pid_t pipe_pid;

      if(pid == 0){ // child
        if(redirect[3]){ // pipe
          int my_pipe[2];
          pipe(my_pipe);
          pipe_pid = fork(); // fork off another process for the second function
          if(pipe_pid == 0){    // pipe child (receiver)
            cmds += redirect[3] + 1;
            dup2(my_pipe[0],0);   // replace stdin with pipe read end
            close(my_pipe[1]);    // close unused write end
          }else{                // pipe parent (sender)
            dup2(my_pipe[1],1);   // replace stdout with pipe write end
            close(my_pipe[0]);    // close unused read end
          }
        }
        char *allpaths = getenv("PATH"); // get list of paths from environ variable
        char *path = malloc(64);         // buffer for each individual path
        strcpy(path,strtok(allpaths,":"));
        while(path != NULL){             // try each path in the PATH variable
          strcat(path,"/");
          strcat(path,cmds[0]);
          execv(path,cmds);              // if successful, the process is replaced with the one specified
          strcpy(path,strtok(NULL,":"));
        }
        printf("cs~>%s is not a command or program I could find. Try again.\n", cmds[0]);
        if(pipe_pid == 0 || pid == 0)    // end child processes if the exec failed
          return 0;
      }else // parent
        if(!background) // wait if there's no '&'
          wait();

    } // ...end of cmd interpretation

    if(redirect[0] || redirect[3])
      dup2(stdin_copy,0);    // replace stdin if input was redirected
    if(redirect[1] || redirect[2] || redirect[3])
      dup2(stdout_copy,1);   // replace stdout if output was redirected

  } // ...end of main loop

  fclose(bat);
  close(stdin_copy);
  close(stdout_copy);
  return 0;
}

char **parsecmd(char *line, int *count, int (*redirect)[]){
  line[strlen(line) - 1] = '\0';         // cut off last newline char
  char **tokenlist = calloc(10, sizeof(char*)); // supports 10 tokens
  char *nexttoken;
  nexttoken = strtok(line, " ");
  int i;
  for(i = 0; nexttoken != NULL; i++){
    *(tokenlist+i) = malloc(32); // buffer for each token
    strcpy(*(tokenlist+i), nexttoken);
    if(strcmp(nexttoken,"<") == 0)       // check for input redirect
      (*redirect)[0] = i;                  // set to index of token
    else if(strcmp(nexttoken,">") == 0)  // check for output redirect
      (*redirect)[1] = i;
    else if(strcmp(nexttoken,">>") == 0) // check for appended output
      (*redirect)[2] = i;
    else if(strcmp(nexttoken,"|") == 0)  // check for pipe
      (*redirect)[3] = i;
    nexttoken = strtok(NULL, " ");
  }
  *count = i;  // count of number of commands
  return tokenlist;
}

void changedir(char* path){
  if(!path)   // if supplied with no arg
    printf("No directory given. Usage: cd <directory>.\n");
  char *fullpath = malloc(CWD_BUF_LEN); // buffer for CWD
  getcwd(fullpath, CWD_BUF_LEN);
  strcat(fullpath, "/");
  strcat(fullpath, path);               // concat entered directory
  if (chdir(fullpath) == -1)
    printf("%s is not a directory. Try again or type \"help\" for help.\n", path);
}

void help(){
  printf("  ----- coolshell v1.0 -----\n");
  printf("  Invoke from command line with a file for batch mode. (Usage: coolshell <file>)\n");
  printf("  Supports UNIX commands. Always put at least one space between tokens.\n");
  printf("cd       -  change directory\n");
  printf("clear    -  \"clear\" the screen\n");
  printf("help     -  print this dialog\n");
  printf("echo     -  print your message\n");
  printf("environ  -  print a list of environment variables\n");
  printf("pause    -  suspend prompt until Enter key is pressed\n");
  printf("euphoric -  print a sarcastic motivational quotation\n");
  printf("thanks   -  be grateful for coolshell\n");
  printf("exit     -  quit coolshell\n");
  printf("quit     -  quit coolshell\n\n");
  printf("  I/O redirection supported:\n");
  printf("<   -  read input for function from file\n");
  printf(">   -  write output of function to file\n");
  printf(">>  -  append output of function to file\n");
  printf("|   -  pipe output of one function as input for another function\n");
}
