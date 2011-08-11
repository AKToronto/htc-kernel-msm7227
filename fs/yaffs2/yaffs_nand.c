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

#include "yaffs_nand.h"
#include "yaffs_tagscompat.h"

#include "yaffs_getblockinfo.h"

int yaffs_rd_chunk_tags_nand(struct yaffs_dev *dev, int nand_chunk,
			     u8 *buffer, struct yaffs_ext_tags *tags)
{
	int result;
	struct yaffs_ext_tags local_tags;
	int realigned_chunk = nand_chunk - dev->chunk_offset;

	dev->n_page_reads++;

	/* If there are no tags provided use local tags. */
	if (!tags)
		tags = &local_tags;

<<<<<<< HEAD
	if (dev->param.readChunkWithTagsFromNAND)
		result = dev->param.readChunkWithTagsFromNAND(dev, realignedChunkInNAND, buffer,
						      tags);
	else
		result = yaffs_TagsCompatabilityReadChunkWithTagsFromNAND(dev,
									realignedChunkInNAND,
									buffer,
									tags);
	if (tags &&
	   tags->eccResult > YAFFS_ECC_RESULT_NO_ERROR) {

		yaffs_BlockInfo *bi;
		bi = yaffs_GetBlockInfo(dev, chunkInNAND/dev->param.nChunksPerBlock);
		yaffs_HandleChunkError(dev, bi);
=======
	if (dev->param.read_chunk_tags_fn)
		result =
		    dev->param.read_chunk_tags_fn(dev, realigned_chunk, buffer,
						  tags);
	else
		result = yaffs_tags_compat_rd(dev,
					      realigned_chunk, buffer, tags);
	if (tags && tags->ecc_result > YAFFS_ECC_RESULT_NO_ERROR) {

		struct yaffs_block_info *bi;
		bi = yaffs_get_block_info(dev,
					  nand_chunk /
					  dev->param.chunks_per_block);
		yaffs_handle_chunk_error(dev, bi);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	}
	return result;
}

int yaffs_wr_chunk_tags_nand(struct yaffs_dev *dev,
				int nand_chunk,
				const u8 *buffer, struct yaffs_ext_tags *tags)
{
	dev->n_page_writes++;
	nand_chunk -= dev->chunk_offset;

	if (tags) {
		tags->seq_number = dev->seq_number;
		tags->chunk_used = 1;
		yaffs_trace(YAFFS_TRACE_WRITE,
			"Writing chunk %d tags %d %d",
			nand_chunk, tags->obj_id, tags->chunk_id);
	} else {
		yaffs_trace(YAFFS_TRACE_ERROR, "Writing with no tags");
		BUG();
		return YAFFS_FAIL;
	}

<<<<<<< HEAD
	if (dev->param.writeChunkWithTagsToNAND)
		return dev->param.writeChunkWithTagsToNAND(dev, chunkInNAND, buffer,
						     tags);
	else
		return yaffs_TagsCompatabilityWriteChunkWithTagsToNAND(dev,
								       chunkInNAND,
								       buffer,
								       tags);
=======
	if (dev->param.write_chunk_tags_fn)
		return dev->param.write_chunk_tags_fn(dev, nand_chunk, buffer,
							tags);

	return yaffs_tags_compat_wr(dev, nand_chunk, buffer, tags);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
}

int yaffs_mark_bad(struct yaffs_dev *dev, int block_no)
{
	block_no -= dev->block_offset;
	if (dev->param.bad_block_fn)
		return dev->param.bad_block_fn(dev, block_no);

<<<<<<< HEAD
	if (dev->param.markNANDBlockBad)
		return dev->param.markNANDBlockBad(dev, blockNo);
	else
		return yaffs_TagsCompatabilityMarkNANDBlockBad(dev, blockNo);
=======
	return yaffs_tags_compat_mark_bad(dev, block_no);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
}

int yaffs_query_init_block_state(struct yaffs_dev *dev,
				 int block_no,
				 enum yaffs_block_state *state,
				 u32 *seq_number)
{
	block_no -= dev->block_offset;
	if (dev->param.query_block_fn)
		return dev->param.query_block_fn(dev, block_no, state,
						 seq_number);

<<<<<<< HEAD
	if (dev->param.queryNANDBlock)
		return dev->param.queryNANDBlock(dev, blockNo, state, sequenceNumber);
	else
		return yaffs_TagsCompatabilityQueryNANDBlock(dev, blockNo,
							     state,
							     sequenceNumber);
=======
	return yaffs_tags_compat_query_block(dev, block_no, state, seq_number);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
}

int yaffs_erase_block(struct yaffs_dev *dev, int flash_block)
{
	int result;

<<<<<<< HEAD
	blockInNAND -= dev->blockOffset;

	dev->nBlockErasures++;

	result = dev->param.eraseBlockInNAND(dev, blockInNAND);

=======
	flash_block -= dev->block_offset;
	dev->n_erasures++;
	result = dev->param.erase_fn(dev, flash_block);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	return result;
}

int yaffs_init_nand(struct yaffs_dev *dev)
{
<<<<<<< HEAD
	if(dev->param.initialiseNAND)
		return dev->param.initialiseNAND(dev);
=======
	if (dev->param.initialise_flash_fn)
		return dev->param.initialise_flash_fn(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	return YAFFS_OK;
}
