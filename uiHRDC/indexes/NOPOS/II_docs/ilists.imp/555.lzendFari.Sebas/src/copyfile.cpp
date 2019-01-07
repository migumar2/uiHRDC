#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int cp_file(const char *from, const char *to)
{
    ssize_t totalbytes=0;
    int fd_to, fd_from;
    int BUFF_SIZE=(1024*64);
    char buf[BUFF_SIZE];
    ssize_t nread;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0){
	char msg[2048];
	sprintf(msg,"COPY-FILE:open-src-file failed (%s)",from);
	perror(msg);
        return -1;
    }

    unlink(to);
    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0){
	char msg[2048];
	sprintf(msg,"COPY-FILE:failed to create dst file(%s)",to);
	perror(msg);
	    if (fd_to >= 0)
		close(fd_to);
        return -1;
    }

    while ((nread = read(fd_from, buf, BUFF_SIZE)) > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

            nwritten = write(fd_to, out_ptr, nread);
	    totalbytes +=nwritten;
	    if (nwritten !=nread){
		perror("COPY-FILE:write failed");
	    if (fd_from >= 0)
		close(fd_from);
	    if (fd_to >= 0)
		close(fd_to);
		return -1;
	    }

    }


	close(fd_to);
	close(fd_from);

	/* Success! */

	return totalbytes;
}


/*
int main(int argc, char * argv[]) {
if (argc !=3) {
	printf("\n syntax incorrect: %s src-file dst-file",argv[0]);
	exit(0);
}

ssize_t totalbytes=cp_file(argv[1],argv[2]);
if (-1==totalbytes) {
	printf("\n cp-file failed: %s to %s",argv[1],argv[2]);
	exit(0);
}

printf("\n copy succeedded (%zd bytes): %s to %s",totalbytes,argv[1],argv[2]);
}
*/
