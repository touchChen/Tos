#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "hd.h"
#include "fs.h"
#include "proc.h"
#include "keyboard.h"
#include "global.h"
#include "proto.h"


/*****************************************************************************
 * Perform syslog() system call .
 *
 * @return 
 *****************************************************************************/
PUBLIC int do_disklog()
{
	char buf[STR_DEFAULT_LEN];

	/* get parameters from the message */
	int str_len = fs_msg.CNT;	/* length of filename */
	int src = fs_msg.source;	/* caller proc nr. */
	assert(str_len < STR_DEFAULT_LEN);
	phys_copy((void*)va2la(TASK_FS, buf),    /* to   */
		  (void*)va2la(src, fs_msg.BUF), /* from */
		  str_len);
	buf[str_len] = 0;	/* terminate the string */

	return disklog(buf);
}

/*****************************************************************************
 * <Ring 1> This routine handles the DEV_LOG message.
 * 
 * @param p  Ptr to the MESSAGE.
 *****************************************************************************/
PUBLIC int disklog(char * logstr)
{
	int device = root_inode->i_dev;
	struct super_block * sb = get_super_block(device);
	int nr_log_blk0_nr = sb->nr_sects - NR_SECTS_FOR_LOG;  // 除去 log 

	static int pos = 0;
	printl("static pos:%d\n", pos);
	if (!pos) { /* first time invoking this routine */

#ifdef SET_LOG_SECT_SMAP_AT_STARTUP
		/*
		 * set sector-map so that other files cannot use the log sectors
		 */

		int bits_per_sect = SECTOR_SIZE * 8; /* 4096 */

		int smap_blk0_nr = 1 + 1 + sb->nr_imap_sects; /* 3 */
		int sect_nr  = smap_blk0_nr + nr_log_blk0_nr / bits_per_sect; 
		int byte_off = (nr_log_blk0_nr % bits_per_sect) / 8; 
		int bit_off  = (nr_log_blk0_nr % bits_per_sect) % 8; 
		int sect_cnt = NR_SECTS_FOR_LOG / bits_per_sect + 2; 
		int bits_left= NR_SECTS_FOR_LOG;

		int i;
		for (i = 0; i < sect_cnt; i++) {
			RD_SECT(device, sect_nr + i); /* RD_SECT(?, 12) */

			for (; byte_off < SECTOR_SIZE && bits_left > 0; byte_off++) {
				for (; bit_off < 8; bit_off++) { /* repeat till enough bits are set */
					assert(((fsbuf[byte_off] >> bit_off) & 1) == 0);
					fsbuf[byte_off] |= (1 << bit_off);
					if (--bits_left  == 0)
						break;
				}
				bit_off = 0;
			}
			byte_off = 0;
			bit_off = 0;

			WR_SECT(device, sect_nr + i);

			if (bits_left == 0)
				break;
		}
		assert(bits_left == 0);
#endif /* SET_LOG_SECT_SMAP_AT_STARTUP */

		pos = 0x40;

#ifdef MEMSET_LOG_SECTS
		/* write padding stuff to log sectors */
		//int chunk = min(MAX_IO_BYTES, FSBUF_SIZE >> SECTOR_SIZE_SHIFT);
		//assert(chunk == 256);
		int chunk = NR_SECTS_FOR_LOG;
		int sects_left = NR_SECTS_FOR_LOG;
		for (i = nr_log_blk0_nr;
		     i < nr_log_blk0_nr + NR_SECTS_FOR_LOG;
		     i += chunk) {
			memset(fsbuf, 0x20, chunk*SECTOR_SIZE);
			rw_sector(DEV_WRITE,
				  device,
				  i * SECTOR_SIZE,
				  chunk * SECTOR_SIZE,
				  TASK_FS,
				  fsbuf);
			sects_left -= chunk;
		}
		if (sects_left != 0)
			panic("sects_left should be 0, current: %d.", sects_left);
#endif /* MEMSET_LOG_SECTS */
	}

	char * p = logstr;
	int bytes_left = strlen(logstr);

	int sect_nr = nr_log_blk0_nr + (pos >> SECTOR_SIZE_SHIFT);

	while (bytes_left) {
		RD_SECT(device, sect_nr);

		int off = pos % SECTOR_SIZE;
		int bytes = min(bytes_left, SECTOR_SIZE - off);

		memcpy(&fsbuf[off], p, bytes);
		fsbuf[off+bytes] = '\0';
                printl("write to log: %s\n",&fsbuf[off]);
		off += bytes;
		bytes_left -= bytes;

		WR_SECT(device, sect_nr);
		sect_nr++;
		pos += bytes;
		p += bytes;
	}

	/* write `pos' into the log file header */
	RD_SECT(device, nr_log_blk0_nr);

	sprintf((char*)fsbuf, "%8d\n", pos);
	memset(fsbuf+9, ' ', 22);
	fsbuf[31] = '\n';

	memset(fsbuf+32, ' ', 31);
	fsbuf[63] = '\n';

	WR_SECT(device, nr_log_blk0_nr);
	memset(fsbuf+64, fsbuf[32+19], 512-64);
	WR_SECT(device, nr_log_blk0_nr + NR_SECTS_FOR_LOG - 1);

	return pos;
}






