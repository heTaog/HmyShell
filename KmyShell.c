#include "KmyShell.h"

#define KSHELL_RD_BUFSIZE 1024
#define KSHELL_TOK_DELIM  " \t\r\n\a"

void sys_err(const char* ptr)
{
    fprintf(stderr, "%s\n", ptr);
    exit(EXIT_FAILURE);
}

void Ksh_loop()
{
    char*   line;       // user 输入的命令
    char**  args;       // parse(解析) 命令
    int     status;     // 执行命令后，返还的执行状态码

    do {
        printf("Kshell> ");
        
        line = Ksh_read_line();
        args = Ksh_spilt_line(line);
        status = Ksh_execv(args);

        free(line);
        free(args);
    } while (1 == status);
}

char* Ksh_read_line()
{
#ifdef KSHELL_USE_STD_GETLINE
    char* buffer = NULL;
    ssize_t bufsize = 0;

    if (getline(&buffer, &bufsize, stdin) == -1) {
        if (feof(stdin)) exit(EXIT_SUCCESS);
        else sys_err("Kshell: getline error");
    }

    return buffer;
#else
    int bufsize = KSHELL_RD_BUFSIZE;
    int position = 0;
    char* buffer = (char*) malloc(sizeof(bufsize) * sizeof(char));

    if (!buffer)
        sys_err("error malloc(): memory allocate failed.");

    for (int c ; ; ) {
        c = getchar();

        if (EOF == c) exit(EXIT_FAILURE);
        if (c == '\n') {
            buffer[bufsize] = '\0';
            return buffer;
        }
        else buffer[position++] = c;

        // 当 user 输入的字符数量，超过字符容器上限时，自动扩容 2 倍
        if (position >= bufsize) {
            bufsize += KSHELL_RD_BUFSIZE;
            
            // auto tmp = buffer;
            buffer = realloc(buffer, bufsize * sizeof(char));
            if (!buffer) {
                // free(tmp);
                sys_err("realloc error: memory allocate failed.");
            }
        }
    }
#endif
}

char** Ksh_spilt_line(char* line)
{
    int bufsize = KSHELL_RD_BUFSIZE, position = 0;
    char** tokens = (char**) malloc(sizeof(char*) * bufsize);
    char*  token = NULL, *token_backup = NULL;
    if (!tokens)
        sys_err("malloc error: memory allocate failed.");

    for ( ; ; ) {
        token = strtok_r(line, KSHELL_TOK_DELIM, &token_backup);
        if (NULL == token) break;

        tokens[position++] = token;

        // 自动扩容 2 被
        if (bufsize <= position) {
            bufsize += KSHELL_RD_BUFSIZE;
            // auto tmp = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (NULL == tokens) {
                // free(tmp);      // 这一步的 free() 个人感觉有点多画龙点睛，是否 free 并无影响
                sys_err("malloc error: memory allocate failed.");
            }
        }
        line = NULL;
    }

    // 构建命令行参数，用于 execvp(3) ，NULL 结尾是为了表示参数到哪里结束
    tokens[position] = NULL;
    return tokens;
}

int Ksh_execv(char** args)
{
    if (0 == strncmp("quit", args[0], 4) ||
        0 == strncmp("exit", args[0], 4))
        return Ksh_quit();
    if (0 == strncmp("help", args[0], 4))
        return Ksh_help();

    return Ksh_launch(args);
}

int Ksh_launch(char** args)
{
    int status;
    pid_t pid = fork();

    if (-1 == pid) {
        sys_err("fork() error");
    } else if (0 == pid) {
        execvp(args[0], args);
        sys_err("execlp() error");
    } else if (0 < pid) {
        do {
            waitpid(pid, &status, WUNTRACED);   // WUNTRACED -> 开启允许程序暂定模式
        } while (!WIFEXITED(status) && ~WIFSIGNALED(status));
    }

    return OK;
}

int Ksh_quit()
{
    printf("Byebye, mio~\n");
    return QUIT;
}

int Ksh_help()
{
    printf("mio~ \ni'm help :).\ni support two Kshell operation:\n1> help\n2> quit or exit\nBye-bye.\n");
    
    return OK;
}