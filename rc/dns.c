#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "dns.h"

#define DNS_PORT 53
#define SVR_PORT 4567

static int 
waitsock(int sockfd, int sec, int usec)
{
	struct timeval tv;
	fd_set 	fdvar;
	int		res;
	
	FD_ZERO(&fdvar);
	FD_SET(sockfd, &fdvar);
	
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	
	res = select(sockfd + 1, &fdvar, NULL, NULL, &tv);

	return res;
}


void dns_query(char *svrip, char *hostname, char qtype, char *ret)
{
    int                 sockfd;
    struct sockaddr_in  svr, cli, from;
    char dnsbuf[MAX_PACKET_SIZE];
    int dnsbuflen;
    dns_request_t m;
    int i;
    int retryCount;
    
    if((sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0 )
    {
        //perror("can't open datagram socket\n");
        return;
    }
      
    bzero((char *)&svr, sizeof(svr));
    svr.sin_family        = AF_INET;
    svr.sin_addr.s_addr   = htonl(INADDR_ANY);
    svr.sin_port          = htons(SVR_PORT);

    if(bind(sockfd, (struct sockaddr *)&svr, sizeof(svr))<0)
    {
		//perror("can't bind datagram socket\n");
	close(sockfd);
	return;
    }

    bzero((char *)&cli, sizeof(cli));

    cli.sin_family = AF_INET;   
    cli.sin_addr.s_addr = inet_addr(svrip);
    cli.sin_port = htons(DNS_PORT);

    dnsbuflen = 0;

    dnsbuf[dnsbuflen++] = 0x00;	//Transaction ID
    dnsbuf[dnsbuflen++] = 0x86; 
    dnsbuf[dnsbuflen++] = 0x01; //Flag: Standard query
    dnsbuf[dnsbuflen++] = 0x00;
    dnsbuf[dnsbuflen++] = 0x00; //Question
    dnsbuf[dnsbuflen++] = 0x01; 
    memset(&dnsbuf[dnsbuflen], 0x00, 6);//Answer RRs, Authority RRs, Additional RRs  
    dnsbuflen+=6;
    dnsbuflen+=dns_construct_name(hostname, &dnsbuf[dnsbuflen]);    
    if (qtype != 'M')  
    {
      dnsbuf[dnsbuflen++] = 0x00; // Type a: host address
      dnsbuf[dnsbuflen++] = 0x01; 
    }  
    else
    {
      dnsbuf[dnsbuflen++] = 0x00; // Type MX: Mail Exchanger
      dnsbuf[dnsbuflen++] = 0x0f; 	
    }  
    dnsbuf[dnsbuflen++] = 0x00; // Class : inet
    dnsbuf[dnsbuflen++] = 0x01;

    retryCount = 0;
    
retry:	    
    retryCount++;	

    if (retryCount==4) goto finish;
    	
    if(sendto(sockfd , dnsbuf, dnsbuflen , 0 ,(struct sockaddr *) &cli, sizeof(cli)) == -1)
    {
       //perror("sendto\n");
       goto retry;
    }

    if (waitsock(sockfd, 3, 0) == 0)
    {
	//perror("timeout\n");
 	goto retry;
    }

    if ((m.numread=recvfrom(sockfd, m.original_buf, sizeof(m.original_buf), 0, NULL, sizeof(from))) == -1)
    {
	//perror("recvfrom\n");
	goto retry;
    }

    if (m.numread<sizeof(m.message.header))
    {
	//perror("invalid packet\n");
	goto retry;
    }

    dns_decode_request(&m);

    if (m.message.header.flags.f.rcode==0)
    {
    	if (qtype != 'M')  
	   sprintf(ret, "%s", m.ip);
	else   
	   sprintf(ret, "%s", m.cname);
#ifdef REMOVE_WL600	   
	printf("Recv: \n");
	for(i=0; i<m.numread; i++)
	{
		printf("%02x ", (unsigned char )m.original_buf[i]);
		if (i%16==15) printf("\n");
	}
#endif	
    }
    else goto retry;
    
finish:
    close(sockfd);
    return;
}

int dns_construct_name(char *name, char *encoded_name)
{
  int i,j,k,n;

  k = 0; /* k is the index to temp */
  i = 0; /* i is the index to name */
  while( name[i] ){

	 /* find the dist to the next '.' or the end of the string and add it*/
	 for( j = 0; name[i+j] && name[i+j] != '.'; j++);
	 encoded_name[k++] = j;

	 /* now copy the text till the next dot */
	 for( n = 0; n < j; n++)
		encoded_name[k++] = name[i+n];
	
	 /* now move to the next dot */ 
	 i += j + 1;

	 /* check to see if last dot was not the end of the string */
	 if(!name[i-1])break;
  }
  encoded_name[k++] = 0;
  return k;
}

/* CVS edit 1 */
