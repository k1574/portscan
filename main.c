#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char *argv0;
char *host;

void
usage(void)
{
	fprintf(stderr, "usage: %s <ip>\n", argv0);
	exit(1);
}


void
scan_port(unsigned short port, char *host)
{
	struct sockaddr_in saddr;
	int sock;
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 4;

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET ;

	if(inet_pton(AF_INET, host, &saddr.sin_addr) <= 0){
		fprintf(stderr, "%s: wrong IP format\n", argv0);
		exit(1);
	}
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "%s: could not create socket\n", argv0);
		return;
	}
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	saddr.sin_port = htons(port) ;
	if(connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0){
		return;
	}

	printf("%d\n", port);
}

int
main(int argc, char *argv[])
{

	int port;
	char buf[32];

	argv0 = argv[0] ;
	if(argc!=2) usage() ;
	host = argv[1] ;

	while(fgets(buf, sizeof(buf), stdin)){
		if(!(port = atoi(buf)))
			continue;
		if(!fork()){
			scan_port(port, host) ;
			goto finish_thread;
		}
	}

finish_thread:
	return 0 ;
}

