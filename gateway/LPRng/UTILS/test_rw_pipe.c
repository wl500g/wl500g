#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#define HAVE_SOCKETPAIR
main()
{

	int fds[2], i;
	char line[4];
	int pid;
	char *m1 = "to";
	char *m2 = "fro";
#ifdef HAVE_SOCKETPAIR
    i = socketpair( AF_UNIX, SOCK_STREAM, 0, fds);
#else
    i = pipe( fds );
#endif

	if( i < 0 ) exit(1);
	printf("pipe\n");
	if( (pid = fork()) < 0) exit(1);
	if( pid == 0 ){
		/* Child */
		printf("child\n");
		if( write( fds[0], m1, strlen(m1) ) < 0 ) exit(1);
		printf("child write\n");
		i = read( fds[0], line, sizeof(line) );
		if( i < 0 ) exit(1);
		line[i] = 0;
		if( strcmp( line, m2 ) ) exit(1);
		printf("child read OK\n");
		fflush(stdout);
		exit(0);
	} else {
		printf("mother\n");
		i = read( fds[1], line, sizeof(line) );
		if( i < 0 ) exit(1);
		line[i] = 0;
		if( strcmp( line, m1 ) ) exit(1);
		printf("mother read\n");
		if( write( fds[1], m2, strlen(m2) ) < 0 ) exit(1);
		printf("mother write OK\n");
		wait(&i);
		printf("child exit %d\n", i);
	}
	exit(0);
}
