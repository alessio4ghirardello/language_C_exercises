#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Numero argomenti sbagliato");
        exit(1);
    }

    int p1p2[2], p2p3[2], p3p0[2], pid;
    char buffer[1], *ptr, strimporto[100];
    double totale = 0;

    pipe(p1p2);
    pid = fork();
    if (pid == 0)
    {
        close(p1p2[0]);
        close(1);
        dup(p1p2[1]);
        close(p1p2[1]);
        execl("/usr/sbin/traceroute", "traceroute", argv[1], NULL);
        return -1;
    }

    pipe(p2p3);
    pid = fork();
    if (pid == 0)
    {
        close(p1p2[1]);
        close(0);
        dup(p1p2[0]);
        close(p1p2[0]);

        close(p2p3[0]);
        close(1);
        dup(p2p3[1]);
        close(p2p3[1]);

        execl("/usr/bin/awk", "awk", "{print $4}", (char *)0);
        return -1;
    }

    close(p1p2[1]);
    close(p1p2[0]);

    pipe(p3p0);
    pid = fork();
    if (pid == 0)
    {

        close(0);
        dup(p2p3[0]);
        close(p2p3[0]);
        close(p2p3[1]);

        close(1);
        dup(p3p0[1]);
        close(p3p0[0]);
        close(p3p0[1]);

        execl("/usr/bin/tail", "tail", "-n", "+2", (char *)0);
        return -1;
    }

    close(p2p3[0]);
    close(p2p3[1]);
    close(p3p0[1]);

    while (read(p3p0[0], buffer, 1) > 0)
    {
        strncat(strimporto, &buffer[0], sizeof(buffer[0]));
        if (buffer[0] == '\n')
        {
            printf("Tempo parziale: %s", strimporto);
            totale = totale + strtod(strimporto, &ptr);
            strimporto[0] = '\0';
        }
    }

    close(p3p0[0]);
    printf("\nIl tempo totale impiegato è: %.2lf\n", totale);
    return 0;
}