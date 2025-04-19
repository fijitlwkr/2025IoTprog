#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {

    int sock;
    struct sockaddr_in serv_addr;
    FILE *fp;
    char buffer[BUF_SIZE];
    int read_len;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP> <Port> <Filename>\n", argv[0]);
        exit(1);
    }

    fp = fopen(argv[3], "rb");
    if (fp == NULL) {
        perror("File open error");
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        fclose(fp);
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect() error");
        fclose(fp);
        close(sock);
        exit(1);
    }

    int filename_len = strlen(argv[3]);
    write(sock, &filename_len, sizeof(int));
    write(sock, argv[3], filename_len);

    while ((read_len = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        write(sock, buffer, read_len);
    }

    fclose(fp);
    close(sock);
    printf("File '%s' sent successfully.\n", argv[3]);

    return 0;
}

