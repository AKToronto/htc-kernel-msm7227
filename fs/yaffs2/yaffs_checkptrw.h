/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
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
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __YAFFS_CHECKPTRW_H__
#define __YAFFS_CHECKPTRW_H__

#include "yaffs_guts.h"

<<<<<<< HEAD
int yaffs2_CheckpointOpen(yaffs_Device *dev, int forWriting);

int yaffs2_CheckpointWrite(yaffs_Device *dev, const void *data, int nBytes);

int yaffs2_CheckpointRead(yaffs_Device *dev, void *data, int nBytes);

int yaffs2_GetCheckpointSum(yaffs_Device *dev, __u32 *sum);

int yaffs2_CheckpointClose(yaffs_Device *dev);

int yaffs2_CheckpointInvalidateStream(yaffs_Device *dev);
=======
int yaffs2_checkpt_open(struct yaffs_dev *dev, int writing);

int yaffs2_checkpt_wr(struct yaffs_dev *dev, const void *data, int n_bytes);

int yaffs2_checkpt_rd(struct yaffs_dev *dev, void *data, int n_bytes);

int yaffs2_get_checkpt_sum(struct yaffs_dev *dev, u32 * sum);

int yaffs_checkpt_close(struct yaffs_dev *dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

int yaffs2_checkpt_invalidate_stream(struct yaffs_dev *dev);

#endif
