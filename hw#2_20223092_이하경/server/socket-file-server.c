#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {

    int serv_sock, clnt_sock;
    FILE *fp;
    char buffer[BUF_SIZE];
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    int read_len;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind() error");
        exit(1);
    }

    if (listen(serv_sock, 1) == -1) {
        perror("listen() error");
        exit(1);
    }

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
        perror("accept() error");
        exit(1);
    }

    int filename_len = 0;
    read(clnt_sock, &filename_len, sizeof(int));
    char filename[256];
    read(clnt_sock, filename, filename_len);
    filename[filename_len] = '\0';

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("File open error");
        close(clnt_sock);
        close(serv_sock);
        exit(1);
    }

    while ((read_len = read(clnt_sock, buffer, BUF_SIZE)) > 0) {
        fwrite(buffer, 1, read_len, fp);
    }

    fclose(fp);
    close(clnt_sock);
    close(serv_sock);
    printf("File '%s' received and saved successfully.\n", filename);

    return 0;
}

