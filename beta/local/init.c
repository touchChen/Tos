#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "global.h"
#include "proto.h"



/**
 * @struct posix_tar_header
 * Borrowed from GNU `tar'
 */
struct posix_tar_header
{				/* byte offset */
	char name[100];		/*   0 */
	char mode[8];		/* 100 */
	char uid[8];		/* 108 */
	char gid[8];		/* 116 */
	char size[12];		/* 124 */
	char mtime[12];		/* 136 */
	char chksum[8];		/* 148 */
	char typeflag;		/* 156 */
	char linkname[100];	/* 157 */
	char magic[6];		/* 257 */
	char version[2];	/* 263 */
	char uname[32];		/* 265 */
	char gname[32];		/* 297 */
	char devmajor[8];	/* 329 */
	char devminor[8];	/* 337 */
	char prefix[155];	/* 345 */
	/* 500 */
};

/*****************************************************************************
 * Extract the tar file and store them.
 * 
 * @param filename The tar file.
 *****************************************************************************/
void untar(const char * filename)
{
	printf("INIT##untar>> extract:%s\n", filename);
	int fd = open(filename, O_RDWR);
	assert(fd != -1);

	char buf[SECTOR_SIZE * 16];
	int chunk = sizeof(buf);

	while (1) {
		read(fd, buf, SECTOR_SIZE); // pos 有位移
		if (buf[0] == 0)
			break;

		struct posix_tar_header * phdr = (struct posix_tar_header *)buf;
		printf("*****%s*******\n",phdr->name);

		/* calculate the file size */
		char * p = phdr->size;
		int f_len = 0;
		while (*p)
			f_len = (f_len * 8) + (*p++ - '0'); /* octal */

		int bytes_left = f_len;
		int fdout = open(phdr->name, O_CREAT | O_RDWR);
		if (fdout == -1) {
			printf("INIT##untar>> failed to extract file: %s\n", phdr->name);
			printf("INIT##untar>> aborted\n");
			return;
		}
		printf("INIT##untar>>  file: %s (%d bytes)\n", phdr->name, f_len);
		while (bytes_left) {
			int iobytes = min(chunk, bytes_left);
			read(fd, buf,
			     ((iobytes - 1) / SECTOR_SIZE + 1) * SECTOR_SIZE);
			write(fdout, buf, iobytes);
			bytes_left -= iobytes;
		}
		printf("INIT##untar>>  copy over\n");
		close(fdout);
		printf("INIT##untar>>  close\n");
	}
	graphlog();

	close(fd);
}


/*****************************************************************************
 *                                Init
 *****************************************************************************/

PUBLIC void Init()
{
	clearlog();
	
	//untar("/cmd.tar");

	int pid = fork();
	if (pid == 0) { // child process
		printf("INIT## child is running, pid:%d\n", getpid());
		
		//execl("/echo", "echo", "hello world", " tc! ", 0);
		
		//exit(99);
	}
	else {	// parent process
		printf("INIT## parent is running, child pid:%d\n", pid);

		int s;
		int child = wait(&s);
		printf("INIT## parent process::child (%d) exited with status: %d.\n", child, s);
	}


	while(1) {
		int s;
		int child = wait(&s);
		printf("INIT## parent process::child (%d) exited with status: %d.\n", child, s);
	}

	// 不会到这一步
	spin("Init...");
}
