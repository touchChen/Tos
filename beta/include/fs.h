#ifndef	_TOS_FS_H_
#define	_TOS_FS_H_

#define	INVALID_INODE		0
#define	ROOT_INODE		1

#define	NR_DEFAULT_FILE_SECTS	32 /* 32 * 512 = 16k */

struct dev_drv_map {
	int driver_nr; 
};

#define	MAGIC_V1	0x98

struct super_block {
	u32	magic;		  		/**< Magic number */
	u32	nr_inodes;	  		/**< How many inodes */
	u32	nr_sects;	  		/**< How many sectors */
	u32	nr_imap_sects;	    /**< How many inode-map sectors */
	u32	nr_smap_sects;	    /**< How many sector-map sectors */
	u32	n_1st_sect;	        /**< Number of the 1st data sector */
	u32	nr_inode_sects;     /**< How many inode sectors */
	u32	root_inode;         /**< Inode nr of root directory */
	u32	inode_size;         /**< INODE_SIZE */
	u32	inode_isize_off;    /**< Offset of `struct inode::i_size' */
	u32	inode_start_off;    /**< Offset of `struct inode::i_start_sect' */
	u32	dir_ent_size;       /**< DIR_ENTRY_SIZE */
	u32	dir_ent_inode_off;  /**< Offset of `struct dir_entry::inode_nr' */
	u32	dir_ent_fname_off;  /**< Offset of `struct dir_entry::name' */

	/*
	 * the following item(s) are only present in memory
	 */
	int	sb_dev; 	/**< the super block's home device */
};

/**
 * Note that this is the size of the struct in the device,  NOT in memory.
 * The size in memory is larger because of some more members.
 */
#define	SUPER_BLOCK_SIZE	56


struct inode {
	u32	i_mode;		/**< Accsess mode */
	u32	i_size;		/**< File size */
	u32	i_start_sect;	/**< The first sector of the data */
	u32	i_nr_sects;		/**< How many sectors the file occupies */
	u8	_unused[16];	/**< Stuff for alignment */

	/* the following items are only present in memory */
	int	i_dev;
	int	i_cnt;		/**< How many procs share this inode  */
	int	i_num;		/**< inode nr.  */
};

/**
 * Note that this is the size of the struct in the device,  NOT in memory.
 * The size in memory is larger because of some more members.
 */
#define	INODE_SIZE	32

#define	MAX_FILENAME_LEN	12


/**
 * @struct dir_entry
 * @brief  Directory Entry
 */
struct dir_entry {
	int		inode_nr;		/**< inode nr. */
	char	name[MAX_FILENAME_LEN];	/**< Filename */
};


#define	DIR_ENTRY_SIZE	sizeof(struct dir_entry)


struct file_desc {
	int		fd_mode;	/**< R or W */
	int		fd_pos;		/**< Current position for R/W. */
	struct inode*	fd_inode;	/**< Ptr to the i-node */
};


/**
 * Since all invocations of `rw_sector()' in FS look similar (most of the
 * params are the same), we use this macro to make code more readable.
 */
#define RD_SECT(dev,sect_nr) rw_sector(DEV_READ, \
				       dev,				\
				       (sect_nr) * SECTOR_SIZE,		\
				       SECTOR_SIZE, /* read one sector */ \
				       TASK_FS,				\
				       fsbuf);
#define WR_SECT(dev,sect_nr) rw_sector(DEV_WRITE, \
				       dev,				\
				       (sect_nr) * SECTOR_SIZE,		\
				       SECTOR_SIZE, /* write one sector */ \
				       TASK_FS,				\
				       fsbuf);



#define	O_CREAT		1
#define	O_RDWR		2

#define	MAX_PATH	128


/*
 * disk log
 */

#define ENABLE_DISK_LOG
#define SET_LOG_SECT_SMAP_AT_STARTUP
#define MEMSET_LOG_SECTS
#define	NR_SECTS_FOR_LOG	NR_DEFAULT_FILE_SECTS * 8


#ifdef ENABLE_DISK_LOG
#define SYSLOG syslog
#endif


#define MK_FS  1
#define UN_MK_FS 0


#endif /* _TOS_FS_H_ */
