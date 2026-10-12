#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void calculate_sum(char symbol, int *sum, int *number, int *sign) {
    if (symbol >= '0' && symbol <= '9') {
        *number = *number * 10 + (symbol - '0');
    }

    else if (symbol == '-') {
        *sign = -1; 
    }

    else if (symbol == ' ') {
        *sum = *sum + (*number * *sign);
        *number = 0;
        *sign = 1; 
    }
}

int int_to_string(int sum, char *result) {
    int length = 0;
    int is_negative = 0;

    if (sum < 0) {
        is_negative = 1;
        sum = -sum;
    }

    if (sum == 0) {
        result[0] = '0';
        length = 1;
    }

    while (sum > 0) {
        int digit = sum % 10;
        result[length] = digit + '0';
        length++;
        sum = sum / 10;
    }
    for (int i = 0; i < length / 2; i++) {
        char temp = result[i];
        result[i] = result[length - i - 1];
        result[length - i - 1] = temp;
    }

    if (is_negative) {
        for (int i = length; i > 0; i--) {
            result[i] = result[i - 1];
        }

        result[0] = '-';
        length++;
    }

    result[length] = '\n';
    length++;

    return length;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }
    int file_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0) {
        return 1;
    }

    char buffer[100];

    int sum = 0;
    int number = 0;
    int sign = 1;

    while (1) {
        ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (bytes_read == 0) {
            break;
        }

        if (bytes_read < 0) {
            close(file_fd);
            return 1;
        }

        for (ssize_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                sum = sum + (number * sign);
                number = 0;
                sign = 1;

                char result[20];
                int result_length = int_to_string(sum, result);

                if (write(file_fd, result, result_length) == -1) {
                    close(file_fd);
                    return 1;
                }
                sum = 0;
            }
            else {
                calculate_sum(buffer[i], &sum, &number, &sign);
            }
        }
    }

    if (close(file_fd) == -1) {
        return 1; 
    }

    return 0;
}