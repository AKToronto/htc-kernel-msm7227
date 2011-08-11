/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
<<<<<<< HEAD
 * Copyright (C) 2002-2010 Aleph One Ltd.
=======
 * Copyright (C) 2002-2011 Aleph One Ltd.
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "yportenv.h"

#include "yaffs_mtdif.h"

#include "linux/mtd/mtd.h"
#include "linux/types.h"
#include "linux/time.h"
#include "linux/mtd/nand.h"

#include "yaffs_linux.h"

<<<<<<< HEAD
int nandmtd_EraseBlockInNAND(yaffs_Device *dev, int blockNumber)
{
	struct mtd_info *mtd = yaffs_DeviceToMtd(dev);
	__u32 addr =
	    ((loff_t) blockNumber) * dev->param.totalBytesPerChunk
		* dev->param.nChunksPerBlock;
=======
int nandmtd_erase_block(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	u32 addr =
	    ((loff_t) block_no) * dev->param.total_bytes_per_chunk *
	    dev->param.chunks_per_block;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	struct erase_info ei;
	
	int retval = 0;

	ei.mtd = mtd;
	ei.addr = addr;
<<<<<<< HEAD
	ei.len = dev->param.totalBytesPerChunk * dev->param.nChunksPerBlock;
=======
	ei.len = dev->param.total_bytes_per_chunk * dev->param.chunks_per_block;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	ei.time = 1000;
	ei.retries = 2;
	ei.callback = NULL;
	ei.priv = (u_long) dev;

	retval = mtd->erase(mtd, &ei);

	if (retval == 0)
		return YAFFS_OK;

	return YAFFS_FAIL;
}

int nandmtd_initialise(struct yaffs_dev *dev)
{
	return YAFFS_OK;
}
