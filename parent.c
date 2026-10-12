#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    char filename[100];

    ssize_t filename_length = read(STDIN_FILENO, filename, sizeof(filename) - 1);

    if (filename_length <= 0) {
        return 1;
    }

    if (filename[filename_length - 1] == '\n') {
        filename[filename_length - 1] = '\0';
    } else {
        filename[filename_length] = '\0';
    }

    int pipefd[2];

    if (pipe(pipefd) == -1){
        return 1;
    }
    pid_t pid = fork();
    if (pid < 0){
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    else if (pid == 0){
        close(pipefd[1]); 

        if (dup2(pipefd[0], STDIN_FILENO) == -1) { 
            close(pipefd[0]);
            return 1;
        }
        close(pipefd[0]);
        execl("./child", "child", filename, NULL); 
        return 1;
    }

    else{
        close(pipefd[0]);

        char buffer[100];

        while (1) {
            ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));

            if (bytes_read == 0) {
                break;
            }

            if (bytes_read < 0) {
                close(pipefd[1]);
                waitpid(pid, NULL, 0);
                return 1;
            }

            ssize_t bytes_written = write(pipefd[1], buffer, bytes_read);

            if (bytes_written != bytes_read) {
                close(pipefd[1]);
                waitpid(pid, NULL, 0);
                return 1;
            }
        }

        if (close(pipefd[1]) == -1) {
            waitpid(pid, NULL, 0);
            return 1;
        }

        if (waitpid(pid, NULL, 0) == -1) {
            return 1;
        }
    }

    return 0;
}