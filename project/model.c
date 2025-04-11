#include "model.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

ShmBuf *buf_init() {
    int fd = shm_open(SHARED_FILE_PATH, O_CREAT | O_RDWR, 0600);
    if (fd < 0) {
        perror("shm_open");
        return NULL;
    }

    ftruncate(fd, BUF_SIZE);
    ShmBuf *shmp = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    shmp->fd = fd;
    sem_init(&shmp->sem, 1, 1);
    shmp->cnt = 0;
    return shmp;
}

void model_send_message(ShmBuf *shmp, const char *msg) {
    sem_wait(&shmp->sem);
    size_t len = strlen(msg);
    if (shmp->cnt + len < BUF_SIZE - sizeof(ShmBuf)) {
        memcpy(&shmp->msgbuf[shmp->cnt], msg, len);
        shmp->cnt += len;
        shmp->msgbuf[shmp->cnt++] = '\n';
    }
    sem_post(&shmp->sem);
}

char *model_read_messages(ShmBuf *shmp) {
    sem_wait(&shmp->sem);
    char *buf = malloc(shmp->cnt + 1);
    memcpy(buf, shmp->msgbuf, shmp->cnt);
    buf[shmp->cnt] = '\0';
    sem_post(&shmp->sem);
    return buf;
}

char *execute_command(const char *cmd) {
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();

    if (pid == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        char *args[64];
        char *token = strtok(strdup(cmd), " ");
        int i = 0;
        while (token && i < 63) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else {
        close(pipefd[1]);
        char *output = malloc(4096);
        read(pipefd[0], output, 4095);
        output[4095] = '\0';
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
        return output;
    }
}
