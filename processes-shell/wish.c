#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXPATHS 100
#define MAXARGS 10
#define MAX_PATH_BUFFER 100
char *PATHS[MAXPATHS];
int PATH_NUM = 0;

typedef struct {
  char *buffer;
  char *full_path_cmd;
  char *cmd;
  char **args;
  size_t args_length;
  size_t buffer_length;
} CommandBuffer;

CommandBuffer *new_cmd_buffer() {
  CommandBuffer *cmd_buffer = malloc(sizeof(CommandBuffer));
  cmd_buffer->full_path_cmd = NULL;
  cmd_buffer->buffer = NULL;
  cmd_buffer->args = malloc(sizeof(char *) * MAXARGS);
  cmd_buffer->args_length = 0;
  cmd_buffer->buffer_length = 0;
  return cmd_buffer;
}

void close_cmd_buffer(CommandBuffer *cmd_buffer) {
  free(cmd_buffer->buffer);
  for (int i = 0; i < cmd_buffer->args_length; i++) {
    free(cmd_buffer->args[i]);
  }
  free(cmd_buffer->args);
  free(cmd_buffer->full_path_cmd);
  free(cmd_buffer);
}

void free_paths() {
  for (int i = 0; i < PATH_NUM - 1; i++) {
    free(PATHS[i]);
  }
}

void get_env_paths() {
  char *path;
  path = strdup(getenv("PATH"));
  char *rest = NULL;
  char *token;

  int i = 0;
  for (token = strtok_r(path, ":", &rest); token != NULL;
       token = strtok_r(NULL, ":", &rest)) {
    PATHS[i] = strdup(token);
    i++;
  }
  PATHS[i++] = "./";
  PATH_NUM = i;
}

void print_env_paths() {
  printf("======== ENV PATHS =========\n");
  for (int i = 0; i < PATH_NUM; i++) {
    printf(" -%s\n", PATHS[i]);
  }
}

void print_prompt() { printf("wish> "); }

int read_input(CommandBuffer *cmd_buffer) {
  ssize_t bytes_read =
      getline(&(cmd_buffer->buffer), &(cmd_buffer->buffer_length), stdin);

  if (bytes_read <= 1) {
    return 0;
  }
  cmd_buffer->buffer[bytes_read - 1] = 0;
  return 1;
}

void trim_cmd_buffer(CommandBuffer *cmd_buffer) {
  char *p = cmd_buffer->buffer;
  int l = strlen(p);

  while (isspace(p[l - 1]))
    p[--l] = 0;
  while (*p && isspace(*p))
    ++p, --l;

  memmove(cmd_buffer->buffer, p, l + 1);
}

void cd(CommandBuffer *cmd_buffer) {
  if (cmd_buffer->args_length < 2) {
    printf("invalid dir\n");
    return;
  }
  if (chdir(cmd_buffer->args[1]) == -1) {
    printf("invalid dir\n");
    return;
  }
}

int check_builtin_cmd(CommandBuffer *cmd_buffer) {
  if (strcmp(cmd_buffer->cmd, "exit") == 0) {
    free_paths();
    close_cmd_buffer(cmd_buffer);
    exit(0);
    return 1;
  } else if (strcmp(cmd_buffer->cmd, "cd") == 0) {
    cd(cmd_buffer);
    return 1;
  } else if (strcmp(cmd_buffer->cmd, "paths") == 0) {
    print_env_paths();
    return 1;
  }
  return 0;
}

void debug_cmd(CommandBuffer *cmd_buffer) {
  printf("Command: %s \n", cmd_buffer->cmd);
  printf("Args:\n");
  for (int i = 0; i < MAXARGS && cmd_buffer->args[i] != NULL; i++) {
    printf("\t %s\n", cmd_buffer->args[i]);
  }
}

void parse_cmd(CommandBuffer *cmd_buffer) {
  int i = 0;
  char *token = strtok(cmd_buffer->buffer, " ");
  cmd_buffer->cmd = token;
  while (token != NULL && i < MAXARGS) {
    cmd_buffer->args[i] = strdup(token);
    token = strtok(NULL, " ");
    i++;
  }
  cmd_buffer->args[i] = NULL;
  cmd_buffer->args_length = i;
}

void check_cmd_path(CommandBuffer *cmd_buffer) {
  for (int i = 0; PATHS[i] != NULL; i++) {
    char full_cmd_path[MAX_PATH_BUFFER];
    char *path = PATHS[i];
    sprintf(full_cmd_path, "%s/%s", path, cmd_buffer->cmd);
    if (access(full_cmd_path, F_OK) == 0) {
      cmd_buffer->full_path_cmd = strdup(full_cmd_path);
    }
  }
}

void prepare_cmd(CommandBuffer *cmd_buffer) {
  trim_cmd_buffer(cmd_buffer);
  parse_cmd(cmd_buffer);
  check_cmd_path(cmd_buffer);
}

int valid_cmd(CommandBuffer *cmd_buffer) {
  if (cmd_buffer->full_path_cmd == NULL) {
    printf("command %s not found \n", cmd_buffer->cmd);
    return 0;
  }
  return 1;
}

void execute_cmd(CommandBuffer *cmd_buffer) {
  int rc = fork();
  if (rc == 0) {
    execv(cmd_buffer->full_path_cmd, cmd_buffer->args);
    printf("something went wrong\n");
  } else {
    rc = (int)wait(NULL);
  }
}

int main() {
  get_env_paths();
  while (1) {
    CommandBuffer *cmd_buffer = new_cmd_buffer();
    print_prompt();
    if (!read_input(cmd_buffer))
      continue;
    prepare_cmd(cmd_buffer);
    if (!check_builtin_cmd(cmd_buffer)) {
      if (!valid_cmd(cmd_buffer))
        continue;
      execute_cmd(cmd_buffer);
    };
    close_cmd_buffer(cmd_buffer);
  }
}
