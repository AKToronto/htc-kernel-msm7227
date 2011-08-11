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
<<<<<<< HEAD
=======

>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
#include "yportenv.h"
#include "yaffs_trace.h"

#include "yaffs_guts.h"
#include "yaffs_getblockinfo.h"
#include "yaffs_tagscompat.h"
<<<<<<< HEAD

#include "yaffs_nand.h"

=======
#include "yaffs_nand.h"
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
#include "yaffs_yaffs1.h"
#include "yaffs_yaffs2.h"
#include "yaffs_bitmap.h"
#include "yaffs_verify.h"
<<<<<<< HEAD

=======
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
#include "yaffs_nand.h"
#include "yaffs_packedtags2.h"
#include "yaffs_nameval.h"
#include "yaffs_allocator.h"
#include "yaffs_attribs.h"

<<<<<<< HEAD
#include "yaffs_nameval.h"
#include "yaffs_allocator.h"

=======
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
/* Note YAFFS_GC_GOOD_ENOUGH must be <= YAFFS_GC_PASSIVE_THRESHOLD */
#define YAFFS_GC_GOOD_ENOUGH 2
#define YAFFS_GC_PASSIVE_THRESHOLD 4

#include "yaffs_ecc.h"

/* Forward declarations */

<<<<<<< HEAD

/* Robustification (if it ever comes about...) */
static void yaffs_RetireBlock(yaffs_Device *dev, int blockInNAND);
static void yaffs_HandleWriteChunkError(yaffs_Device *dev, int chunkInNAND,
		int erasedOk);
static void yaffs_HandleWriteChunkOk(yaffs_Device *dev, int chunkInNAND,
				const __u8 *data,
				const yaffs_ExtendedTags *tags);
static void yaffs_HandleUpdateChunk(yaffs_Device *dev, int chunkInNAND,
				const yaffs_ExtendedTags *tags);

/* Other local prototypes */
static void yaffs_UpdateParent(yaffs_Object *obj);
static int yaffs_UnlinkObject(yaffs_Object *obj);
static int yaffs_ObjectHasCachedWriteData(yaffs_Object *obj);

static int yaffs_WriteNewChunkWithTagsToNAND(yaffs_Device *dev,
					const __u8 *buffer,
					yaffs_ExtendedTags *tags,
					int useReserve);


static yaffs_Object *yaffs_CreateNewObject(yaffs_Device *dev, int number,
					yaffs_ObjectType type);


static int yaffs_ApplyXMod(yaffs_Object *obj, char *buffer, yaffs_XAttrMod *xmod);

static void yaffs_RemoveObjectFromDirectory(yaffs_Object *obj);
static int yaffs_CheckStructures(void);
static int yaffs_DoGenericObjectDeletion(yaffs_Object *in);

static int yaffs_CheckChunkErased(struct yaffs_DeviceStruct *dev,
				int chunkInNAND);

static int yaffs_UnlinkWorker(yaffs_Object *obj);

static int yaffs_TagsMatch(const yaffs_ExtendedTags *tags, int objectId,
			int chunkInObject);

static int yaffs_AllocateChunk(yaffs_Device *dev, int useReserve,
				yaffs_BlockInfo **blockUsedPtr);

static void yaffs_CheckObjectDetailsLoaded(yaffs_Object *in);

static void yaffs_InvalidateWholeChunkCache(yaffs_Object *in);
static void yaffs_InvalidateChunkCache(yaffs_Object *object, int chunkId);

static int yaffs_FindChunkInFile(yaffs_Object *in, int chunkInInode,
				yaffs_ExtendedTags *tags);

static int yaffs_VerifyChunkWritten(yaffs_Device *dev,
					int chunkInNAND,
					const __u8 *data,
					yaffs_ExtendedTags *tags);


static void yaffs_LoadNameFromObjectHeader(yaffs_Device *dev,YCHAR *name, const YCHAR *ohName, int bufferSize);
static void yaffs_LoadObjectHeaderFromName(yaffs_Device *dev,YCHAR *ohName, const YCHAR *name);
=======
static int yaffs_wr_data_obj(struct yaffs_obj *in, int inode_chunk,
			     const u8 *buffer, int n_bytes, int use_reserve);

>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000


/* Function to calculate chunk and offset */

static inline void yaffs_addr_to_chunk(struct yaffs_dev *dev, loff_t addr,
					int *chunk_out, u32 *offset_out)
{
	int chunk;
	u32 offset;

	chunk = (u32) (addr >> dev->chunk_shift);

	if (dev->chunk_div == 1) {
		/* easy power of 2 case */
		offset = (u32) (addr & dev->chunk_mask);
	} else {
		/* Non power-of-2 case */

		loff_t chunk_base;

		chunk /= dev->chunk_div;

		chunk_base = ((loff_t) chunk) * dev->data_bytes_per_chunk;
		offset = (u32) (addr - chunk_base);
	}

	*chunk_out = chunk;
	*offset_out = offset;
}

/* Function to return the number of shifts for a power of 2 greater than or
 * equal to the given number
 * Note we don't try to cater for all possible numbers and this does not have to
 * be hellishly efficient.
 */

static inline u32 calc_shifts_ceiling(u32 x)
{
	int extra_bits;
	int shifts;

	shifts = extra_bits = 0;

	while (x > 1) {
		if (x & 1)
			extra_bits++;
		x >>= 1;
		shifts++;
	}

	if (extra_bits)
		shifts++;

	return shifts;
}

/* Function to return the number of shifts to get a 1 in bit 0
 */

static inline u32 calc_shifts(u32 x)
{
<<<<<<< HEAD
	__u32 nShifts;
=======
	u32 shifts;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	shifts = 0;

	if (!x)
		return 0;

	while (!(x & 1)) {
		x >>= 1;
		shifts++;
	}

	return shifts;
}

/*
 * Temporary buffer manipulations.
 */

static int yaffs_init_tmp_buffers(struct yaffs_dev *dev)
{
	int i;
	u8 *buf = (u8 *) 1;

	memset(dev->temp_buffer, 0, sizeof(dev->temp_buffer));

	for (i = 0; buf && i < YAFFS_N_TEMP_BUFFERS; i++) {
<<<<<<< HEAD
		dev->tempBuffer[i].line = 0;	/* not in use */
		dev->tempBuffer[i].buffer = buf =
		    YMALLOC_DMA(dev->param.totalBytesPerChunk);
=======
		dev->temp_buffer[i].in_use = 0;
		buf = kmalloc(dev->param.total_bytes_per_chunk, GFP_NOFS);
		dev->temp_buffer[i].buffer = buf;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	}

	return buf ? YAFFS_OK : YAFFS_FAIL;
}

u8 *yaffs_get_temp_buffer(struct yaffs_dev * dev)
{
	int i;

	dev->temp_in_use++;
	if (dev->temp_in_use > dev->max_temp)
		dev->max_temp = dev->temp_in_use;

	for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++) {
		if (dev->temp_buffer[i].in_use == 0) {
			dev->temp_buffer[i].in_use = 1;
			return dev->temp_buffer[i].buffer;
		}
	}

	yaffs_trace(YAFFS_TRACE_BUFFERS, "Out of temp buffers");
	/*
	 * If we got here then we have to allocate an unmanaged one
	 * This is not good.
	 */

	dev->unmanaged_buffer_allocs++;
	return kmalloc(dev->data_bytes_per_chunk, GFP_NOFS);

}

void yaffs_release_temp_buffer(struct yaffs_dev *dev, u8 *buffer)
{
	int i;

	dev->temp_in_use--;

	for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++) {
		if (dev->temp_buffer[i].buffer == buffer) {
			dev->temp_buffer[i].in_use = 0;
			return;
		}
	}

	if (buffer) {
		/* assume it is an unmanaged one. */
		yaffs_trace(YAFFS_TRACE_BUFFERS, "Releasing unmanaged temp buffer");
		kfree(buffer);
		dev->unmanaged_buffer_deallocs++;
	}

}

/*
 * Determine if we have a managed buffer.
 */
int yaffs_is_managed_tmp_buffer(struct yaffs_dev *dev, const u8 *buffer)
{
	int i;

	for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++) {
		if (dev->temp_buffer[i].buffer == buffer)
			return 1;
	}

<<<<<<< HEAD
	for (i = 0; i < dev->param.nShortOpCaches; i++) {
		if (dev->srCache[i].data == buffer)
=======
	for (i = 0; i < dev->param.n_caches; i++) {
		if (dev->cache[i].data == buffer)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
			return 1;
	}

	if (buffer == dev->checkpt_buffer)
		return 1;

	yaffs_trace(YAFFS_TRACE_ALWAYS,
	  "yaffs: unmaged buffer detected.");
	return 0;
}

/*
<<<<<<< HEAD
 * Verification code
 */




/*
 *  Simple hash function. Needs to have a reasonable spread
 */

static Y_INLINE int yaffs_HashFunction(int n)
{
	n = abs(n);
	return n % YAFFS_NOBJECT_BUCKETS;
}

/*
 * Access functions to useful fake objects.
 * Note that root might have a presence in NAND if permissions are set.
 */

yaffs_Object *yaffs_Root(yaffs_Device *dev)
{
	return dev->rootDir;
}

yaffs_Object *yaffs_LostNFound(yaffs_Device *dev)
{
	return dev->lostNFoundDir;
}

=======
 * Functions for robustisizing TODO
 *
 */

static void yaffs_handle_chunk_wr_ok(struct yaffs_dev *dev, int nand_chunk,
				     const u8 *data,
				     const struct yaffs_ext_tags *tags)
{
	dev = dev;
	nand_chunk = nand_chunk;
	data = data;
	tags = tags;
}

static void yaffs_handle_chunk_update(struct yaffs_dev *dev, int nand_chunk,
				      const struct yaffs_ext_tags *tags)
{
	dev = dev;
	nand_chunk = nand_chunk;
	tags = tags;
}

void yaffs_handle_chunk_error(struct yaffs_dev *dev,
			      struct yaffs_block_info *bi)
{
	if (!bi->gc_prioritise) {
		bi->gc_prioritise = 1;
		dev->has_pending_prioritised_gc = 1;
		bi->chunk_error_strikes++;

		if (bi->chunk_error_strikes > 3) {
			bi->needs_retiring = 1;	/* Too many stikes, so retire */
			yaffs_trace(YAFFS_TRACE_ALWAYS,
				"yaffs: Block struck out");

		}
	}
}

static void yaffs_handle_chunk_wr_error(struct yaffs_dev *dev, int nand_chunk,
					int erased_ok)
{
	int flash_block = nand_chunk / dev->param.chunks_per_block;
	struct yaffs_block_info *bi = yaffs_get_block_info(dev, flash_block);

	yaffs_handle_chunk_error(dev, bi);

	if (erased_ok) {
		/* Was an actual write failure,
		 * so mark the block for retirement.*/
		bi->needs_retiring = 1;
		yaffs_trace(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
		  "**>> Block %d needs retiring", flash_block);
	}

	/* Delete the chunk */
	yaffs_chunk_del(dev, nand_chunk, 1, __LINE__);
	yaffs_skip_rest_of_block(dev);
}

/*
 * Verification code
 */

/*
 *  Simple hash function. Needs to have a reasonable spread
 */

static inline int yaffs_hash_fn(int n)
{
	n = abs(n);
	return n % YAFFS_NOBJECT_BUCKETS;
}

/*
 * Access functions to useful fake objects.
 * Note that root might have a presence in NAND if permissions are set.
 */

struct yaffs_obj *yaffs_root(struct yaffs_dev *dev)
{
	return dev->root_dir;
}

struct yaffs_obj *yaffs_lost_n_found(struct yaffs_dev *dev)
{
	return dev->lost_n_found;
}
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

/*
 *  Erased NAND checking functions
 */

<<<<<<< HEAD
int yaffs_CheckFF(__u8 *buffer, int nBytes)
{
	/* Horrible, slow implementation */
	while (nBytes--) {
		if (*buffer != 0xFF)
			return 0;
		buffer++;
	}
	return 1;
}

static int yaffs_CheckChunkErased(struct yaffs_DeviceStruct *dev,
				int chunkInNAND)
{
	int retval = YAFFS_OK;
	__u8 *data = yaffs_GetTempBuffer(dev, __LINE__);
	yaffs_ExtendedTags tags;
	int result;

	result = yaffs_ReadChunkWithTagsFromNAND(dev, chunkInNAND, data, &tags);

	if (tags.eccResult > YAFFS_ECC_RESULT_NO_ERROR)
		retval = YAFFS_FAIL;

	if (!yaffs_CheckFF(data, dev->nDataBytesPerChunk) || tags.chunkUsed) {
		T(YAFFS_TRACE_NANDACCESS,
		  (TSTR("Chunk %d not erased" TENDSTR), chunkInNAND));
		retval = YAFFS_FAIL;
	}

	yaffs_ReleaseTempBuffer(dev, data, __LINE__);

	return retval;

}


static int yaffs_VerifyChunkWritten(yaffs_Device *dev,
					int chunkInNAND,
					const __u8 *data,
					yaffs_ExtendedTags *tags)
{
	int retval = YAFFS_OK;
	yaffs_ExtendedTags tempTags;
	__u8 *buffer = yaffs_GetTempBuffer(dev,__LINE__);
	int result;
	
	result = yaffs_ReadChunkWithTagsFromNAND(dev,chunkInNAND,buffer,&tempTags);
	if(memcmp(buffer,data,dev->nDataBytesPerChunk) ||
		tempTags.objectId != tags->objectId ||
		tempTags.chunkId  != tags->chunkId ||
		tempTags.byteCount != tags->byteCount)
		retval = YAFFS_FAIL;

	yaffs_ReleaseTempBuffer(dev, buffer, __LINE__);

	return retval;
}

static int yaffs_WriteNewChunkWithTagsToNAND(struct yaffs_DeviceStruct *dev,
					const __u8 *data,
					yaffs_ExtendedTags *tags,
					int useReserve)
{
	int attempts = 0;
	int writeOk = 0;
	int chunk;

	yaffs2_InvalidateCheckpoint(dev);

	do {
		yaffs_BlockInfo *bi = 0;
		int erasedOk = 0;

		chunk = yaffs_AllocateChunk(dev, useReserve, &bi);
		if (chunk < 0) {
			/* no space */
			break;
		}

		/* First check this chunk is erased, if it needs
		 * checking.  The checking policy (unless forced
		 * always on) is as follows:
		 *
		 * Check the first page we try to write in a block.
		 * If the check passes then we don't need to check any
		 * more.	If the check fails, we check again...
		 * If the block has been erased, we don't need to check.
		 *
		 * However, if the block has been prioritised for gc,
		 * then we think there might be something odd about
		 * this block and stop using it.
		 *
		 * Rationale: We should only ever see chunks that have
		 * not been erased if there was a partially written
		 * chunk due to power loss.  This checking policy should
		 * catch that case with very few checks and thus save a
		 * lot of checks that are most likely not needed.
		 *
		 * Mods to the above
		 * If an erase check fails or the write fails we skip the 
		 * rest of the block.
		 */

		/* let's give it a try */
		attempts++;

		if(dev->param.alwaysCheckErased)
			bi->skipErasedCheck = 0;

		if (!bi->skipErasedCheck) {
			erasedOk = yaffs_CheckChunkErased(dev, chunk);
			if (erasedOk != YAFFS_OK) {
				T(YAFFS_TRACE_ERROR,
				(TSTR("**>> yaffs chunk %d was not erased"
				TENDSTR), chunk));
=======
int yaffs_check_ff(u8 *buffer, int n_bytes)
{
	/* Horrible, slow implementation */
	while (n_bytes--) {
		if (*buffer != 0xff)
			return 0;
		buffer++;
	}
	return 1;
}

static int yaffs_check_chunk_erased(struct yaffs_dev *dev, int nand_chunk)
{
	int retval = YAFFS_OK;
	u8 *data = yaffs_get_temp_buffer(dev);
	struct yaffs_ext_tags tags;
	int result;

	result = yaffs_rd_chunk_tags_nand(dev, nand_chunk, data, &tags);

	if (tags.ecc_result > YAFFS_ECC_RESULT_NO_ERROR)
		retval = YAFFS_FAIL;

	if (!yaffs_check_ff(data, dev->data_bytes_per_chunk) ||
		tags.chunk_used) {
		yaffs_trace(YAFFS_TRACE_NANDACCESS,
			"Chunk %d not erased", nand_chunk);
		retval = YAFFS_FAIL;
	}

	yaffs_release_temp_buffer(dev, data);

	return retval;

}

static int yaffs_verify_chunk_written(struct yaffs_dev *dev,
				      int nand_chunk,
				      const u8 *data,
				      struct yaffs_ext_tags *tags)
{
	int retval = YAFFS_OK;
	struct yaffs_ext_tags temp_tags;
	u8 *buffer = yaffs_get_temp_buffer(dev);
	int result;

	result = yaffs_rd_chunk_tags_nand(dev, nand_chunk, buffer, &temp_tags);
	if (memcmp(buffer, data, dev->data_bytes_per_chunk) ||
	    temp_tags.obj_id != tags->obj_id ||
	    temp_tags.chunk_id != tags->chunk_id ||
	    temp_tags.n_bytes != tags->n_bytes)
		retval = YAFFS_FAIL;

	yaffs_release_temp_buffer(dev, buffer);

	return retval;
}


int yaffs_check_alloc_available(struct yaffs_dev *dev, int n_chunks)
{
	int reserved_chunks;
	int reserved_blocks = dev->param.n_reserved_blocks;
	int checkpt_blocks;

	checkpt_blocks = yaffs_calc_checkpt_blocks_required(dev);

	reserved_chunks =
	    (reserved_blocks + checkpt_blocks) * dev->param.chunks_per_block;

	return (dev->n_free_chunks > (reserved_chunks + n_chunks));
}

static int yaffs_find_alloc_block(struct yaffs_dev *dev)
{
	int i;
	struct yaffs_block_info *bi;

	if (dev->n_erased_blocks < 1) {
		/* Hoosterman we've got a problem.
		 * Can't get space to gc
		 */
		yaffs_trace(YAFFS_TRACE_ERROR,
		  "yaffs tragedy: no more erased blocks");

		return -1;
	}

	/* Find an empty block. */

	for (i = dev->internal_start_block; i <= dev->internal_end_block; i++) {
		dev->alloc_block_finder++;
		if (dev->alloc_block_finder < dev->internal_start_block
		    || dev->alloc_block_finder > dev->internal_end_block) {
			dev->alloc_block_finder = dev->internal_start_block;
		}

		bi = yaffs_get_block_info(dev, dev->alloc_block_finder);

		if (bi->block_state == YAFFS_BLOCK_STATE_EMPTY) {
			bi->block_state = YAFFS_BLOCK_STATE_ALLOCATING;
			dev->seq_number++;
			bi->seq_number = dev->seq_number;
			dev->n_erased_blocks--;
			yaffs_trace(YAFFS_TRACE_ALLOCATE,
			  "Allocated block %d, seq  %d, %d left" ,
			   dev->alloc_block_finder, dev->seq_number,
			   dev->n_erased_blocks);
			return dev->alloc_block_finder;
		}
	}

	yaffs_trace(YAFFS_TRACE_ALWAYS,
		"yaffs tragedy: no more erased blocks, but there should have been %d",
		dev->n_erased_blocks);

	return -1;
}

static int yaffs_alloc_chunk(struct yaffs_dev *dev, int use_reserver,
			     struct yaffs_block_info **block_ptr)
{
	int ret_val;
	struct yaffs_block_info *bi;

	if (dev->alloc_block < 0) {
		/* Get next block to allocate off */
		dev->alloc_block = yaffs_find_alloc_block(dev);
		dev->alloc_page = 0;
	}

	if (!use_reserver && !yaffs_check_alloc_available(dev, 1)) {
		/* No space unless we're allowed to use the reserve. */
		return -1;
	}

	if (dev->n_erased_blocks < dev->param.n_reserved_blocks
	    && dev->alloc_page == 0)
		yaffs_trace(YAFFS_TRACE_ALLOCATE, "Allocating reserve");

	/* Next page please.... */
	if (dev->alloc_block >= 0) {
		bi = yaffs_get_block_info(dev, dev->alloc_block);

		ret_val = (dev->alloc_block * dev->param.chunks_per_block) +
		    dev->alloc_page;
		bi->pages_in_use++;
		yaffs_set_chunk_bit(dev, dev->alloc_block, dev->alloc_page);

		dev->alloc_page++;

		dev->n_free_chunks--;

		/* If the block is full set the state to full */
		if (dev->alloc_page >= dev->param.chunks_per_block) {
			bi->block_state = YAFFS_BLOCK_STATE_FULL;
			dev->alloc_block = -1;
		}

		if (block_ptr)
			*block_ptr = bi;

		return ret_val;
	}

	yaffs_trace(YAFFS_TRACE_ERROR,
		"!!!!!!!!! Allocator out !!!!!!!!!!!!!!!!!");

	return -1;
}

static int yaffs_get_erased_chunks(struct yaffs_dev *dev)
{
	int n;

	n = dev->n_erased_blocks * dev->param.chunks_per_block;

	if (dev->alloc_block > 0)
		n += (dev->param.chunks_per_block - dev->alloc_page);

	return n;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

				/* If not erased, delete this one,
				 * skip rest of block and
				 * try another chunk */
				 yaffs_DeleteChunk(dev,chunk,1,__LINE__);
				 yaffs_SkipRestOfBlock(dev);
				continue;
			}
		}

<<<<<<< HEAD
		writeOk = yaffs_WriteChunkWithTagsToNAND(dev, chunk,
				data, tags);

		if(!bi->skipErasedCheck)
			writeOk = yaffs_VerifyChunkWritten(dev, chunk, data, tags);

		if (writeOk != YAFFS_OK) {
			/* Clean up aborted write, skip to next block and
			 * try another chunk */
			yaffs_HandleWriteChunkError(dev, chunk, erasedOk);
			continue;
		}

		bi->skipErasedCheck = 1;

		/* Copy the data into the robustification buffer */
		yaffs_HandleWriteChunkOk(dev, chunk, data, tags);

	} while (writeOk != YAFFS_OK &&
		(yaffs_wr_attempts <= 0 || attempts <= yaffs_wr_attempts));

	if (!writeOk)
		chunk = -1;

	if (attempts > 1) {
		T(YAFFS_TRACE_ERROR,
			(TSTR("**>> yaffs write required %d attempts" TENDSTR),
			attempts));

		dev->nRetriedWrites += (attempts - 1);
	}

	return chunk;
}
=======
/*
 * yaffs_skip_rest_of_block() skips over the rest of the allocation block
 * if we don't want to write to it.
 */
void yaffs_skip_rest_of_block(struct yaffs_dev *dev)
{
	struct yaffs_block_info *bi;

	if (dev->alloc_block > 0) {
		bi = yaffs_get_block_info(dev, dev->alloc_block);
		if (bi->block_state == YAFFS_BLOCK_STATE_ALLOCATING) {
			bi->block_state = YAFFS_BLOCK_STATE_FULL;
			dev->alloc_block = -1;
		}
	}
}

static int yaffs_write_new_chunk(struct yaffs_dev *dev,
				 const u8 *data,
				 struct yaffs_ext_tags *tags, int use_reserver)
{
	int attempts = 0;
	int write_ok = 0;
	int chunk;

	yaffs2_checkpt_invalidate(dev);

	do {
		struct yaffs_block_info *bi = 0;
		int erased_ok = 0;

		chunk = yaffs_alloc_chunk(dev, use_reserver, &bi);
		if (chunk < 0) {
			/* no space */
			break;
		}
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

		/* First check this chunk is erased, if it needs
		 * checking.  The checking policy (unless forced
		 * always on) is as follows:
		 *
		 * Check the first page we try to write in a block.
		 * If the check passes then we don't need to check any
		 * more.        If the check fails, we check again...
		 * If the block has been erased, we don't need to check.
		 *
		 * However, if the block has been prioritised for gc,
		 * then we think there might be something odd about
		 * this block and stop using it.
		 *
		 * Rationale: We should only ever see chunks that have
		 * not been erased if there was a partially written
		 * chunk due to power loss.  This checking policy should
		 * catch that case with very few checks and thus save a
		 * lot of checks that are most likely not needed.
		 *
		 * Mods to the above
		 * If an erase check fails or the write fails we skip the
		 * rest of the block.
		 */

<<<<<<< HEAD
 
/*
 * Block retiring for handling a broken block.
 */

static void yaffs_RetireBlock(yaffs_Device *dev, int blockInNAND)
{
	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, blockInNAND);

	yaffs2_InvalidateCheckpoint(dev);
	
	yaffs2_ClearOldestDirtySequence(dev,bi);

	if (yaffs_MarkBlockBad(dev, blockInNAND) != YAFFS_OK) {
		if (yaffs_EraseBlockInNAND(dev, blockInNAND) != YAFFS_OK) {
			T(YAFFS_TRACE_ALWAYS, (TSTR(
				"yaffs: Failed to mark bad and erase block %d"
				TENDSTR), blockInNAND));
		} else {
			yaffs_ExtendedTags tags;
			int chunkId = blockInNAND * dev->param.nChunksPerBlock;

			__u8 *buffer = yaffs_GetTempBuffer(dev, __LINE__);

			memset(buffer, 0xff, dev->nDataBytesPerChunk);
			yaffs_InitialiseTags(&tags);
			tags.sequenceNumber = YAFFS_SEQUENCE_BAD_BLOCK;
			if (dev->param.writeChunkWithTagsToNAND(dev, chunkId -
				dev->chunkOffset, buffer, &tags) != YAFFS_OK)
				T(YAFFS_TRACE_ALWAYS, (TSTR("yaffs: Failed to "
					TCONT("write bad block marker to block %d")
					TENDSTR), blockInNAND));

			yaffs_ReleaseTempBuffer(dev, buffer, __LINE__);
=======
		/* let's give it a try */
		attempts++;

		if (dev->param.always_check_erased)
			bi->skip_erased_check = 0;

		if (!bi->skip_erased_check) {
			erased_ok = yaffs_check_chunk_erased(dev, chunk);
			if (erased_ok != YAFFS_OK) {
				yaffs_trace(YAFFS_TRACE_ERROR,
				  "**>> yaffs chunk %d was not erased",
				  chunk);

				/* If not erased, delete this one,
				 * skip rest of block and
				 * try another chunk */
				yaffs_chunk_del(dev, chunk, 1, __LINE__);
				yaffs_skip_rest_of_block(dev);
				continue;
			}
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		}

<<<<<<< HEAD
	bi->blockState = YAFFS_BLOCK_STATE_DEAD;
	bi->gcPrioritise = 0;
	bi->needsRetiring = 0;

	dev->nRetiredBlocks++;
}

/*
 * Functions for robustisizing TODO
 *
 */

static void yaffs_HandleWriteChunkOk(yaffs_Device *dev, int chunkInNAND,
				const __u8 *data,
				const yaffs_ExtendedTags *tags)
{
	dev=dev;
	chunkInNAND=chunkInNAND;
	data=data;
	tags=tags;
}

static void yaffs_HandleUpdateChunk(yaffs_Device *dev, int chunkInNAND,
				const yaffs_ExtendedTags *tags)
{
	dev=dev;
	chunkInNAND=chunkInNAND;
	tags=tags;
}

void yaffs_HandleChunkError(yaffs_Device *dev, yaffs_BlockInfo *bi)
{
	if (!bi->gcPrioritise) {
		bi->gcPrioritise = 1;
		dev->hasPendingPrioritisedGCs = 1;
		bi->chunkErrorStrikes++;

		if (bi->chunkErrorStrikes > 3) {
			bi->needsRetiring = 1; /* Too many stikes, so retire this */
			T(YAFFS_TRACE_ALWAYS, (TSTR("yaffs: Block struck out" TENDSTR)));

=======
		write_ok = yaffs_wr_chunk_tags_nand(dev, chunk, data, tags);

		if (!bi->skip_erased_check)
			write_ok =
			    yaffs_verify_chunk_written(dev, chunk, data, tags);

		if (write_ok != YAFFS_OK) {
			/* Clean up aborted write, skip to next block and
			 * try another chunk */
			yaffs_handle_chunk_wr_error(dev, chunk, erased_ok);
			continue;
		}

		bi->skip_erased_check = 1;

		/* Copy the data into the robustification buffer */
		yaffs_handle_chunk_wr_ok(dev, chunk, data, tags);

	} while (write_ok != YAFFS_OK &&
		 (yaffs_wr_attempts <= 0 || attempts <= yaffs_wr_attempts));

	if (!write_ok)
		chunk = -1;

	if (attempts > 1) {
		yaffs_trace(YAFFS_TRACE_ERROR,
			"**>> yaffs write required %d attempts",
			attempts);
		dev->n_retired_writes += (attempts - 1);
	}

	return chunk;
}

/*
 * Block retiring for handling a broken block.
 */

static void yaffs_retire_block(struct yaffs_dev *dev, int flash_block)
{
	struct yaffs_block_info *bi = yaffs_get_block_info(dev, flash_block);

	yaffs2_checkpt_invalidate(dev);

	yaffs2_clear_oldest_dirty_seq(dev, bi);

	if (yaffs_mark_bad(dev, flash_block) != YAFFS_OK) {
		if (yaffs_erase_block(dev, flash_block) != YAFFS_OK) {
			yaffs_trace(YAFFS_TRACE_ALWAYS,
				"yaffs: Failed to mark bad and erase block %d",
				flash_block);
		} else {
			struct yaffs_ext_tags tags;
			int chunk_id =
			    flash_block * dev->param.chunks_per_block;

			u8 *buffer = yaffs_get_temp_buffer(dev);

			memset(buffer, 0xff, dev->data_bytes_per_chunk);
			memset(&tags, 0, sizeof(tags));
			tags.seq_number = YAFFS_SEQUENCE_BAD_BLOCK;
			if (dev->param.write_chunk_tags_fn(dev, chunk_id -
							   dev->chunk_offset,
							   buffer,
							   &tags) != YAFFS_OK)
				yaffs_trace(YAFFS_TRACE_ALWAYS,
					"yaffs: Failed to write bad block marker to block %d",
					flash_block);

			yaffs_release_temp_buffer(dev, buffer);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		}
	}

	bi->block_state = YAFFS_BLOCK_STATE_DEAD;
	bi->gc_prioritise = 0;
	bi->needs_retiring = 0;

	dev->n_retired_blocks++;
}

<<<<<<< HEAD
static void yaffs_HandleWriteChunkError(yaffs_Device *dev, int chunkInNAND,
		int erasedOk)
{
	int blockInNAND = chunkInNAND / dev->param.nChunksPerBlock;
	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, blockInNAND);

	yaffs_HandleChunkError(dev, bi);

	if (erasedOk) {
		/* Was an actual write failure, so mark the block for retirement  */
		bi->needsRetiring = 1;
		T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
		  (TSTR("**>> Block %d needs retiring" TENDSTR), blockInNAND));
	}

	/* Delete the chunk */
	yaffs_DeleteChunk(dev, chunkInNAND, 1, __LINE__);
	yaffs_SkipRestOfBlock(dev);
}


/*---------------- Name handling functions ------------*/

static __u16 yaffs_CalcNameSum(const YCHAR *name)
{
	__u16 sum = 0;
	__u16 i = 1;

	const YUCHAR *bname = (const YUCHAR *) name;
	if (bname) {
		while ((*bname) && (i < (YAFFS_MAX_NAME_LENGTH/2))) {

#ifdef CONFIG_YAFFS_CASE_INSENSITIVE
			sum += yaffs_toupper(*bname) * i;
#else
			sum += (*bname) * i;
#endif
			i++;
			bname++;
		}
	}
	return sum;
}

void yaffs_SetObjectName(yaffs_Object *obj, const YCHAR *name)
{
#ifdef CONFIG_YAFFS_SHORT_NAMES_IN_RAM
	memset(obj->shortName, 0, sizeof(YCHAR) * (YAFFS_SHORT_NAME_LENGTH+1));
	if (name && yaffs_strnlen(name,YAFFS_SHORT_NAME_LENGTH+1) <= YAFFS_SHORT_NAME_LENGTH)
		yaffs_strcpy(obj->shortName, name);
	else
		obj->shortName[0] = _Y('\0');
#endif
	obj->sum = yaffs_CalcNameSum(name);
}

void yaffs_SetObjectNameFromOH(yaffs_Object *obj, const yaffs_ObjectHeader *oh)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE
	YCHAR tmpName[YAFFS_MAX_NAME_LENGTH+1];
	memset(tmpName,0,sizeof(tmpName));
	yaffs_LoadNameFromObjectHeader(obj->myDev,tmpName,oh->name,YAFFS_MAX_NAME_LENGTH+1);
	yaffs_SetObjectName(obj,tmpName);
#else
	yaffs_SetObjectName(obj,oh->name);
#endif
}

/*-------------------- TNODES -------------------

 * List of spare tnodes
 * The list is hooked together using the first pointer
 * in the tnode.
 */


yaffs_Tnode *yaffs_GetTnode(yaffs_Device *dev)
{
	yaffs_Tnode *tn = yaffs_AllocateRawTnode(dev);
	if (tn){
		memset(tn, 0, dev->tnodeSize);
		dev->nTnodes++;
	}

	dev->nCheckpointBlocksRequired = 0; /* force recalculation*/

	return tn;
}

/* FreeTnode frees up a tnode and puts it back on the free list */
static void yaffs_FreeTnode(yaffs_Device *dev, yaffs_Tnode *tn)
{
	yaffs_FreeRawTnode(dev,tn);
	dev->nTnodes--;
	dev->nCheckpointBlocksRequired = 0; /* force recalculation*/
}

static void yaffs_DeinitialiseTnodesAndObjects(yaffs_Device *dev)
{
	yaffs_DeinitialiseRawTnodesAndObjects(dev);
	dev->nObjects = 0;
	dev->nTnodes = 0;
}


void yaffs_LoadLevel0Tnode(yaffs_Device *dev, yaffs_Tnode *tn, unsigned pos,
		unsigned val)
{
	__u32 *map = (__u32 *)tn;
	__u32 bitInMap;
	__u32 bitInWord;
	__u32 wordInMap;
	__u32 mask;

	pos &= YAFFS_TNODES_LEVEL0_MASK;
	val >>= dev->chunkGroupBits;

	bitInMap = pos * dev->tnodeWidth;
	wordInMap = bitInMap / 32;
	bitInWord = bitInMap & (32 - 1);

	mask = dev->tnodeMask << bitInWord;

	map[wordInMap] &= ~mask;
	map[wordInMap] |= (mask & (val << bitInWord));

	if (dev->tnodeWidth > (32 - bitInWord)) {
		bitInWord = (32 - bitInWord);
		wordInMap++;;
		mask = dev->tnodeMask >> (/*dev->tnodeWidth -*/ bitInWord);
		map[wordInMap] &= ~mask;
		map[wordInMap] |= (mask & (val >> bitInWord));
	}
}

__u32 yaffs_GetChunkGroupBase(yaffs_Device *dev, yaffs_Tnode *tn,
		unsigned pos)
{
	__u32 *map = (__u32 *)tn;
	__u32 bitInMap;
	__u32 bitInWord;
	__u32 wordInMap;
	__u32 val;

	pos &= YAFFS_TNODES_LEVEL0_MASK;

	bitInMap = pos * dev->tnodeWidth;
	wordInMap = bitInMap / 32;
	bitInWord = bitInMap & (32 - 1);

	val = map[wordInMap] >> bitInWord;

	if	(dev->tnodeWidth > (32 - bitInWord)) {
		bitInWord = (32 - bitInWord);
		wordInMap++;;
		val |= (map[wordInMap] << bitInWord);
	}

	val &= dev->tnodeMask;
	val <<= dev->chunkGroupBits;

	return val;
}

/* ------------------- End of individual tnode manipulation -----------------*/

/* ---------Functions to manipulate the look-up tree (made up of tnodes) ------
 * The look up tree is represented by the top tnode and the number of topLevel
 * in the tree. 0 means only the level 0 tnode is in the tree.
 */

/* FindLevel0Tnode finds the level 0 tnode, if one exists. */
yaffs_Tnode *yaffs_FindLevel0Tnode(yaffs_Device *dev,
					yaffs_FileStructure *fStruct,
					__u32 chunkId)
{
	yaffs_Tnode *tn = fStruct->top;
	__u32 i;
	int requiredTallness;
	int level = fStruct->topLevel;

	dev=dev;

	/* Check sane level and chunk Id */
	if (level < 0 || level > YAFFS_TNODES_MAX_LEVEL)
		return NULL;

	if (chunkId > YAFFS_MAX_CHUNK_ID)
		return NULL;

	/* First check we're tall enough (ie enough topLevel) */

	i = chunkId >> YAFFS_TNODES_LEVEL0_BITS;
	requiredTallness = 0;
	while (i) {
		i >>= YAFFS_TNODES_INTERNAL_BITS;
		requiredTallness++;
	}

	if (requiredTallness > fStruct->topLevel)
		return NULL; /* Not tall enough, so we can't find it */

	/* Traverse down to level 0 */
	while (level > 0 && tn) {
		tn = tn->internal[(chunkId >>
			(YAFFS_TNODES_LEVEL0_BITS +
				(level - 1) *
				YAFFS_TNODES_INTERNAL_BITS)) &
			YAFFS_TNODES_INTERNAL_MASK];
		level--;
	}

	return tn;
}

/* AddOrFindLevel0Tnode finds the level 0 tnode if it exists, otherwise first expands the tree.
 * This happens in two steps:
 *  1. If the tree isn't tall enough, then make it taller.
 *  2. Scan down the tree towards the level 0 tnode adding tnodes if required.
 *
 * Used when modifying the tree.
 *
 *  If the tn argument is NULL, then a fresh tnode will be added otherwise the specified tn will
 *  be plugged into the ttree.
 */

static yaffs_Tnode *yaffs_AddOrFindLevel0Tnode(yaffs_Device *dev,
					yaffs_FileStructure *fStruct,
					__u32 chunkId,
					yaffs_Tnode *passedTn)
{
	int requiredTallness;
	int i;
	int l;
	yaffs_Tnode *tn;

	__u32 x;


	/* Check sane level and page Id */
	if (fStruct->topLevel < 0 || fStruct->topLevel > YAFFS_TNODES_MAX_LEVEL)
		return NULL;

	if (chunkId > YAFFS_MAX_CHUNK_ID)
		return NULL;

	/* First check we're tall enough (ie enough topLevel) */

	x = chunkId >> YAFFS_TNODES_LEVEL0_BITS;
	requiredTallness = 0;
	while (x) {
		x >>= YAFFS_TNODES_INTERNAL_BITS;
		requiredTallness++;
	}


	if (requiredTallness > fStruct->topLevel) {
		/* Not tall enough, gotta make the tree taller */
		for (i = fStruct->topLevel; i < requiredTallness; i++) {

			tn = yaffs_GetTnode(dev);

			if (tn) {
				tn->internal[0] = fStruct->top;
				fStruct->top = tn;
			} else {
				T(YAFFS_TRACE_ERROR,
				  (TSTR("yaffs: no more tnodes" TENDSTR)));
			}
		}

		fStruct->topLevel = requiredTallness;
	}

	/* Traverse down to level 0, adding anything we need */

	l = fStruct->topLevel;
	tn = fStruct->top;

	if (l > 0) {
		while (l > 0 && tn) {
			x = (chunkId >>
			     (YAFFS_TNODES_LEVEL0_BITS +
			      (l - 1) * YAFFS_TNODES_INTERNAL_BITS)) &
			    YAFFS_TNODES_INTERNAL_MASK;


			if ((l > 1) && !tn->internal[x]) {
				/* Add missing non-level-zero tnode */
				tn->internal[x] = yaffs_GetTnode(dev);

			} else if (l == 1) {
				/* Looking from level 1 at level 0 */
				if (passedTn) {
					/* If we already have one, then release it.*/
					if (tn->internal[x])
						yaffs_FreeTnode(dev, tn->internal[x]);
					tn->internal[x] = passedTn;

				} else if (!tn->internal[x]) {
					/* Don't have one, none passed in */
					tn->internal[x] = yaffs_GetTnode(dev);
				}
			}

			tn = tn->internal[x];
			l--;
		}
	} else {
		/* top is level 0 */
		if (passedTn) {
			memcpy(tn, passedTn, (dev->tnodeWidth * YAFFS_NTNODES_LEVEL0)/8);
			yaffs_FreeTnode(dev, passedTn);
		}
	}

	return tn;
}

static int yaffs_FindChunkInGroup(yaffs_Device *dev, int theChunk,
				yaffs_ExtendedTags *tags, int objectId,
				int chunkInInode)
{
	int j;

	for (j = 0; theChunk && j < dev->chunkGroupSize; j++) {
		if (yaffs_CheckChunkBit(dev, theChunk / dev->nChunksPerBlock,
				theChunk % dev->nChunksPerBlock)) {
			
			if(dev->chunkGroupSize == 1)
				return theChunk;
			else {
				yaffs_ReadChunkWithTagsFromNAND(dev, theChunk, NULL,
								tags);
				if (yaffs_TagsMatch(tags, objectId, chunkInInode)) {
					/* found it; */
					return theChunk;
				}
			}
		}
		theChunk++;
	}
	return -1;
}


/* DeleteWorker scans backwards through the tnode tree and deletes all the
 * chunks and tnodes in the file
 * Returns 1 if the tree was deleted.
 * Returns 0 if it stopped early due to hitting the limit and the delete is incomplete.
 */
#if 0 /*Mark the unused function to remove warning message*/
static int yaffs_DeleteWorker(yaffs_Object *in, yaffs_Tnode *tn, __u32 level,
			      int chunkOffset, int *limit)
{
	int i;
	int chunkInInode;
	int theChunk;
	yaffs_ExtendedTags tags;
	int foundChunk;
	yaffs_Device *dev = in->myDev;

	int allDone = 1;

	if (tn) {
		if (level > 0) {
			for (i = YAFFS_NTNODES_INTERNAL - 1; allDone && i >= 0;
			     i--) {
				if (tn->internal[i]) {
					if (limit && (*limit) < 0) {
						allDone = 0;
					} else {
						allDone =
							yaffs_DeleteWorker(in,
								tn->
								internal
								[i],
								level -
								1,
								(chunkOffset
									<<
									YAFFS_TNODES_INTERNAL_BITS)
								+ i,
								limit);
					}
					if (allDone) {
						yaffs_FreeTnode(dev,
								tn->
								internal[i]);
						tn->internal[i] = NULL;
					}
				}
			}
			return (allDone) ? 1 : 0;
		} else if (level == 0) {
			int hitLimit = 0;

			for (i = YAFFS_NTNODES_LEVEL0 - 1; i >= 0 && !hitLimit;
					i--) {
				theChunk = yaffs_GetChunkGroupBase(dev, tn, i);
				if (theChunk) {

					chunkInInode = (chunkOffset <<
						YAFFS_TNODES_LEVEL0_BITS) + i;

					foundChunk =
						yaffs_FindChunkInGroup(dev,
								theChunk,
								&tags,
								in->objectId,
								chunkInInode);

					if (foundChunk > 0) {
						yaffs_DeleteChunk(dev,
								  foundChunk, 1,
								  __LINE__);
						in->nDataChunks--;
						if (limit) {
							*limit = *limit - 1;
							if (*limit <= 0)
								hitLimit = 1;
						}

					}

					yaffs_PutLevel0Tnode(dev, tn, i, 0);
				}

			}
			return (i < 0) ? 1 : 0;

		}

	}

	return 1;

}
#endif
static void yaffs_SoftDeleteChunk(yaffs_Device *dev, int chunk)
{
	yaffs_BlockInfo *theBlock;

	T(YAFFS_TRACE_DELETION, (TSTR("soft delete chunk %d" TENDSTR), chunk));

	theBlock = yaffs_GetBlockInfo(dev, chunk / dev->nChunksPerBlock);
	if (theBlock) {
		theBlock->softDeletions++;
		dev->nFreeChunks++;
		yaffs_UpdateOldestDirtySequence(dev, theBlock);
	}
}

/* SoftDeleteWorker scans backwards through the tnode tree and soft deletes all the chunks in the file.
 * All soft deleting does is increment the block's softdelete count and pulls the chunk out
 * of the tnode.
 * Thus, essentially this is the same as DeleteWorker except that the chunks are soft deleted.
 */

static int yaffs_SoftDeleteWorker(yaffs_Object *in, yaffs_Tnode *tn,
				  __u32 level, int chunkOffset)
{
	int i;
	int theChunk;
	int allDone = 1;
	yaffs_Device *dev = in->myDev;

	if (tn) {
		if (level > 0) {

			for (i = YAFFS_NTNODES_INTERNAL - 1; allDone && i >= 0;
			     i--) {
				if (tn->internal[i]) {
					allDone =
					    yaffs_SoftDeleteWorker(in,
								   tn->
								   internal[i],
								   level - 1,
								   (chunkOffset
								    <<
								    YAFFS_TNODES_INTERNAL_BITS)
								   + i);
					if (allDone) {
						yaffs_FreeTnode(dev,
								tn->
								internal[i]);
						tn->internal[i] = NULL;
					} else {
						/* Hoosterman... how could this happen? */
					}
				}
			}
			return (allDone) ? 1 : 0;
		} else if (level == 0) {

			for (i = YAFFS_NTNODES_LEVEL0 - 1; i >= 0; i--) {
				theChunk = yaffs_GetChunkGroupBase(dev, tn, i);
				if (theChunk) {
					/* Note this does not find the real chunk, only the chunk group.
					 * We make an assumption that a chunk group is not larger than
					 * a block.
					 */
					yaffs_SoftDeleteChunk(dev, theChunk);
					yaffs_PutLevel0Tnode(dev, tn, i, 0);
				}

			}
			return 1;

		}

	}

	return 1;

}

static void yaffs_SoftDeleteFile(yaffs_Object *obj)
{
	if (obj->deleted &&
	    obj->variantType == YAFFS_OBJECT_TYPE_FILE && !obj->softDeleted) {
		if (obj->nDataChunks <= 0) {
			/* Empty file with no duplicate object headers, just delete it immediately */
			yaffs_FreeTnode(obj->myDev,
					obj->variant.fileVariant.top);
			obj->variant.fileVariant.top = NULL;
			T(YAFFS_TRACE_TRACING,
			  (TSTR("yaffs: Deleting empty file %d" TENDSTR),
			   obj->objectId));
			yaffs_DoGenericObjectDeletion(obj);
		} else {
			yaffs_SoftDeleteWorker(obj,
					       obj->variant.fileVariant.top,
					       obj->variant.fileVariant.
					       topLevel, 0);
			obj->softDeleted = 1;
		}
	}
}

/* Pruning removes any part of the file structure tree that is beyond the
 * bounds of the file (ie that does not point to chunks).
 *
 * A file should only get pruned when its size is reduced.
 *
 * Before pruning, the chunks must be pulled from the tree and the
 * level 0 tnode entries must be zeroed out.
 * Could also use this for file deletion, but that's probably better handled
 * by a special case.
 */

static yaffs_Tnode *yaffs_PruneWorker(yaffs_Device *dev, yaffs_Tnode *tn,
				__u32 level, int del0)
{
	int i;
	int hasData;

	if (tn) {
		hasData = 0;

		for (i = 0; i < YAFFS_NTNODES_INTERNAL; i++) {
			if (tn->internal[i] && level > 0) {
				tn->internal[i] =
				    yaffs_PruneWorker(dev, tn->internal[i],
						      level - 1,
						      (i == 0) ? del0 : 1);
			}

			if (tn->internal[i])
				hasData++;
		}

		if (hasData == 0 && del0) {
			/* Free and return NULL */

			yaffs_FreeTnode(dev, tn);
			tn = NULL;
		}

	}

	return tn;

}

static int yaffs_PruneFileStructure(yaffs_Device *dev,
				yaffs_FileStructure *fStruct)
{
	int i;
	int hasData;
	int done = 0;
	yaffs_Tnode *tn;

	if (fStruct->topLevel > 0) {
		fStruct->top =
		    yaffs_PruneWorker(dev, fStruct->top, fStruct->topLevel, 0);

		/* Now we have a tree with all the non-zero branches NULL but the height
		 * is the same as it was.
		 * Let's see if we can trim internal tnodes to shorten the tree.
		 * We can do this if only the 0th element in the tnode is in use
		 * (ie all the non-zero are NULL)
		 */

		while (fStruct->topLevel && !done) {
			tn = fStruct->top;

			hasData = 0;
			for (i = 1; i < YAFFS_NTNODES_INTERNAL; i++) {
				if (tn->internal[i])
					hasData++;
			}

			if (!hasData) {
				fStruct->top = tn->internal[0];
				fStruct->topLevel--;
				yaffs_FreeTnode(dev, tn);
			} else {
				done = 1;
			}
		}
	}

	return YAFFS_OK;
}

/*-------------------- End of File Structure functions.-------------------*/

/* yaffs_CreateFreeObjects creates a bunch more objects and
 * adds them to the object free list.
 */
static int yaffs_CreateFreeObjects(yaffs_Device *dev, int nObjects)
{
	int i;
	yaffs_Object *newObjects;
	yaffs_ObjectList *list;

	if (nObjects < 1)
		return YAFFS_OK;

	/* make these things */
	newObjects = YMALLOC(nObjects * sizeof(yaffs_Object));
	list = YMALLOC(sizeof(yaffs_ObjectList));

	if (!newObjects || !list) {
		if (newObjects)
			YFREE(newObjects);
		if (list)
			YFREE(list);
		T(YAFFS_TRACE_ALLOCATE,
		  (TSTR("yaffs: Could not allocate more objects" TENDSTR)));
		return YAFFS_FAIL;
	}

	/* Hook them into the free list */
	for (i = 0; i < nObjects - 1; i++) {
		newObjects[i].siblings.next =
				(struct ylist_head *)(&newObjects[i + 1]);
	}

	newObjects[nObjects - 1].siblings.next = (void *)dev->freeObjects;
	dev->freeObjects = newObjects;
	dev->nFreeObjects += nObjects;
	dev->nObjectsCreated += nObjects;

	/* Now add this bunch of Objects to a list for freeing up. */

	list->objects = newObjects;
	list->next = dev->allocatedObjectList;
	dev->allocatedObjectList = list;

	return YAFFS_OK;
}


/* AllocateEmptyObject gets us a clean Object. Tries to make allocate more if we run out */
static yaffs_Object *yaffs_AllocateEmptyObject(yaffs_Device *dev)
{
	yaffs_Object *tn = NULL;

#ifdef VALGRIND_TEST
	tn = YMALLOC(sizeof(yaffs_Object));
#else
	/* If there are none left make more */
	if (!dev->freeObjects)
		yaffs_CreateFreeObjects(dev, YAFFS_ALLOCATION_NOBJECTS);

	if (dev->freeObjects) {
		tn = dev->freeObjects;
		dev->freeObjects =
			(yaffs_Object *) (dev->freeObjects->siblings.next);
		dev->nFreeObjects--;
	}
#endif
	if (tn) {
		/* Now sweeten it up... */

		memset(tn, 0, sizeof(yaffs_Object));
		tn->beingCreated = 1;

		tn->myDev = dev;
		tn->hdrChunk = 0;
		tn->variantType = YAFFS_OBJECT_TYPE_UNKNOWN;
		YINIT_LIST_HEAD(&(tn->hardLinks));
		YINIT_LIST_HEAD(&(tn->hashLink));
		YINIT_LIST_HEAD(&tn->siblings);


		/* Now make the directory sane */
		if (dev->rootDir) {
			tn->parent = dev->rootDir;
			ylist_add(&(tn->siblings), &dev->rootDir->variant.directoryVariant.children);
		}

		/* Add it to the lost and found directory.
		 * NB Can't put root or lostNFound in lostNFound so
		 * check if lostNFound exists first
		 */
		if (dev->lostNFoundDir)
			yaffs_AddObjectToDirectory(dev->lostNFoundDir, tn);

		tn->beingCreated = 0;
	}

	dev->nCheckpointBlocksRequired = 0; /* force recalculation*/

	return tn;
}

static yaffs_Object *yaffs_CreateFakeDirectory(yaffs_Device *dev, int number,
					       __u32 mode)
{

	yaffs_Object *obj =
	    yaffs_CreateNewObject(dev, number, YAFFS_OBJECT_TYPE_DIRECTORY);
	if (obj) {
		obj->fake = 1;		/* it is fake so it might have no NAND presence... */
		obj->renameAllowed = 0;	/* ... and we're not allowed to rename it... */
		obj->unlinkAllowed = 0;	/* ... or unlink it */
		obj->deleted = 0;
		obj->unlinked = 0;
		obj->yst_mode = mode;
		obj->myDev = dev;
		obj->hdrChunk = 0;	/* Not a valid chunk. */
	}

	return obj;

}

static void yaffs_UnhashObject(yaffs_Object *tn)
{
	int bucket;
	yaffs_Device *dev = tn->myDev;

	/* If it is still linked into the bucket list, free from the list */
	if (!ylist_empty(&tn->hashLink)) {
		ylist_del_init(&tn->hashLink);
		bucket = yaffs_HashFunction(tn->objectId);
		dev->objectBucket[bucket].count--;
	}
}

/*  FreeObject frees up a Object and puts it back on the free list */
static void yaffs_FreeObject(yaffs_Object *tn)
{
	yaffs_Device *dev = tn->myDev;

#ifdef __KERNEL__
	T(YAFFS_TRACE_OS, (TSTR("FreeObject %p inode %p"TENDSTR), tn, tn->myInode));
#endif

	if (tn->parent)
		YBUG();
	if (!ylist_empty(&tn->siblings))
		YBUG();


#ifdef __KERNEL__
	if (tn->myInode) {
		/* We're still hooked up to a cached inode.
		 * Don't delete now, but mark for later deletion
		 */
		tn->deferedFree = 1;
		return;
	}
#endif

	yaffs_UnhashObject(tn);

#ifdef VALGRIND_TEST
	YFREE(tn);
#else
	/* Link into the free list. */
	tn->siblings.next = (struct ylist_head *)(dev->freeObjects);
	dev->freeObjects = tn;
	dev->nFreeObjects++;
#endif
	dev->nCheckpointBlocksRequired = 0; /* force recalculation*/
}

#ifdef __KERNEL__

void yaffs_HandleDeferedFree(yaffs_Object *obj)
{
	if (obj->deferedFree)
		yaffs_FreeObject(obj);
}

#endif

static void yaffs_DeinitialiseObjects(yaffs_Device *dev)
{
	/* Free the list of allocated Objects */

	yaffs_ObjectList *tmp;

	while (dev->allocatedObjectList) {
		tmp = dev->allocatedObjectList->next;
		YFREE(dev->allocatedObjectList->objects);
		YFREE(dev->allocatedObjectList);

		dev->allocatedObjectList = tmp;
	}

	dev->freeObjects = NULL;
	dev->nFreeObjects = 0;
}

static void yaffs_InitialiseObjects(yaffs_Device *dev)
{
	int i;

	dev->allocatedObjectList = NULL;
	dev->freeObjects = NULL;
	dev->nFreeObjects = 0;

	for (i = 0; i < YAFFS_NOBJECT_BUCKETS; i++) {
		YINIT_LIST_HEAD(&dev->objectBucket[i].list);
		dev->objectBucket[i].count = 0;
	}
}

static int yaffs_FindNiceObjectBucket(yaffs_Device *dev)
{
	static int x;
	int i;
	int l = 999;
	int lowest = 999999;

	/* First let's see if we can find one that's empty. */

	for (i = 0; i < 10 && lowest > 0; i++) {
		x++;
		x %= YAFFS_NOBJECT_BUCKETS;
		if (dev->objectBucket[x].count < lowest) {
			lowest = dev->objectBucket[x].count;
			l = x;
		}

	}

	/* If we didn't find an empty list, then try
	 * looking a bit further for a short one
	 */

	for (i = 0; i < 10 && lowest > 3; i++) {
		x++;
		x %= YAFFS_NOBJECT_BUCKETS;
		if (dev->objectBucket[x].count < lowest) {
			lowest = dev->objectBucket[x].count;
			l = x;
		}

	}

	return l;
}

static int yaffs_CreateNewObjectNumber(yaffs_Device *dev)
{
	int bucket = yaffs_FindNiceObjectBucket(dev);

	/* Now find an object value that has not already been taken
	 * by scanning the list.
	 */

	int found = 0;
	struct ylist_head *i;

	__u32 n = (__u32) bucket;

	/* yaffs_CheckObjectHashSanity();  */

	while (!found) {
		found = 1;
		n += YAFFS_NOBJECT_BUCKETS;
		if (1 || dev->objectBucket[bucket].count > 0) {
			ylist_for_each(i, &dev->objectBucket[bucket].list) {
				/* If there is already one in the list */
				if (i && ylist_entry(i, yaffs_Object,
						hashLink)->objectId == n) {
					found = 0;
				}
			}
		}
	}

	return n;
}

static void yaffs_HashObject(yaffs_Object *in)
{
	int bucket = yaffs_HashFunction(in->objectId);
	yaffs_Device *dev = in->myDev;

	ylist_add(&in->hashLink, &dev->objectBucket[bucket].list);
	dev->objectBucket[bucket].count++;
}

yaffs_Object *yaffs_FindObjectByNumber(yaffs_Device *dev, __u32 number)
{
	int bucket = yaffs_HashFunction(number);
	struct ylist_head *i;
	yaffs_Object *in;

	ylist_for_each(i, &dev->objectBucket[bucket].list) {
		/* Look if it is in the list */
		if (i) {
			in = ylist_entry(i, yaffs_Object, hashLink);
			if (in->objectId == number) {
#ifdef __KERNEL__
				/* Don't tell the VFS about this one if it is defered free */
				if (in->deferedFree)
					return NULL;
#endif

				return in;
			}
		}
	}

	return NULL;
}

yaffs_Object *yaffs_CreateNewObject(yaffs_Device *dev, int number,
				    yaffs_ObjectType type)
{
	yaffs_Object *theObject;
	yaffs_Tnode *tn = NULL;

	if (number < 0)
		number = yaffs_CreateNewObjectNumber(dev);

	theObject = yaffs_AllocateEmptyObject(dev);
	if (!theObject)
		return NULL;

	if (type == YAFFS_OBJECT_TYPE_FILE) {
		tn = yaffs_GetTnode(dev);
		if (!tn) {
			yaffs_FreeObject(theObject);
			return NULL;
		}
	}

	if (theObject) {
		theObject->fake = 0;
		theObject->renameAllowed = 1;
		theObject->unlinkAllowed = 1;
		theObject->objectId = number;
		yaffs_HashObject(theObject);
		theObject->variantType = type;
#ifdef CONFIG_YAFFS_WINCE
		yfsd_WinFileTimeNow(theObject->win_atime);
		theObject->win_ctime[0] = theObject->win_mtime[0] =
		    theObject->win_atime[0];
		theObject->win_ctime[1] = theObject->win_mtime[1] =
		    theObject->win_atime[1];

#else

		theObject->yst_atime = theObject->yst_mtime =
		    theObject->yst_ctime = Y_CURRENT_TIME;
#endif
		switch (type) {
		case YAFFS_OBJECT_TYPE_FILE:
			theObject->variant.fileVariant.fileSize = 0;
			theObject->variant.fileVariant.scannedFileSize = 0;
			theObject->variant.fileVariant.shrinkSize = 0xFFFFFFFF;	/* max __u32 */
			theObject->variant.fileVariant.topLevel = 0;
			theObject->variant.fileVariant.top = tn;
			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			YINIT_LIST_HEAD(&theObject->variant.directoryVariant.
					children);
			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
		case YAFFS_OBJECT_TYPE_HARDLINK:
		case YAFFS_OBJECT_TYPE_SPECIAL:
			/* No action required */
			break;
		case YAFFS_OBJECT_TYPE_UNKNOWN:
			/* todo this should not happen */
			break;
		}
	}

	return theObject;
}

static yaffs_Object *yaffs_FindOrCreateObjectByNumber(yaffs_Device *dev,
						      int number,
						      yaffs_ObjectType type)
{
	yaffs_Object *theObject = NULL;

	if (number > 0)
		theObject = yaffs_FindObjectByNumber(dev, number);

	if (!theObject)
		theObject = yaffs_CreateNewObject(dev, number, type);

	return theObject;

}


static YCHAR *yaffs_CloneString(const YCHAR *str)
{
	YCHAR *newStr = NULL;

	if (str && *str) {
		newStr = YMALLOC((yaffs_strlen(str) + 1) * sizeof(YCHAR));
		if (newStr)
			yaffs_strcpy(newStr, str);
	}

	return newStr;

}

/*
 * Mknod (create) a new object.
 * equivalentObject only has meaning for a hard link;
 * aliasString only has meaning for a sumlink.
 * rdev only has meaning for devices (a subset of special objects)
 */

static yaffs_Object *yaffs_MknodObject(yaffs_ObjectType type,
				       yaffs_Object *parent,
				       const YCHAR *name,
				       __u32 mode,
				       __u32 uid,
				       __u32 gid,
				       yaffs_Object *equivalentObject,
				       const YCHAR *aliasString, __u32 rdev)
{
	yaffs_Object *in;
	YCHAR *str = NULL;

	yaffs_Device *dev = parent->myDev;

	/* Check if the entry exists. If it does then fail the call since we don't want a dup.*/
	if (yaffs_FindObjectByName(parent, name))
		return NULL;

	in = yaffs_CreateNewObject(dev, -1, type);

	if (!in)
		return YAFFS_FAIL;

	if (type == YAFFS_OBJECT_TYPE_SYMLINK) {
		str = yaffs_CloneString(aliasString);
		if (!str) {
			yaffs_FreeObject(in);
			return NULL;
		}
	}



	if (in) {
		in->hdrChunk = 0;
		in->valid = 1;
		in->variantType = type;

		in->yst_mode = mode;

#ifdef CONFIG_YAFFS_WINCE
		yfsd_WinFileTimeNow(in->win_atime);
		in->win_ctime[0] = in->win_mtime[0] = in->win_atime[0];
		in->win_ctime[1] = in->win_mtime[1] = in->win_atime[1];

#else
		in->yst_atime = in->yst_mtime = in->yst_ctime = Y_CURRENT_TIME;

		in->yst_rdev = rdev;
		in->yst_uid = uid;
		in->yst_gid = gid;
#endif
		in->nDataChunks = 0;

		yaffs_SetObjectName(in, name);
		in->dirty = 1;

		yaffs_AddObjectToDirectory(parent, in);

		in->myDev = parent->myDev;

		switch (type) {
		case YAFFS_OBJECT_TYPE_SYMLINK:
			in->variant.symLinkVariant.alias = str;
			break;
		case YAFFS_OBJECT_TYPE_HARDLINK:
			in->variant.hardLinkVariant.equivalentObject =
				equivalentObject;
			in->variant.hardLinkVariant.equivalentObjectId =
				equivalentObject->objectId;
			ylist_add(&in->hardLinks, &equivalentObject->hardLinks);
			break;
		case YAFFS_OBJECT_TYPE_FILE:
		case YAFFS_OBJECT_TYPE_DIRECTORY:
		case YAFFS_OBJECT_TYPE_SPECIAL:
		case YAFFS_OBJECT_TYPE_UNKNOWN:
			/* do nothing */
			break;
		}

		if (yaffs_UpdateObjectHeader(in, name, 0, 0, 0) < 0) {
			/* Could not create the object header, fail the creation */
			yaffs_DeleteObject(in);
			in = NULL;
		}

		yaffs_UpdateParent(parent);
	}

	return in;
}

yaffs_Object *yaffs_MknodFile(yaffs_Object *parent, const YCHAR *name,
			__u32 mode, __u32 uid, __u32 gid)
{
	return yaffs_MknodObject(YAFFS_OBJECT_TYPE_FILE, parent, name, mode,
				uid, gid, NULL, NULL, 0);
}

yaffs_Object *yaffs_MknodDirectory(yaffs_Object *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid)
{
	return yaffs_MknodObject(YAFFS_OBJECT_TYPE_DIRECTORY, parent, name,
				 mode, uid, gid, NULL, NULL, 0);
}

yaffs_Object *yaffs_MknodSpecial(yaffs_Object *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid, __u32 rdev)
{
	return yaffs_MknodObject(YAFFS_OBJECT_TYPE_SPECIAL, parent, name, mode,
				 uid, gid, NULL, NULL, rdev);
}

yaffs_Object *yaffs_MknodSymLink(yaffs_Object *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid,
				const YCHAR *alias)
{
	return yaffs_MknodObject(YAFFS_OBJECT_TYPE_SYMLINK, parent, name, mode,
				uid, gid, NULL, alias, 0);
}

/* yaffs_Link returns the object id of the equivalent object.*/
yaffs_Object *yaffs_Link(yaffs_Object *parent, const YCHAR *name,
			yaffs_Object *equivalentObject)
{
	/* Get the real object in case we were fed a hard link as an equivalent object */
	equivalentObject = yaffs_GetEquivalentObject(equivalentObject);

	if (yaffs_MknodObject
	    (YAFFS_OBJECT_TYPE_HARDLINK, parent, name, 0, 0, 0,
	     equivalentObject, NULL, 0)) {
		return equivalentObject;
	} else {
		return NULL;
	}

}

static int yaffs_ChangeObjectName(yaffs_Object *obj, yaffs_Object *newDir,
				const YCHAR *newName, int force, int shadows)
{
	int unlinkOp;
	int deleteOp;

	yaffs_Object *existingTarget;

	if (newDir == NULL)
		newDir = obj->parent;	/* use the old directory */

	if (newDir->variantType != YAFFS_OBJECT_TYPE_DIRECTORY) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: yaffs_ChangeObjectName: newDir is not a directory"
		    TENDSTR)));
		YBUG();
	}

	/* TODO: Do we need this different handling for YAFFS2 and YAFFS1?? */
	if (obj->myDev->isYaffs2)
		unlinkOp = (newDir == obj->myDev->unlinkedDir);
	else
		unlinkOp = (newDir == obj->myDev->unlinkedDir
			    && obj->variantType == YAFFS_OBJECT_TYPE_FILE);

	deleteOp = (newDir == obj->myDev->deletedDir);

	existingTarget = yaffs_FindObjectByName(newDir, newName);

	/* If the object is a file going into the unlinked directory,
	 *   then it is OK to just stuff it in since duplicate names are allowed.
	 *   else only proceed if the new name does not exist and if we're putting
	 *   it into a directory.
	 */
	if ((unlinkOp ||
	     deleteOp ||
	     force ||
	     (shadows > 0) ||
	     !existingTarget) &&
	    newDir->variantType == YAFFS_OBJECT_TYPE_DIRECTORY) {
		yaffs_SetObjectName(obj, newName);
		obj->dirty = 1;

		yaffs_AddObjectToDirectory(newDir, obj);

		if (unlinkOp)
			obj->unlinked = 1;

		/* If it is a deletion then we mark it as a shrink for gc purposes. */
		if (yaffs_UpdateObjectHeader(obj, newName, 0, deleteOp, shadows) >= 0)
			return YAFFS_OK;
	}

	return YAFFS_FAIL;
}

int yaffs_RenameObject(yaffs_Object *oldDir, const YCHAR *oldName,
		yaffs_Object *newDir, const YCHAR *newName)
{
	yaffs_Object *obj = NULL;
	yaffs_Object *existingTarget = NULL;
	int force = 0;
	int result;
	yaffs_Device *dev;


	if (!oldDir || oldDir->variantType != YAFFS_OBJECT_TYPE_DIRECTORY)
		YBUG();
	if (!newDir || newDir->variantType != YAFFS_OBJECT_TYPE_DIRECTORY)
		YBUG();

	dev = oldDir->myDev;

#ifdef CONFIG_YAFFS_CASE_INSENSITIVE
	/* Special case for case insemsitive systems (eg. WinCE).
	 * While look-up is case insensitive, the name isn't.
	 * Therefore we might want to change x.txt to X.txt
	*/
	if (oldDir == newDir && yaffs_strcmp(oldName, newName) == 0)
		force = 1;
#endif

	if(yaffs_strlen(newName) > YAFFS_MAX_NAME_LENGTH)
		/* ENAMETOOLONG */
		return YAFFS_FAIL;

	obj = yaffs_FindObjectByName(oldDir, oldName);

	if (obj && obj->renameAllowed) {

		/* Now do the handling for an existing target, if there is one */

		existingTarget = yaffs_FindObjectByName(newDir, newName);
		if (existingTarget &&
			existingTarget->variantType == YAFFS_OBJECT_TYPE_DIRECTORY &&
			!ylist_empty(&existingTarget->variant.directoryVariant.children)) {
			/* There is a target that is a non-empty directory, so we fail */
			return YAFFS_FAIL;	/* EEXIST or ENOTEMPTY */
		} else if (existingTarget && existingTarget != obj) {
			/* Nuke the target first, using shadowing,
			 * but only if it isn't the same object.
			 *
			 * Note we must disable gc otherwise it can mess up the shadowing.
			 *
			 */
			dev->isDoingGC = 1;
			yaffs_ChangeObjectName(obj, newDir, newName, force,
						existingTarget->objectId);
			existingTarget->isShadowed = 1;
			yaffs_UnlinkObject(existingTarget);
			dev->isDoingGC = 0;
		}

		result = yaffs_ChangeObjectName(obj, newDir, newName, 1, 0);

		yaffs_UpdateParent(oldDir);
		if(newDir != oldDir)
			yaffs_UpdateParent(newDir);

		return result;
	}
	return YAFFS_FAIL;
}

/*------------------------- Block Management and Page Allocation ----------------*/

static int yaffs_InitialiseBlocks(yaffs_Device *dev)
{
	int nBlocks = dev->internalEndBlock - dev->internalStartBlock + 1;

	dev->blockInfo = NULL;
	dev->chunkBits = NULL;

	dev->allocationBlock = -1;	/* force it to get a new one */

	/* If the first allocation strategy fails, thry the alternate one */
	dev->blockInfo = YMALLOC(nBlocks * sizeof(yaffs_BlockInfo));
	if (!dev->blockInfo) {
		dev->blockInfo = YMALLOC_ALT(nBlocks * sizeof(yaffs_BlockInfo));
		dev->blockInfoAlt = 1;
	} else
		dev->blockInfoAlt = 0;

	if (dev->blockInfo) {
		/* Set up dynamic blockinfo stuff. */
		dev->chunkBitmapStride = (dev->nChunksPerBlock + 7) / 8; /* round up bytes */
		dev->chunkBits = YMALLOC(dev->chunkBitmapStride * nBlocks);
		if (!dev->chunkBits) {
			dev->chunkBits = YMALLOC_ALT(dev->chunkBitmapStride * nBlocks);
			dev->chunkBitsAlt = 1;
		} else
			dev->chunkBitsAlt = 0;
	}

	if (dev->blockInfo && dev->chunkBits) {
		memset(dev->blockInfo, 0, nBlocks * sizeof(yaffs_BlockInfo));
		memset(dev->chunkBits, 0, dev->chunkBitmapStride * nBlocks);
		return YAFFS_OK;
	}

	return YAFFS_FAIL;
}

static void yaffs_DeinitialiseBlocks(yaffs_Device *dev)
{
	if (dev->blockInfoAlt && dev->blockInfo)
		YFREE_ALT(dev->blockInfo);
	else if (dev->blockInfo)
		YFREE(dev->blockInfo);

	dev->blockInfoAlt = 0;

	dev->blockInfo = NULL;

	if (dev->chunkBitsAlt && dev->chunkBits)
		YFREE_ALT(dev->chunkBits);
	else if (dev->chunkBits)
		YFREE(dev->chunkBits);
	dev->chunkBitsAlt = 0;
	dev->chunkBits = NULL;
}

static int yaffs_BlockNotDisqualifiedFromGC(yaffs_Device *dev,
					yaffs_BlockInfo *bi)
{
	if (!dev->isYaffs2)
		return 1;	/* disqualification only applies to yaffs2. */

	if (!bi->hasShrinkHeader)
		return 1;	/* can gc */

	yaffs_FindOldestDirtySequence(dev);

	/* Can't do gc of this block if there are any blocks older than this one that have
	 * discarded pages.
	 */
	return (bi->sequenceNumber <= dev->oldestDirtySequence);
}

/* FindDiretiestBlock is used to select the dirtiest block (or close enough)
 * for garbage collection.
 */

static int yaffs_FindBlockForGarbageCollection(yaffs_Device *dev,
					int aggressive)
{
	int b = dev->currentDirtyChecker;

	int i;
	int iterations;
	int dirtiest = -1;
	int pagesInUse = 0;
	int prioritised = 0;
	yaffs_BlockInfo *bi;
	int pendingPrioritisedExist = 0;

	/* First let's see if we need to grab a prioritised block */
	if (dev->hasPendingPrioritisedGCs) {
		for (i = dev->internalStartBlock; i < dev->internalEndBlock && !prioritised; i++) {

			bi = yaffs_GetBlockInfo(dev, i);
			/* yaffs_VerifyBlock(dev,bi,i); */

			if (bi->gcPrioritise) {
				pendingPrioritisedExist = 1;
				if (bi->blockState == YAFFS_BLOCK_STATE_FULL &&
				   yaffs_BlockNotDisqualifiedFromGC(dev, bi)) {
					pagesInUse = (bi->pagesInUse - bi->softDeletions);
					dirtiest = i;
					prioritised = 1;
					aggressive = 1; /* Fool the non-aggressive skip logiv below */
				}
			}
		}

		if (!pendingPrioritisedExist) /* None found, so we can clear this */
			dev->hasPendingPrioritisedGCs = 0;
	}

	/* If we're doing aggressive GC then we are happy to take a less-dirty block, and
	 * search harder.
	 * else (we're doing a leasurely gc), then we only bother to do this if the
	 * block has only a few pages in use.
	 */

	dev->nonAggressiveSkip--;

	if (!aggressive && (dev->nonAggressiveSkip > 0))
		return -1;

	if (!prioritised)
		pagesInUse =
			(aggressive) ? dev->nChunksPerBlock : YAFFS_PASSIVE_GC_CHUNKS + 1;

	if (aggressive)
		iterations =
		    dev->internalEndBlock - dev->internalStartBlock + 1;
	else {
		iterations =
		    dev->internalEndBlock - dev->internalStartBlock + 1;
		iterations = iterations / 16;
		if (iterations > 200)
			iterations = 200;
	}

	for (i = 0; i <= iterations && pagesInUse > 0 && !prioritised; i++) {
		b++;
		if (b < dev->internalStartBlock || b > dev->internalEndBlock)
			b = dev->internalStartBlock;

		if (b < dev->internalStartBlock || b > dev->internalEndBlock) {
			T(YAFFS_TRACE_ERROR,
			  (TSTR("**>> Block %d is not valid" TENDSTR), b));
			YBUG();
		}

		bi = yaffs_GetBlockInfo(dev, b);

		if (bi->blockState == YAFFS_BLOCK_STATE_FULL &&
			(bi->pagesInUse - bi->softDeletions) < pagesInUse &&
				yaffs_BlockNotDisqualifiedFromGC(dev, bi)) {
			dirtiest = b;
			pagesInUse = (bi->pagesInUse - bi->softDeletions);
		}
	}

	dev->currentDirtyChecker = b;

	if (dirtiest > 0) {
		T(YAFFS_TRACE_GC,
		  (TSTR("GC Selected block %d with %d free, prioritised:%d" TENDSTR), dirtiest,
		   dev->nChunksPerBlock - pagesInUse, prioritised));
	}

	if (dirtiest > 0)
		dev->nonAggressiveSkip = 4;

	return dirtiest;
}

static void yaffs_BlockBecameDirty(yaffs_Device *dev, int blockNo)
{
	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, blockNo);

	int erasedOk = 0;

	/* If the block is still healthy erase it and mark as clean.
	 * If the block has had a data failure, then retire it.
	 */

	T(YAFFS_TRACE_GC | YAFFS_TRACE_ERASE,
		(TSTR("yaffs_BlockBecameDirty block %d state %d %s"TENDSTR),
		blockNo, bi->blockState, (bi->needsRetiring) ? "needs retiring" : ""));

	yaffs_ClearOldestDirtySequence(dev, bi);

	bi->blockState = YAFFS_BLOCK_STATE_DIRTY;

	if (!bi->needsRetiring) {
		yaffs_InvalidateCheckpoint(dev);
		erasedOk = yaffs_EraseBlockInNAND(dev, blockNo);
		if (!erasedOk) {
			dev->nErasureFailures++;
			T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
			  (TSTR("**>> Erasure failed %d" TENDSTR), blockNo));
		}
	}

	if (erasedOk &&
	    ((yaffs_traceMask & YAFFS_TRACE_ERASE) || !yaffs_SkipVerification(dev))) {
		int i;
		for (i = 0; i < dev->nChunksPerBlock; i++) {
			if (!yaffs_CheckChunkErased
			    (dev, blockNo * dev->nChunksPerBlock + i)) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   (">>Block %d erasure supposedly OK, but chunk %d not erased"
				    TENDSTR), blockNo, i));
			}
		}
	}

	if (erasedOk) {
		/* Clean it up... */
		bi->blockState = YAFFS_BLOCK_STATE_EMPTY;
		dev->nErasedBlocks++;
		bi->pagesInUse = 0;
		bi->softDeletions = 0;
		bi->hasShrinkHeader = 0;
		bi->skipErasedCheck = 1;  /* This is clean, so no need to check */
		bi->gcPrioritise = 0;
		yaffs_ClearChunkBits(dev, blockNo);

		T(YAFFS_TRACE_ERASE,
		  (TSTR("Erased block %d" TENDSTR), blockNo));
	} else {
		dev->nFreeChunks -= dev->nChunksPerBlock;	/* We lost a block of free space */

		yaffs_RetireBlock(dev, blockNo);
		T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
		  (TSTR("**>> Block %d retired" TENDSTR), blockNo));
	}
}

static int yaffs_FindBlockForAllocation(yaffs_Device *dev)
{
	int i;

	yaffs_BlockInfo *bi;

	if (dev->nErasedBlocks < 1) {
		/* Hoosterman we've got a problem.
		 * Can't get space to gc
		 */
		T(YAFFS_TRACE_ERROR,
		  (TSTR("yaffs tragedy: no more erased blocks" TENDSTR)));

		return -1;
	}

	/* Find an empty block. */

	for (i = dev->internalStartBlock; i <= dev->internalEndBlock; i++) {
		dev->allocationBlockFinder++;
		if (dev->allocationBlockFinder < dev->internalStartBlock
		    || dev->allocationBlockFinder > dev->internalEndBlock) {
			dev->allocationBlockFinder = dev->internalStartBlock;
		}

		bi = yaffs_GetBlockInfo(dev, dev->allocationBlockFinder);

		if (bi->blockState == YAFFS_BLOCK_STATE_EMPTY) {
			bi->blockState = YAFFS_BLOCK_STATE_ALLOCATING;
			dev->sequenceNumber++;
			bi->sequenceNumber = dev->sequenceNumber;
			dev->nErasedBlocks--;
			T(YAFFS_TRACE_ALLOCATE,
			  (TSTR("Allocated block %d, seq  %d, %d left" TENDSTR),
			   dev->allocationBlockFinder, dev->sequenceNumber,
			   dev->nErasedBlocks));
			return dev->allocationBlockFinder;
		}
	}

	T(YAFFS_TRACE_ALWAYS,
	  (TSTR
	   ("yaffs tragedy: no more erased blocks, but there should have been %d"
	    TENDSTR), dev->nErasedBlocks));

	return -1;
}



static int yaffs_CalcCheckpointBlocksRequired(yaffs_Device *dev)
{
	if (!dev->nCheckpointBlocksRequired &&
	   dev->isYaffs2) {
		/* Not a valid value so recalculate */
		int nBytes = 0;
		int nBlocks;
		int devBlocks = (dev->endBlock - dev->startBlock + 1);
		int tnodeSize;

		tnodeSize = (dev->tnodeWidth * YAFFS_NTNODES_LEVEL0)/8;

		if (tnodeSize < sizeof(yaffs_Tnode))
			tnodeSize = sizeof(yaffs_Tnode);

		nBytes += sizeof(yaffs_CheckpointValidity);
		nBytes += sizeof(yaffs_CheckpointDevice);
		nBytes += devBlocks * sizeof(yaffs_BlockInfo);
		nBytes += devBlocks * dev->chunkBitmapStride;
		nBytes += (sizeof(yaffs_CheckpointObject) + sizeof(__u32)) * (dev->nObjectsCreated - dev->nFreeObjects);
		nBytes += (tnodeSize + sizeof(__u32)) * (dev->nTnodesCreated - dev->nFreeTnodes);
		nBytes += sizeof(yaffs_CheckpointValidity);
		nBytes += sizeof(__u32); /* checksum*/

		/* Round up and add 2 blocks to allow for some bad blocks, so add 3 */

		nBlocks = (nBytes/(dev->nDataBytesPerChunk * dev->nChunksPerBlock)) + 3;

		dev->nCheckpointBlocksRequired = nBlocks;
	}

	return dev->nCheckpointBlocksRequired;
}

/*
 * Check if there's space to allocate...
 * Thinks.... do we need top make this ths same as yaffs_GetFreeChunks()?
 */
static int yaffs_CheckSpaceForAllocation(yaffs_Device *dev)
{
	int reservedChunks;
	int reservedBlocks = dev->nReservedBlocks;
	int checkpointBlocks;

	if (dev->isYaffs2) {
		checkpointBlocks =  yaffs_CalcCheckpointBlocksRequired(dev) -
				    dev->blocksInCheckpoint;
		if (checkpointBlocks < 0)
			checkpointBlocks = 0;
	} else {
		checkpointBlocks = 0;
	}

	reservedChunks = ((reservedBlocks + checkpointBlocks) * dev->nChunksPerBlock);

	return (dev->nFreeChunks > reservedChunks);
}

static int yaffs_AllocateChunk(yaffs_Device *dev, int useReserve,
		yaffs_BlockInfo **blockUsedPtr)
{
	int retVal;
	yaffs_BlockInfo *bi;

	if (dev->allocationBlock < 0) {
		/* Get next block to allocate off */
		dev->allocationBlock = yaffs_FindBlockForAllocation(dev);
		dev->allocationPage = 0;
	}

	if (!useReserve && !yaffs_CheckSpaceForAllocation(dev)) {
		/* Not enough space to allocate unless we're allowed to use the reserve. */
		return -1;
	}

	if (dev->nErasedBlocks < dev->nReservedBlocks
			&& dev->allocationPage == 0) {
		T(YAFFS_TRACE_ALLOCATE, (TSTR("Allocating reserve" TENDSTR)));
	}

	/* Next page please.... */
	if (dev->allocationBlock >= 0) {
		bi = yaffs_GetBlockInfo(dev, dev->allocationBlock);

		retVal = (dev->allocationBlock * dev->nChunksPerBlock) +
			dev->allocationPage;
		bi->pagesInUse++;
		yaffs_SetChunkBit(dev, dev->allocationBlock,
				dev->allocationPage);

		dev->allocationPage++;

		dev->nFreeChunks--;

		/* If the block is full set the state to full */
		if (dev->allocationPage >= dev->nChunksPerBlock) {
			bi->blockState = YAFFS_BLOCK_STATE_FULL;
			dev->allocationBlock = -1;
		}

		if (blockUsedPtr)
			*blockUsedPtr = bi;

		return retVal;
	}

	T(YAFFS_TRACE_ERROR,
			(TSTR("!!!!!!!!! Allocator out !!!!!!!!!!!!!!!!!" TENDSTR)));

	return -1;
}

static int yaffs_GetErasedChunks(yaffs_Device *dev)
{
	int n;

	n = dev->nErasedBlocks * dev->nChunksPerBlock;

	if (dev->allocationBlock > 0)
		n += (dev->nChunksPerBlock - dev->allocationPage);

	return n;

}

static int yaffs_GarbageCollectBlock(yaffs_Device *dev, int block,
		int wholeBlock)
{
	int oldChunk;
	int newChunk;
	int markNAND;
	int retVal = YAFFS_OK;
	int cleanups = 0;
	int i;
	int isCheckpointBlock;
	int matchingChunk;
	int maxCopies;

	int chunksBefore = yaffs_GetErasedChunks(dev);
	int chunksAfter;

	yaffs_ExtendedTags tags;

	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, block);

	yaffs_Object *object;

	isCheckpointBlock = (bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT);


	T(YAFFS_TRACE_TRACING,
			(TSTR("Collecting block %d, in use %d, shrink %d, wholeBlock %d" TENDSTR),
			 block,
			 bi->pagesInUse,
			 bi->hasShrinkHeader,
			 wholeBlock));

	/*yaffs_VerifyFreeChunks(dev); */

	if(bi->blockState == YAFFS_BLOCK_STATE_FULL)
		bi->blockState = YAFFS_BLOCK_STATE_COLLECTING;
	
	bi->hasShrinkHeader = 0;	/* clear the flag so that the block can erase */

	/* Take off the number of soft deleted entries because
	 * they're going to get really deleted during GC.
	 */
	if (dev->gcChunk == 0) /* first time through for this block */
		dev->nFreeChunks -= bi->softDeletions;

	dev->isDoingGC = 1;

	if (isCheckpointBlock ||
			!yaffs_StillSomeChunkBits(dev, block)) {
		T(YAFFS_TRACE_TRACING,
				(TSTR
				 ("Collecting block %d that has no chunks in use" TENDSTR),
				 block));
		yaffs_BlockBecameDirty(dev, block);
	} else {

		__u8 *buffer = yaffs_GetTempBuffer(dev, __LINE__);

		yaffs_VerifyBlock(dev, bi, block);

		maxCopies = (wholeBlock) ? dev->nChunksPerBlock : 10;
		oldChunk = block * dev->nChunksPerBlock + dev->gcChunk;

		for (/* init already done */;
		     retVal == YAFFS_OK &&
		     dev->gcChunk < dev->nChunksPerBlock &&
		     (bi->blockState == YAFFS_BLOCK_STATE_COLLECTING) &&
		     maxCopies > 0;
		     dev->gcChunk++, oldChunk++) {
			if (yaffs_CheckChunkBit(dev, block, dev->gcChunk)) {

				/* This page is in use and might need to be copied off */

				maxCopies--;

				markNAND = 1;

				yaffs_InitialiseTags(&tags);

				yaffs_ReadChunkWithTagsFromNAND(dev, oldChunk,
								buffer, &tags);

				object =
				    yaffs_FindObjectByNumber(dev,
							     tags.objectId);

				T(YAFFS_TRACE_GC_DETAIL,
				  (TSTR
				   ("Collecting chunk in block %d, %d %d %d " TENDSTR),
				   dev->gcChunk, tags.objectId, tags.chunkId,
				   tags.byteCount));

				if (object && !yaffs_SkipVerification(dev)) {
					if (tags.chunkId == 0)
						matchingChunk = object->hdrChunk;
					else if (object->softDeleted)
						matchingChunk = oldChunk; /* Defeat the test */
					else
						matchingChunk = yaffs_FindChunkInFile(object, tags.chunkId, NULL);

					if (oldChunk != matchingChunk)
						T(YAFFS_TRACE_ERROR,
						  (TSTR("gc: page in gc mismatch: %d %d %d %d"TENDSTR),
						  oldChunk, matchingChunk, tags.objectId, tags.chunkId));

				}

				if (!object) {
					T(YAFFS_TRACE_ERROR,
					  (TSTR
					   ("page %d in gc has no object: %d %d %d "
					    TENDSTR), oldChunk,
					    tags.objectId, tags.chunkId, tags.byteCount));
				}

				if (object &&
				    object->deleted &&
				    object->softDeleted &&
				    tags.chunkId != 0) {
					/* Data chunk in a soft deleted file, throw it away
					 * It's a soft deleted data chunk,
					 * No need to copy this, just forget about it and
					 * fix up the object.
					 */

					object->nDataChunks--;

					if (object->nDataChunks <= 0) {
						/* remeber to clean up the object */
						dev->gcCleanupList[cleanups] =
						    tags.objectId;
						cleanups++;
					}
					markNAND = 0;
				} else if (0) {
					/* Todo object && object->deleted && object->nDataChunks == 0 */
					/* Deleted object header with no data chunks.
					 * Can be discarded and the file deleted.
					 */
					object->hdrChunk = 0;
					yaffs_FreeTnode(object->myDev,
							object->variant.
							fileVariant.top);
					object->variant.fileVariant.top = NULL;
					yaffs_DoGenericObjectDeletion(object);

				} else if (object) {
					/* It's either a data chunk in a live file or
					 * an ObjectHeader, so we're interested in it.
					 * NB Need to keep the ObjectHeaders of deleted files
					 * until the whole file has been deleted off
					 */
					tags.serialNumber++;

					dev->nGCCopies++;

					if (tags.chunkId == 0) {
						/* It is an object Id,
						 * We need to nuke the shrinkheader flags first
						 * Also need to clean up shadowing.
						 * We no longer want the shrinkHeader flag since its work is done
						 * and if it is left in place it will mess up scanning.
						 */

						yaffs_ObjectHeader *oh;
						oh = (yaffs_ObjectHeader *)buffer;
						oh->isShrink = 0;
						tags.extraIsShrinkHeader = 0;
						oh->shadowsObject = 0;
						oh->inbandShadowsObject = 0;
						tags.extraShadows = 0;

						yaffs_VerifyObjectHeader(object, oh, &tags, 1);
					}

					newChunk =
					    yaffs_WriteNewChunkWithTagsToNAND(dev, buffer, &tags, 1);

					if (newChunk < 0) {
						retVal = YAFFS_FAIL;
					} else {

						/* Ok, now fix up the Tnodes etc. */

						if (tags.chunkId == 0) {
							/* It's a header */
							object->hdrChunk =  newChunk;
							object->serial =   tags.serialNumber;
						} else {
							/* It's a data chunk */
							yaffs_PutChunkIntoFile
							    (object,
							     tags.chunkId,
							     newChunk, 0);
						}
					}
				}

				if (retVal == YAFFS_OK)
					yaffs_DeleteChunk(dev, oldChunk, markNAND, __LINE__);

			}
		}

		yaffs_ReleaseTempBuffer(dev, buffer, __LINE__);

yaffs_Tnode *yaffs_AddOrFindLevel0Tnode(yaffs_Device *dev,
					yaffs_FileStructure *fStruct,
					__u32 chunkId,
					yaffs_Tnode *passedTn)
{
	int requiredTallness;
	int i;
	int l;
	yaffs_Tnode *tn;

	__u32 x;


	/* Check sane level and page Id */
	if (fStruct->topLevel < 0 || fStruct->topLevel > YAFFS_TNODES_MAX_LEVEL)
		return NULL;

	if (chunkId > YAFFS_MAX_CHUNK_ID)
		return NULL;

	/* First check we're tall enough (ie enough topLevel) */

	x = chunkId >> YAFFS_TNODES_LEVEL0_BITS;
	requiredTallness = 0;
	while (x) {
		x >>= YAFFS_TNODES_INTERNAL_BITS;
		requiredTallness++;
	}


	if (requiredTallness > fStruct->topLevel) {
		/* Not tall enough, gotta make the tree taller */
		for (i = fStruct->topLevel; i < requiredTallness; i++) {

			tn = yaffs_GetTnode(dev);

			if (tn) {
				tn->internal[0] = fStruct->top;
				fStruct->top = tn;
				fStruct->topLevel++;
			} else {
				T(YAFFS_TRACE_ERROR,
					(TSTR("yaffs: no more tnodes" TENDSTR)));
				return NULL;
			}
		}
	}

	/* Traverse down to level 0, adding anything we need */

	l = fStruct->topLevel;
	tn = fStruct->top;

	if (l > 0) {
		while (l > 0 && tn) {
			x = (chunkId >>
			     (YAFFS_TNODES_LEVEL0_BITS +
			      (l - 1) * YAFFS_TNODES_INTERNAL_BITS)) &
			    YAFFS_TNODES_INTERNAL_MASK;


			if ((l > 1) && !tn->internal[x]) {
				/* Add missing non-level-zero tnode */
				tn->internal[x] = yaffs_GetTnode(dev);
				if(!tn->internal[x])
					return NULL;
			} else if (l == 1) {
				/* Looking from level 1 at level 0 */
				if (passedTn) {
					/* If we already have one, then release it.*/
					if (tn->internal[x])
						yaffs_FreeTnode(dev, tn->internal[x]);
					tn->internal[x] = passedTn;

				} else if (!tn->internal[x]) {
					/* Don't have one, none passed in */
					tn->internal[x] = yaffs_GetTnode(dev);
					if(!tn->internal[x])
						return NULL;
				}
			}

			tn = tn->internal[x];
			l--;
		}
	} else {
		/* top is level 0 */
		if (passedTn) {
			memcpy(tn, passedTn, (dev->tnodeWidth * YAFFS_NTNODES_LEVEL0)/8);
			yaffs_FreeTnode(dev, passedTn);
		}
	}

	return tn;
}

static int yaffs_FindChunkInGroup(yaffs_Device *dev, int theChunk,
				yaffs_ExtendedTags *tags, int objectId,
				int chunkInInode)
{
	int j;

	for (j = 0; theChunk && j < dev->chunkGroupSize; j++) {
		if (yaffs_CheckChunkBit(dev, theChunk / dev->param.nChunksPerBlock,
				theChunk % dev->param.nChunksPerBlock)) {
			
			if(dev->chunkGroupSize == 1)
				return theChunk;
			else {
				yaffs_ReadChunkWithTagsFromNAND(dev, theChunk, NULL,
								tags);
				if (yaffs_TagsMatch(tags, objectId, chunkInInode)) {
					/* found it; */
					return theChunk;
				}
			}
		}
		theChunk++;
	}
	return -1;
}

#if 0
/* Experimental code not being used yet. Might speed up file deletion */
/* DeleteWorker scans backwards through the tnode tree and deletes all the
 * chunks and tnodes in the file.
 * Returns 1 if the tree was deleted.
 * Returns 0 if it stopped early due to hitting the limit and the delete is incomplete.
 */

static int yaffs_DeleteWorker(yaffs_Object *in, yaffs_Tnode *tn, __u32 level,
			      int chunkOffset, int *limit)
{
	int i;
	int chunkInInode;
	int theChunk;
	yaffs_ExtendedTags tags;
	int foundChunk;
	yaffs_Device *dev = in->myDev;

	int allDone = 1;

	if (tn) {
		if (level > 0) {
			for (i = YAFFS_NTNODES_INTERNAL - 1; allDone && i >= 0;
			     i--) {
				if (tn->internal[i]) {
					if (limit && (*limit) < 0) {
						allDone = 0;
					} else {
						allDone =
							yaffs_DeleteWorker(in,
								tn->
								internal
								[i],
								level -
								1,
								(chunkOffset
									<<
									YAFFS_TNODES_INTERNAL_BITS)
								+ i,
								limit);
					}
					if (allDone) {
						yaffs_FreeTnode(dev,
								tn->
								internal[i]);
						tn->internal[i] = NULL;
					}
				}
			}
			return (allDone) ? 1 : 0;
		} else if (level == 0) {
			int hitLimit = 0;

			for (i = YAFFS_NTNODES_LEVEL0 - 1; i >= 0 && !hitLimit;
					i--) {
				theChunk = yaffs_GetChunkGroupBase(dev, tn, i);
				if (theChunk) {

					chunkInInode = (chunkOffset <<
						YAFFS_TNODES_LEVEL0_BITS) + i;

					foundChunk =
						yaffs_FindChunkInGroup(dev,
								theChunk,
								&tags,
								in->objectId,
								chunkInInode);

					if (foundChunk > 0) {
						yaffs_DeleteChunk(dev,
								  foundChunk, 1,
								  __LINE__);
						in->nDataChunks--;
						if (limit) {
							*limit = *limit - 1;
							if (*limit <= 0)
								hitLimit = 1;
						}

					}

					yaffs_LoadLevel0Tnode(dev, tn, i, 0);
				}

			}
			return (i < 0) ? 1 : 0;

		}

	}

	return 1;

}

#endif

static void yaffs_SoftDeleteChunk(yaffs_Device *dev, int chunk)
{
	yaffs_BlockInfo *theBlock;
	unsigned blockNo;

	T(YAFFS_TRACE_DELETION, (TSTR("soft delete chunk %d" TENDSTR), chunk));

	blockNo =  chunk / dev->param.nChunksPerBlock;
	theBlock = yaffs_GetBlockInfo(dev, blockNo);
	if (theBlock) {
		theBlock->softDeletions++;
		dev->nFreeChunks++;
		yaffs2_UpdateOldestDirtySequence(dev, blockNo, theBlock);
	}
}

/* SoftDeleteWorker scans backwards through the tnode tree and soft deletes all the chunks in the file.
 * All soft deleting does is increment the block's softdelete count and pulls the chunk out
 * of the tnode.
 * Thus, essentially this is the same as DeleteWorker except that the chunks are soft deleted.
 */

static int yaffs_SoftDeleteWorker(yaffs_Object *in, yaffs_Tnode *tn,
				  __u32 level, int chunkOffset)
{
	int i;
	int theChunk;
	int allDone = 1;
	yaffs_Device *dev = in->myDev;

	if (tn) {
		if (level > 0) {

			for (i = YAFFS_NTNODES_INTERNAL - 1; allDone && i >= 0;
			     i--) {
				if (tn->internal[i]) {
					allDone =
					    yaffs_SoftDeleteWorker(in,
								   tn->
								   internal[i],
								   level - 1,
								   (chunkOffset
								    <<
								    YAFFS_TNODES_INTERNAL_BITS)
								   + i);
					if (allDone) {
						yaffs_FreeTnode(dev,
								tn->
								internal[i]);
						tn->internal[i] = NULL;
					} else {
						/* Hoosterman... how could this happen? */
					}
				}
			}
			return (allDone) ? 1 : 0;
		} else if (level == 0) {

			for (i = YAFFS_NTNODES_LEVEL0 - 1; i >= 0; i--) {
				theChunk = yaffs_GetChunkGroupBase(dev, tn, i);
				if (theChunk) {
					/* Note this does not find the real chunk, only the chunk group.
					 * We make an assumption that a chunk group is not larger than
					 * a block.
					 */
					yaffs_SoftDeleteChunk(dev, theChunk);
					yaffs_LoadLevel0Tnode(dev, tn, i, 0);
				}

			}
			return 1;

		}

	}

	return 1;

}

static void yaffs_SoftDeleteFile(yaffs_Object *obj)
{
	if (obj->deleted &&
	    obj->variantType == YAFFS_OBJECT_TYPE_FILE && !obj->softDeleted) {
		if (obj->nDataChunks <= 0) {
			/* Empty file with no duplicate object headers, just delete it immediately */
			yaffs_FreeTnode(obj->myDev,
					obj->variant.fileVariant.top);
			obj->variant.fileVariant.top = NULL;
			T(YAFFS_TRACE_TRACING,
			  (TSTR("yaffs: Deleting empty file %d" TENDSTR),
			   obj->objectId));
			yaffs_DoGenericObjectDeletion(obj);
		} else {
			yaffs_SoftDeleteWorker(obj,
					       obj->variant.fileVariant.top,
					       obj->variant.fileVariant.
					       topLevel, 0);
			obj->softDeleted = 1;
		}
	}
}

/* Pruning removes any part of the file structure tree that is beyond the
 * bounds of the file (ie that does not point to chunks).
 *
 * A file should only get pruned when its size is reduced.
 *
 * Before pruning, the chunks must be pulled from the tree and the
 * level 0 tnode entries must be zeroed out.
 * Could also use this for file deletion, but that's probably better handled
 * by a special case.
 *
 * This function is recursive. For levels > 0 the function is called again on
 * any sub-tree. For level == 0 we just check if the sub-tree has data.
 * If there is no data in a subtree then it is pruned.
 */

static yaffs_Tnode *yaffs_PruneWorker(yaffs_Device *dev, yaffs_Tnode *tn,
				__u32 level, int del0)
{
	int i;
	int hasData;

	if (tn) {
		hasData = 0;

		if(level > 0){
			for (i = 0; i < YAFFS_NTNODES_INTERNAL; i++) {
				if (tn->internal[i]) {
					tn->internal[i] =
						yaffs_PruneWorker(dev, tn->internal[i],
							level - 1,
							(i == 0) ? del0 : 1);
				}

				if (tn->internal[i])
					hasData++;
			}
		} else {
			int tnodeSize_u32 = dev->tnodeSize/sizeof(__u32);
			__u32 *map = (__u32 *)tn;

                        for(i = 0; !hasData && i < tnodeSize_u32; i++){
                                if(map[i])
                                        hasData++;
                        }
                }

		if (hasData == 0 && del0) {
			/* Free and return NULL */

			yaffs_FreeTnode(dev, tn);
			tn = NULL;
		}

	}

	return tn;

}

static int yaffs_PruneFileStructure(yaffs_Device *dev,
				yaffs_FileStructure *fStruct)
{
	int i;
	int hasData;
	int done = 0;
	yaffs_Tnode *tn;

	if (fStruct->topLevel > 0) {
		fStruct->top =
		    yaffs_PruneWorker(dev, fStruct->top, fStruct->topLevel, 0);

		/* Now we have a tree with all the non-zero branches NULL but the height
		 * is the same as it was.
		 * Let's see if we can trim internal tnodes to shorten the tree.
		 * We can do this if only the 0th element in the tnode is in use
		 * (ie all the non-zero are NULL)
		 */

		while (fStruct->topLevel && !done) {
			tn = fStruct->top;

			hasData = 0;
			for (i = 1; i < YAFFS_NTNODES_INTERNAL; i++) {
				if (tn->internal[i])
					hasData++;
			}

			if (!hasData) {
				fStruct->top = tn->internal[0];
				fStruct->topLevel--;
				yaffs_FreeTnode(dev, tn);
			} else {
				done = 1;
			}
		}
	}

	return YAFFS_OK;
}

/*-------------------- End of File Structure functions.-------------------*/


/* AllocateEmptyObject gets us a clean Object. Tries to make allocate more if we run out */
static yaffs_Object *yaffs_AllocateEmptyObject(yaffs_Device *dev)
{
	yaffs_Object *obj = yaffs_AllocateRawObject(dev);

	if (obj) {
		dev->nObjects++;

		/* Now sweeten it up... */

		memset(obj, 0, sizeof(yaffs_Object));
		obj->beingCreated = 1;

		obj->myDev = dev;
		obj->hdrChunk = 0;
		obj->variantType = YAFFS_OBJECT_TYPE_UNKNOWN;
		YINIT_LIST_HEAD(&(obj->hardLinks));
		YINIT_LIST_HEAD(&(obj->hashLink));
		YINIT_LIST_HEAD(&obj->siblings);


		/* Now make the directory sane */
		if (dev->rootDir) {
			obj->parent = dev->rootDir;
			ylist_add(&(obj->siblings), &dev->rootDir->variant.directoryVariant.children);
		}

		/* Add it to the lost and found directory.
		 * NB Can't put root or lostNFound in lostNFound so
		 * check if lostNFound exists first
		 */
		if (dev->lostNFoundDir)
			yaffs_AddObjectToDirectory(dev->lostNFoundDir, obj);

		obj->beingCreated = 0;
	}

	dev->nCheckpointBlocksRequired = 0; /* force recalculation*/

	return obj;
}

static yaffs_Object *yaffs_CreateFakeDirectory(yaffs_Device *dev, int number,
					       __u32 mode)
{

	yaffs_Object *obj =
	    yaffs_CreateNewObject(dev, number, YAFFS_OBJECT_TYPE_DIRECTORY);
	if (obj) {
		obj->fake = 1;		/* it is fake so it might have no NAND presence... */
		obj->renameAllowed = 0;	/* ... and we're not allowed to rename it... */
		obj->unlinkAllowed = 0;	/* ... or unlink it */
		obj->deleted = 0;
		obj->unlinked = 0;
		obj->yst_mode = mode;
		obj->myDev = dev;
		obj->hdrChunk = 0;	/* Not a valid chunk. */
	}

	return obj;

}

static void yaffs_UnhashObject(yaffs_Object *obj)
{
	int bucket;
	yaffs_Device *dev = obj->myDev;

	/* If it is still linked into the bucket list, free from the list */
	if (!ylist_empty(&obj->hashLink)) {
		ylist_del_init(&obj->hashLink);
		bucket = yaffs_HashFunction(obj->objectId);
		dev->objectBucket[bucket].count--;
	}
}

/*  FreeObject frees up a Object and puts it back on the free list */
static void yaffs_FreeObject(yaffs_Object *obj)
{
	yaffs_Device *dev = obj->myDev;

	T(YAFFS_TRACE_OS, (TSTR("FreeObject %p inode %p"TENDSTR), obj, obj->myInode));

	if (!obj)
		YBUG();
	if (obj->parent)
		YBUG();
	if (!ylist_empty(&obj->siblings))
		YBUG();


	if (obj->myInode) {
		/* We're still hooked up to a cached inode.
		 * Don't delete now, but mark for later deletion
		 */
		obj->deferedFree = 1;
		return;
	}

	yaffs_UnhashObject(obj);

	yaffs_FreeRawObject(dev,obj);
	dev->nObjects--;
	dev->nCheckpointBlocksRequired = 0; /* force recalculation*/
}


void yaffs_HandleDeferedFree(yaffs_Object *obj)
{
	if (obj->deferedFree)
		yaffs_FreeObject(obj);
}

static void yaffs_InitialiseTnodesAndObjects(yaffs_Device *dev)
{
	int i;

	dev->nObjects = 0;
	dev->nTnodes = 0;

	yaffs_InitialiseRawTnodesAndObjects(dev);

	for (i = 0; i < YAFFS_NOBJECT_BUCKETS; i++) {
		YINIT_LIST_HEAD(&dev->objectBucket[i].list);
		dev->objectBucket[i].count = 0;
	}
}

static int yaffs_FindNiceObjectBucket(yaffs_Device *dev)
{
	int i;
	int l = 999;
	int lowest = 999999;


	/* Search for the shortest list or one that
	 * isn't too long.
	 */

	for (i = 0; i < 10 && lowest > 4; i++) {
		dev->bucketFinder++;
		dev->bucketFinder %= YAFFS_NOBJECT_BUCKETS;
		if (dev->objectBucket[dev->bucketFinder].count < lowest) {
			lowest = dev->objectBucket[dev->bucketFinder].count;
			l = dev->bucketFinder;
		}

	}

	return l;
}

static int yaffs_CreateNewObjectNumber(yaffs_Device *dev)
{
	int bucket = yaffs_FindNiceObjectBucket(dev);

	/* Now find an object value that has not already been taken
	 * by scanning the list.
	 */

	int found = 0;
	struct ylist_head *i;

	__u32 n = (__u32) bucket;

	/* yaffs_CheckObjectHashSanity();  */

	while (!found) {
		found = 1;
		n += YAFFS_NOBJECT_BUCKETS;
		if (1 || dev->objectBucket[bucket].count > 0) {
			ylist_for_each(i, &dev->objectBucket[bucket].list) {
				/* If there is already one in the list */
				if (i && ylist_entry(i, yaffs_Object,
						hashLink)->objectId == n) {
					found = 0;
				}
			}
		}
	}

	return n;
}

static void yaffs_HashObject(yaffs_Object *in)
{
	int bucket = yaffs_HashFunction(in->objectId);
	yaffs_Device *dev = in->myDev;

	ylist_add(&in->hashLink, &dev->objectBucket[bucket].list);
	dev->objectBucket[bucket].count++;
}

yaffs_Object *yaffs_FindObjectByNumber(yaffs_Device *dev, __u32 number)
{
	int bucket = yaffs_HashFunction(number);
	struct ylist_head *i;
	yaffs_Object *in;

	ylist_for_each(i, &dev->objectBucket[bucket].list) {
		/* Look if it is in the list */
		if (i) {
			in = ylist_entry(i, yaffs_Object, hashLink);
			if (in->objectId == number) {

				/* Don't tell the VFS about this one if it is defered free */
				if (in->deferedFree)
					return NULL;

				return in;
			}
		}
	}

	return NULL;
}

yaffs_Object *yaffs_CreateNewObject(yaffs_Device *dev, int number,
				    yaffs_ObjectType type)
{
	yaffs_Object *theObject=NULL;
	yaffs_Tnode *tn = NULL;

	if (number < 0)
		number = yaffs_CreateNewObjectNumber(dev);

	if (type == YAFFS_OBJECT_TYPE_FILE) {
		tn = yaffs_GetTnode(dev);
		if (!tn)
			return NULL;
	}

	theObject = yaffs_AllocateEmptyObject(dev);
	if (!theObject){
		if(tn)
			yaffs_FreeTnode(dev,tn);
		return NULL;
	}


	if (theObject) {
		theObject->fake = 0;
		theObject->renameAllowed = 1;
		theObject->unlinkAllowed = 1;
		theObject->objectId = number;
		yaffs_HashObject(theObject);
		theObject->variantType = type;
#ifdef CONFIG_YAFFS_WINCE
		yfsd_WinFileTimeNow(theObject->win_atime);
		theObject->win_ctime[0] = theObject->win_mtime[0] =
		    theObject->win_atime[0];
		theObject->win_ctime[1] = theObject->win_mtime[1] =
		    theObject->win_atime[1];

#else

		theObject->yst_atime = theObject->yst_mtime =
		    theObject->yst_ctime = Y_CURRENT_TIME;
#endif
		switch (type) {
		case YAFFS_OBJECT_TYPE_FILE:
			theObject->variant.fileVariant.fileSize = 0;
			theObject->variant.fileVariant.scannedFileSize = 0;
			theObject->variant.fileVariant.shrinkSize = 0xFFFFFFFF;	/* max __u32 */
			theObject->variant.fileVariant.topLevel = 0;
			theObject->variant.fileVariant.top = tn;
			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			YINIT_LIST_HEAD(&theObject->variant.directoryVariant.
					children);
			YINIT_LIST_HEAD(&theObject->variant.directoryVariant.
					dirty);
			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
		case YAFFS_OBJECT_TYPE_HARDLINK:
		case YAFFS_OBJECT_TYPE_SPECIAL:
			/* No action required */
			break;
		case YAFFS_OBJECT_TYPE_UNKNOWN:
			/* todo this should not happen */
			break;
		}
	}

	return theObject;
}

yaffs_Object *yaffs_FindOrCreateObjectByNumber(yaffs_Device *dev,
						int number,
						yaffs_ObjectType type)
{
	yaffs_Object *theObject = NULL;

	if (number > 0)
		theObject = yaffs_FindObjectByNumber(dev, number);

	if (!theObject)
		theObject = yaffs_CreateNewObject(dev, number, type);

	return theObject;

}


YCHAR *yaffs_CloneString(const YCHAR *str)
{
	YCHAR *newStr = NULL;
	int len;

	if (!str)
		str = _Y("");

	len = yaffs_strnlen(str,YAFFS_MAX_ALIAS_LENGTH);
	newStr = YMALLOC((len + 1) * sizeof(YCHAR));
	if (newStr){
		yaffs_strncpy(newStr, str,len);
		newStr[len] = 0;
	}
	return newStr;

}

/*
 * Mknod (create) a new object.
 * equivalentObject only has meaning for a hard link;
 * aliasString only has meaning for a symlink.
 * rdev only has meaning for devices (a subset of special objects)
 */

static yaffs_Object *yaffs_MknodObject(yaffs_ObjectType type,
				       yaffs_Object *parent,
				       const YCHAR *name,
				       __u32 mode,
				       __u32 uid,
				       __u32 gid,
				       yaffs_Object *equivalentObject,
				       const YCHAR *aliasString, __u32 rdev)
{
	yaffs_Object *in;
	YCHAR *str = NULL;

	yaffs_Device *dev = parent->myDev;

	/* Check if the entry exists. If it does then fail the call since we don't want a dup.*/
	if (yaffs_FindObjectByName(parent, name))
		return NULL;

	if (type == YAFFS_OBJECT_TYPE_SYMLINK) {
		str = yaffs_CloneString(aliasString);
		if (!str)
			return NULL;
	}

	in = yaffs_CreateNewObject(dev, -1, type);

	if (!in){
		if(str)
			YFREE(str);
		return NULL;
	}
=======
/*---------------- Name handling functions ------------*/

static u16 yaffs_calc_name_sum(const YCHAR *name)
{
	u16 sum = 0;
	u16 i = 1;

	if (!name)
		return 0;

	while ((*name) && i < (YAFFS_MAX_NAME_LENGTH / 2)) {

		/* 0x1f mask is case insensitive */
		sum += ((*name) & 0x1f) * i;
		i++;
		name++;
	}
	return sum;
}

void yaffs_set_obj_name(struct yaffs_obj *obj, const YCHAR * name)
{
#ifndef CONFIG_YAFFS_NO_SHORT_NAMES
	memset(obj->short_name, 0, sizeof(obj->short_name));
	if (name &&
		strnlen(name, YAFFS_SHORT_NAME_LENGTH + 1) <=
		YAFFS_SHORT_NAME_LENGTH)
		strcpy(obj->short_name, name);
	else
		obj->short_name[0] = _Y('\0');
#endif
	obj->sum = yaffs_calc_name_sum(name);
}

void yaffs_set_obj_name_from_oh(struct yaffs_obj *obj,
				const struct yaffs_obj_hdr *oh)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE
	YCHAR tmp_name[YAFFS_MAX_NAME_LENGTH + 1];
	memset(tmp_name, 0, sizeof(tmp_name));
	yaffs_load_name_from_oh(obj->my_dev, tmp_name, oh->name,
				YAFFS_MAX_NAME_LENGTH + 1);
	yaffs_set_obj_name(obj, tmp_name);
#else
	yaffs_set_obj_name(obj, oh->name);
#endif
}

/*-------------------- TNODES -------------------

 * List of spare tnodes
 * The list is hooked together using the first pointer
 * in the tnode.
 */

struct yaffs_tnode *yaffs_get_tnode(struct yaffs_dev *dev)
{
	struct yaffs_tnode *tn = yaffs_alloc_raw_tnode(dev);

	if (tn) {
		memset(tn, 0, dev->tnode_size);
		dev->n_tnodes++;
	}

	dev->checkpoint_blocks_required = 0;	/* force recalculation */

	return tn;
}

/* FreeTnode frees up a tnode and puts it back on the free list */
static void yaffs_free_tnode(struct yaffs_dev *dev, struct yaffs_tnode *tn)
{
	yaffs_free_raw_tnode(dev, tn);
	dev->n_tnodes--;
	dev->checkpoint_blocks_required = 0;	/* force recalculation */
}

static void yaffs_deinit_tnodes_and_objs(struct yaffs_dev *dev)
{
	yaffs_deinit_raw_tnodes_and_objs(dev);
	dev->n_obj = 0;
	dev->n_tnodes = 0;
}

void yaffs_load_tnode_0(struct yaffs_dev *dev, struct yaffs_tnode *tn,
			unsigned pos, unsigned val)
{
	u32 *map = (u32 *) tn;
	u32 bit_in_map;
	u32 bit_in_word;
	u32 word_in_map;
	u32 mask;

	pos &= YAFFS_TNODES_LEVEL0_MASK;
	val >>= dev->chunk_grp_bits;

	bit_in_map = pos * dev->tnode_width;
	word_in_map = bit_in_map / 32;
	bit_in_word = bit_in_map & (32 - 1);

	mask = dev->tnode_mask << bit_in_word;

	map[word_in_map] &= ~mask;
	map[word_in_map] |= (mask & (val << bit_in_word));

	if (dev->tnode_width > (32 - bit_in_word)) {
		bit_in_word = (32 - bit_in_word);
		word_in_map++;
		mask =
		    dev->tnode_mask >> bit_in_word;
		map[word_in_map] &= ~mask;
		map[word_in_map] |= (mask & (val >> bit_in_word));
	}
}

u32 yaffs_get_group_base(struct yaffs_dev *dev, struct yaffs_tnode *tn,
			 unsigned pos)
{
	u32 *map = (u32 *) tn;
	u32 bit_in_map;
	u32 bit_in_word;
	u32 word_in_map;
	u32 val;

	pos &= YAFFS_TNODES_LEVEL0_MASK;

	bit_in_map = pos * dev->tnode_width;
	word_in_map = bit_in_map / 32;
	bit_in_word = bit_in_map & (32 - 1);

	val = map[word_in_map] >> bit_in_word;

	if (dev->tnode_width > (32 - bit_in_word)) {
		bit_in_word = (32 - bit_in_word);
		word_in_map++;
		val |= (map[word_in_map] << bit_in_word);
	}

	val &= dev->tnode_mask;
	val <<= dev->chunk_grp_bits;

	return val;
}

/* ------------------- End of individual tnode manipulation -----------------*/

/* ---------Functions to manipulate the look-up tree (made up of tnodes) ------
 * The look up tree is represented by the top tnode and the number of top_level
 * in the tree. 0 means only the level 0 tnode is in the tree.
 */

/* FindLevel0Tnode finds the level 0 tnode, if one exists. */
struct yaffs_tnode *yaffs_find_tnode_0(struct yaffs_dev *dev,
				       struct yaffs_file_var *file_struct,
				       u32 chunk_id)
{
	struct yaffs_tnode *tn = file_struct->top;
	u32 i;
	int required_depth;
	int level = file_struct->top_level;

	dev = dev;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	/* Check sane level and chunk Id */
	if (level < 0 || level > YAFFS_TNODES_MAX_LEVEL)
		return NULL;

<<<<<<< HEAD



	if (in) {
		in->hdrChunk = 0;
		in->valid = 1;
		in->variantType = type;

		in->yst_mode = mode;

#ifdef CONFIG_YAFFS_WINCE
		yfsd_WinFileTimeNow(in->win_atime);
		in->win_ctime[0] = in->win_mtime[0] = in->win_atime[0];
		in->win_ctime[1] = in->win_mtime[1] = in->win_atime[1];

#else
		in->yst_atime = in->yst_mtime = in->yst_ctime = Y_CURRENT_TIME;

		in->yst_rdev = rdev;
		in->yst_uid = uid;
		in->yst_gid = gid;
#endif
		in->nDataChunks = 0;

		yaffs_SetObjectName(in, name);
		in->dirty = 1;

		yaffs_AddObjectToDirectory(parent, in);

		in->myDev = parent->myDev;

		switch (type) {
		case YAFFS_OBJECT_TYPE_SYMLINK:
			in->variant.symLinkVariant.alias = str;
			break;
		case YAFFS_OBJECT_TYPE_HARDLINK:
			in->variant.hardLinkVariant.equivalentObject =
				equivalentObject;
			in->variant.hardLinkVariant.equivalentObjectId =
				equivalentObject->objectId;
			ylist_add(&in->hardLinks, &equivalentObject->hardLinks);
			break;
		case YAFFS_OBJECT_TYPE_FILE:
		case YAFFS_OBJECT_TYPE_DIRECTORY:
		case YAFFS_OBJECT_TYPE_SPECIAL:
		case YAFFS_OBJECT_TYPE_UNKNOWN:
			/* do nothing */
			break;
		}

		if (yaffs_UpdateObjectHeader(in, name, 0, 0, 0, NULL) < 0) {
			/* Could not create the object header, fail the creation */
			yaffs_DeleteObject(in);
			in = NULL;
		}

		yaffs_UpdateParent(parent);
	}

	return in;
}

yaffs_Object *yaffs_MknodFile(yaffs_Object *parent, const YCHAR *name,
			__u32 mode, __u32 uid, __u32 gid)
{
	return yaffs_MknodObject(YAFFS_OBJECT_TYPE_FILE, parent, name, mode,
				uid, gid, NULL, NULL, 0);
}

yaffs_Object *yaffs_MknodDirectory(yaffs_Object *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid)
{
	return yaffs_MknodObject(YAFFS_OBJECT_TYPE_DIRECTORY, parent, name,
				 mode, uid, gid, NULL, NULL, 0);
}

yaffs_Object *yaffs_MknodSpecial(yaffs_Object *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid, __u32 rdev)
{
	return yaffs_MknodObject(YAFFS_OBJECT_TYPE_SPECIAL, parent, name, mode,
				 uid, gid, NULL, NULL, rdev);
}

yaffs_Object *yaffs_MknodSymLink(yaffs_Object *parent, const YCHAR *name,
				__u32 mode, __u32 uid, __u32 gid,
				const YCHAR *alias)
{
	return yaffs_MknodObject(YAFFS_OBJECT_TYPE_SYMLINK, parent, name, mode,
				uid, gid, NULL, alias, 0);
}

/* yaffs_Link returns the object id of the equivalent object.*/
yaffs_Object *yaffs_Link(yaffs_Object *parent, const YCHAR *name,
			yaffs_Object *equivalentObject)
{
	/* Get the real object in case we were fed a hard link as an equivalent object */
	equivalentObject = yaffs_GetEquivalentObject(equivalentObject);

	if (yaffs_MknodObject
	    (YAFFS_OBJECT_TYPE_HARDLINK, parent, name, 0, 0, 0,
	     equivalentObject, NULL, 0)) {
		return equivalentObject;
	} else {
		return NULL;
	}

}

static int yaffs_ChangeObjectName(yaffs_Object *obj, yaffs_Object *newDir,
				const YCHAR *newName, int force, int shadows)
{
	int unlinkOp;
	int deleteOp;

	yaffs_Object *existingTarget;

	if (newDir == NULL)
		newDir = obj->parent;	/* use the old directory */

	if (newDir->variantType != YAFFS_OBJECT_TYPE_DIRECTORY) {
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("tragedy: yaffs_ChangeObjectName: newDir is not a directory"
		    TENDSTR)));
		YBUG();
	}

	/* TODO: Do we need this different handling for YAFFS2 and YAFFS1?? */
	if (obj->myDev->param.isYaffs2)
		unlinkOp = (newDir == obj->myDev->unlinkedDir);
	else
		unlinkOp = (newDir == obj->myDev->unlinkedDir
			    && obj->variantType == YAFFS_OBJECT_TYPE_FILE);

	deleteOp = (newDir == obj->myDev->deletedDir);

	existingTarget = yaffs_FindObjectByName(newDir, newName);

	/* If the object is a file going into the unlinked directory,
	 *   then it is OK to just stuff it in since duplicate names are allowed.
	 *   else only proceed if the new name does not exist and if we're putting
	 *   it into a directory.
	 */
	if ((unlinkOp ||
	     deleteOp ||
	     force ||
	     (shadows > 0) ||
	     !existingTarget) &&
	    newDir->variantType == YAFFS_OBJECT_TYPE_DIRECTORY) {
		yaffs_SetObjectName(obj, newName);
		obj->dirty = 1;

		yaffs_AddObjectToDirectory(newDir, obj);

		if (unlinkOp)
			obj->unlinked = 1;

		/* If it is a deletion then we mark it as a shrink for gc purposes. */
		if (yaffs_UpdateObjectHeader(obj, newName, 0, deleteOp, shadows, NULL) >= 0)
			return YAFFS_OK;
	}

	return YAFFS_FAIL;
}

int yaffs_RenameObject(yaffs_Object *oldDir, const YCHAR *oldName,
		yaffs_Object *newDir, const YCHAR *newName)
{
	yaffs_Object *obj = NULL;
	yaffs_Object *existingTarget = NULL;
	int force = 0;
	int result;
	yaffs_Device *dev;


	if (!oldDir || oldDir->variantType != YAFFS_OBJECT_TYPE_DIRECTORY)
		YBUG();
	if (!newDir || newDir->variantType != YAFFS_OBJECT_TYPE_DIRECTORY)
		YBUG();

	dev = oldDir->myDev;

#ifdef CONFIG_YAFFS_CASE_INSENSITIVE
	/* Special case for case insemsitive systems (eg. WinCE).
	 * While look-up is case insensitive, the name isn't.
	 * Therefore we might want to change x.txt to X.txt
	*/
	if (oldDir == newDir && yaffs_strcmp(oldName, newName) == 0)
		force = 1;
#endif

	if(yaffs_strnlen(newName,YAFFS_MAX_NAME_LENGTH+1) > YAFFS_MAX_NAME_LENGTH)
		/* ENAMETOOLONG */
		return YAFFS_FAIL;

	obj = yaffs_FindObjectByName(oldDir, oldName);

	if (obj && obj->renameAllowed) {

		/* Now do the handling for an existing target, if there is one */

		existingTarget = yaffs_FindObjectByName(newDir, newName);
		if (existingTarget &&
			existingTarget->variantType == YAFFS_OBJECT_TYPE_DIRECTORY &&
			!ylist_empty(&existingTarget->variant.directoryVariant.children)) {
			/* There is a target that is a non-empty directory, so we fail */
			return YAFFS_FAIL;	/* EEXIST or ENOTEMPTY */
		} else if (existingTarget && existingTarget != obj) {
			/* Nuke the target first, using shadowing,
			 * but only if it isn't the same object.
			 *
			 * Note we must disable gc otherwise it can mess up the shadowing.
			 *
			 */
			dev->gcDisable=1;
			yaffs_ChangeObjectName(obj, newDir, newName, force,
						existingTarget->objectId);
			existingTarget->isShadowed = 1;
			yaffs_UnlinkObject(existingTarget);
			dev->gcDisable=0;
		}

		result = yaffs_ChangeObjectName(obj, newDir, newName, 1, 0);

		yaffs_UpdateParent(oldDir);
		if(newDir != oldDir)
			yaffs_UpdateParent(newDir);
		
		return result;
	}
	return YAFFS_FAIL;
}

/*------------------------- Block Management and Page Allocation ----------------*/

static int yaffs_InitialiseBlocks(yaffs_Device *dev)
{
	int nBlocks = dev->internalEndBlock - dev->internalStartBlock + 1;

	dev->blockInfo = NULL;
	dev->chunkBits = NULL;

	dev->allocationBlock = -1;	/* force it to get a new one */

	/* If the first allocation strategy fails, thry the alternate one */
	dev->blockInfo = YMALLOC(nBlocks * sizeof(yaffs_BlockInfo));
	if (!dev->blockInfo) {
		dev->blockInfo = YMALLOC_ALT(nBlocks * sizeof(yaffs_BlockInfo));
		dev->blockInfoAlt = 1;
	} else
		dev->blockInfoAlt = 0;

	if (dev->blockInfo) {
		/* Set up dynamic blockinfo stuff. */
		dev->chunkBitmapStride = (dev->param.nChunksPerBlock + 7) / 8; /* round up bytes */
		dev->chunkBits = YMALLOC(dev->chunkBitmapStride * nBlocks);
		if (!dev->chunkBits) {
			dev->chunkBits = YMALLOC_ALT(dev->chunkBitmapStride * nBlocks);
			dev->chunkBitsAlt = 1;
		} else
			dev->chunkBitsAlt = 0;
	}

	if (dev->blockInfo && dev->chunkBits) {
		memset(dev->blockInfo, 0, nBlocks * sizeof(yaffs_BlockInfo));
		memset(dev->chunkBits, 0, dev->chunkBitmapStride * nBlocks);
		return YAFFS_OK;
	}

	return YAFFS_FAIL;
}

static void yaffs_DeinitialiseBlocks(yaffs_Device *dev)
{
	if (dev->blockInfoAlt && dev->blockInfo)
		YFREE_ALT(dev->blockInfo);
	else if (dev->blockInfo)
		YFREE(dev->blockInfo);

	dev->blockInfoAlt = 0;

	dev->blockInfo = NULL;

	if (dev->chunkBitsAlt && dev->chunkBits)
		YFREE_ALT(dev->chunkBits);
	else if (dev->chunkBits)
		YFREE(dev->chunkBits);
	dev->chunkBitsAlt = 0;
	dev->chunkBits = NULL;
}

void yaffs_BlockBecameDirty(yaffs_Device *dev, int blockNo)
{
	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, blockNo);

	int erasedOk = 0;

	/* If the block is still healthy erase it and mark as clean.
	 * If the block has had a data failure, then retire it.
	 */

	T(YAFFS_TRACE_GC | YAFFS_TRACE_ERASE,
		(TSTR("yaffs_BlockBecameDirty block %d state %d %s"TENDSTR),
		blockNo, bi->blockState, (bi->needsRetiring) ? "needs retiring" : ""));

	yaffs2_ClearOldestDirtySequence(dev,bi);

	bi->blockState = YAFFS_BLOCK_STATE_DIRTY;

	/* If this is the block being garbage collected then stop gc'ing this block */
	if(blockNo == dev->gcBlock)
		dev->gcBlock = 0;

	/* If this block is currently the best candidate for gc then drop as a candidate */
	if(blockNo == dev->gcDirtiest){
		dev->gcDirtiest = 0;
		dev->gcPagesInUse = 0;
	}

	if (!bi->needsRetiring) {
		yaffs2_InvalidateCheckpoint(dev);
		erasedOk = yaffs_EraseBlockInNAND(dev, blockNo);
		if (!erasedOk) {
			dev->nErasureFailures++;
			T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
			  (TSTR("**>> Erasure failed %d" TENDSTR), blockNo));
		}
	}

	if (erasedOk &&
	    ((yaffs_traceMask & YAFFS_TRACE_ERASE) || !yaffs_SkipVerification(dev))) {
		int i;
		for (i = 0; i < dev->param.nChunksPerBlock; i++) {
			if (!yaffs_CheckChunkErased
			    (dev, blockNo * dev->param.nChunksPerBlock + i)) {
				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   (">>Block %d erasure supposedly OK, but chunk %d not erased"
				    TENDSTR), blockNo, i));
			}
		}
	}

	if (erasedOk) {
		/* Clean it up... */
		bi->blockState = YAFFS_BLOCK_STATE_EMPTY;
		bi->sequenceNumber = 0;
		dev->nErasedBlocks++;
		bi->pagesInUse = 0;
		bi->softDeletions = 0;
		bi->hasShrinkHeader = 0;
		bi->skipErasedCheck = 1;  /* This is clean, so no need to check */
		bi->gcPrioritise = 0;
		yaffs_ClearChunkBits(dev, blockNo);

		T(YAFFS_TRACE_ERASE,
		  (TSTR("Erased block %d" TENDSTR), blockNo));
	} else {
		dev->nFreeChunks -= dev->param.nChunksPerBlock;	/* We lost a block of free space */

		yaffs_RetireBlock(dev, blockNo);
		T(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
		  (TSTR("**>> Block %d retired" TENDSTR), blockNo));
	}
}

static int yaffs_FindBlockForAllocation(yaffs_Device *dev)
{
	int i;

	yaffs_BlockInfo *bi;

	if (dev->nErasedBlocks < 1) {
		/* Hoosterman we've got a problem.
		 * Can't get space to gc
		 */
		T(YAFFS_TRACE_ERROR,
		  (TSTR("yaffs tragedy: no more erased blocks" TENDSTR)));

		return -1;
	}

	/* Find an empty block. */

	for (i = dev->internalStartBlock; i <= dev->internalEndBlock; i++) {
		dev->allocationBlockFinder++;
		if (dev->allocationBlockFinder < dev->internalStartBlock
		    || dev->allocationBlockFinder > dev->internalEndBlock) {
			dev->allocationBlockFinder = dev->internalStartBlock;
		}

		bi = yaffs_GetBlockInfo(dev, dev->allocationBlockFinder);

		if (bi->blockState == YAFFS_BLOCK_STATE_EMPTY) {
			bi->blockState = YAFFS_BLOCK_STATE_ALLOCATING;
			dev->sequenceNumber++;
			bi->sequenceNumber = dev->sequenceNumber;
			dev->nErasedBlocks--;
			T(YAFFS_TRACE_ALLOCATE,
			  (TSTR("Allocated block %d, seq  %d, %d left" TENDSTR),
			   dev->allocationBlockFinder, dev->sequenceNumber,
			   dev->nErasedBlocks));
			return dev->allocationBlockFinder;
		}
	}

	T(YAFFS_TRACE_ALWAYS,
	  (TSTR
	   ("yaffs tragedy: no more erased blocks, but there should have been %d"
	    TENDSTR), dev->nErasedBlocks));

	return -1;
}
=======
	if (chunk_id > YAFFS_MAX_CHUNK_ID)
		return NULL;

	/* First check we're tall enough (ie enough top_level) */

	i = chunk_id >> YAFFS_TNODES_LEVEL0_BITS;
	required_depth = 0;
	while (i) {
		i >>= YAFFS_TNODES_INTERNAL_BITS;
		required_depth++;
	}

	if (required_depth > file_struct->top_level)
		return NULL;	/* Not tall enough, so we can't find it */

	/* Traverse down to level 0 */
	while (level > 0 && tn) {
		tn = tn->internal[(chunk_id >>
				   (YAFFS_TNODES_LEVEL0_BITS +
				    (level - 1) *
				    YAFFS_TNODES_INTERNAL_BITS)) &
				  YAFFS_TNODES_INTERNAL_MASK];
		level--;
	}

	return tn;
}

/* add_find_tnode_0 finds the level 0 tnode if it exists,
 * otherwise first expands the tree.
 * This happens in two steps:
 *  1. If the tree isn't tall enough, then make it taller.
 *  2. Scan down the tree towards the level 0 tnode adding tnodes if required.
 *
 * Used when modifying the tree.
 *
 *  If the tn argument is NULL, then a fresh tnode will be added otherwise the
 *  specified tn will be plugged into the ttree.
 */

struct yaffs_tnode *yaffs_add_find_tnode_0(struct yaffs_dev *dev,
					   struct yaffs_file_var *file_struct,
					   u32 chunk_id,
					   struct yaffs_tnode *passed_tn)
{
	int required_depth;
	int i;
	int l;
	struct yaffs_tnode *tn;
	u32 x;

	/* Check sane level and page Id */
	if (file_struct->top_level < 0 ||
	    file_struct->top_level > YAFFS_TNODES_MAX_LEVEL)
		return NULL;

	if (chunk_id > YAFFS_MAX_CHUNK_ID)
		return NULL;

	/* First check we're tall enough (ie enough top_level) */

	x = chunk_id >> YAFFS_TNODES_LEVEL0_BITS;
	required_depth = 0;
	while (x) {
		x >>= YAFFS_TNODES_INTERNAL_BITS;
		required_depth++;
	}

	if (required_depth > file_struct->top_level) {
		/* Not tall enough, gotta make the tree taller */
		for (i = file_struct->top_level; i < required_depth; i++) {

			tn = yaffs_get_tnode(dev);

			if (tn) {
				tn->internal[0] = file_struct->top;
				file_struct->top = tn;
				file_struct->top_level++;
			} else {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"yaffs: no more tnodes");
				return NULL;
			}
		}
	}

	/* Traverse down to level 0, adding anything we need */

	l = file_struct->top_level;
	tn = file_struct->top;

	if (l > 0) {
		while (l > 0 && tn) {
			x = (chunk_id >>
			     (YAFFS_TNODES_LEVEL0_BITS +
			      (l - 1) * YAFFS_TNODES_INTERNAL_BITS)) &
			    YAFFS_TNODES_INTERNAL_MASK;

			if ((l > 1) && !tn->internal[x]) {
				/* Add missing non-level-zero tnode */
				tn->internal[x] = yaffs_get_tnode(dev);
				if (!tn->internal[x])
					return NULL;
			} else if (l == 1) {
				/* Looking from level 1 at level 0 */
				if (passed_tn) {
					/* If we already have one, release it */
					if (tn->internal[x])
						yaffs_free_tnode(dev,
							tn->internal[x]);
					tn->internal[x] = passed_tn;

				} else if (!tn->internal[x]) {
					/* Don't have one, none passed in */
					tn->internal[x] = yaffs_get_tnode(dev);
					if (!tn->internal[x])
						return NULL;
				}
			}

			tn = tn->internal[x];
			l--;
		}
	} else {
		/* top is level 0 */
		if (passed_tn) {
			memcpy(tn, passed_tn,
			       (dev->tnode_width * YAFFS_NTNODES_LEVEL0) / 8);
			yaffs_free_tnode(dev, passed_tn);
		}
	}

	return tn;
}

static int yaffs_tags_match(const struct yaffs_ext_tags *tags, int obj_id,
			    int chunk_obj)
{
	return (tags->chunk_id == chunk_obj &&
		tags->obj_id == obj_id &&
		!tags->is_deleted) ? 1 : 0;

}

static int yaffs_find_chunk_in_group(struct yaffs_dev *dev, int the_chunk,
					struct yaffs_ext_tags *tags, int obj_id,
					int inode_chunk)
{
	int j;

	for (j = 0; the_chunk && j < dev->chunk_grp_size; j++) {
		if (yaffs_check_chunk_bit
		    (dev, the_chunk / dev->param.chunks_per_block,
		     the_chunk % dev->param.chunks_per_block)) {

			if (dev->chunk_grp_size == 1)
				return the_chunk;
			else {
				yaffs_rd_chunk_tags_nand(dev, the_chunk, NULL,
							 tags);
				if (yaffs_tags_match(tags,
							obj_id, inode_chunk)) {
					/* found it; */
					return the_chunk;
				}
			}
		}
		the_chunk++;
	}
	return -1;
}

static int yaffs_find_chunk_in_file(struct yaffs_obj *in, int inode_chunk,
				    struct yaffs_ext_tags *tags)
{
	/*Get the Tnode, then get the level 0 offset chunk offset */
	struct yaffs_tnode *tn;
	int the_chunk = -1;
	struct yaffs_ext_tags local_tags;
	int ret_val = -1;
	struct yaffs_dev *dev = in->my_dev;

	if (!tags) {
		/* Passed a NULL, so use our own tags space */
		tags = &local_tags;
	}

	tn = yaffs_find_tnode_0(dev, &in->variant.file_variant, inode_chunk);

	if (!tn)
		return ret_val;

	the_chunk = yaffs_get_group_base(dev, tn, inode_chunk);

	ret_val = yaffs_find_chunk_in_group(dev, the_chunk, tags, in->obj_id,
					      inode_chunk);
	return ret_val;
}

static int yaffs_find_del_file_chunk(struct yaffs_obj *in, int inode_chunk,
				     struct yaffs_ext_tags *tags)
{
	/* Get the Tnode, then get the level 0 offset chunk offset */
	struct yaffs_tnode *tn;
	int the_chunk = -1;
	struct yaffs_ext_tags local_tags;
	struct yaffs_dev *dev = in->my_dev;
	int ret_val = -1;

	if (!tags) {
		/* Passed a NULL, so use our own tags space */
		tags = &local_tags;
	}

	tn = yaffs_find_tnode_0(dev, &in->variant.file_variant, inode_chunk);

	if (!tn)
		return ret_val;

	the_chunk = yaffs_get_group_base(dev, tn, inode_chunk);

	ret_val = yaffs_find_chunk_in_group(dev, the_chunk, tags, in->obj_id,
					      inode_chunk);

	/* Delete the entry in the filestructure (if found) */
	if (ret_val != -1)
		yaffs_load_tnode_0(dev, tn, inode_chunk, 0);

	return ret_val;
}

int yaffs_put_chunk_in_file(struct yaffs_obj *in, int inode_chunk,
			    int nand_chunk, int in_scan)
{
	/* NB in_scan is zero unless scanning.
	 * For forward scanning, in_scan is > 0;
	 * for backward scanning in_scan is < 0
	 *
	 * nand_chunk = 0 is a dummy insert to make sure the tnodes are there.
	 */

	struct yaffs_tnode *tn;
	struct yaffs_dev *dev = in->my_dev;
	int existing_cunk;
	struct yaffs_ext_tags existing_tags;
	struct yaffs_ext_tags new_tags;
	unsigned existing_serial, new_serial;

	if (in->variant_type != YAFFS_OBJECT_TYPE_FILE) {
		/* Just ignore an attempt at putting a chunk into a non-file
		 * during scanning.
		 * If it is not during Scanning then something went wrong!
		 */
		if (!in_scan) {
			yaffs_trace(YAFFS_TRACE_ERROR,
				"yaffs tragedy:attempt to put data chunk into a non-file"
				);
			BUG();
		}

		yaffs_chunk_del(dev, nand_chunk, 1, __LINE__);
		return YAFFS_OK;
	}

	tn = yaffs_add_find_tnode_0(dev,
				    &in->variant.file_variant,
				    inode_chunk, NULL);
	if (!tn)
		return YAFFS_FAIL;

	if (!nand_chunk)
		/* Dummy insert, bail now */
		return YAFFS_OK;

	existing_cunk = yaffs_get_group_base(dev, tn, inode_chunk);

	if (in_scan != 0) {
		/* If we're scanning then we need to test for duplicates
		 * NB This does not need to be efficient since it should only
		 * happen when the power fails during a write, then only one
		 * chunk should ever be affected.
		 *
		 * Correction for YAFFS2: This could happen quite a lot and we
		 * need to think about efficiency! TODO
		 * Update: For backward scanning we don't need to re-read tags
		 * so this is quite cheap.
		 */

		if (existing_cunk > 0) {
			/* NB Right now existing chunk will not be real
			 * chunk_id if the chunk group size > 1
			 * thus we have to do a FindChunkInFile to get the
			 * real chunk id.
			 *
			 * We have a duplicate now we need to decide which
			 * one to use:
			 *
			 * Backwards scanning YAFFS2: The old one is what
			 * we use, dump the new one.
			 * YAFFS1: Get both sets of tags and compare serial
			 * numbers.
			 */

			if (in_scan > 0) {
				/* Only do this for forward scanning */
				yaffs_rd_chunk_tags_nand(dev,
							 nand_chunk,
							 NULL, &new_tags);

				/* Do a proper find */
				existing_cunk =
				    yaffs_find_chunk_in_file(in, inode_chunk,
							     &existing_tags);
			}

			if (existing_cunk <= 0) {
				/*Hoosterman - how did this happen? */

				yaffs_trace(YAFFS_TRACE_ERROR,
					"yaffs tragedy: existing chunk < 0 in scan"
					);

			}

			/* NB The deleted flags should be false, otherwise
			 * the chunks will not be loaded during a scan
			 */

			if (in_scan > 0) {
				new_serial = new_tags.serial_number;
				existing_serial = existing_tags.serial_number;
			}

			if ((in_scan > 0) &&
			    (existing_cunk <= 0 ||
			     ((existing_serial + 1) & 3) == new_serial)) {
				/* Forward scanning.
				 * Use new
				 * Delete the old one and drop through to
				 * update the tnode
				 */
				yaffs_chunk_del(dev, existing_cunk, 1,
						__LINE__);
			} else {
				/* Backward scanning or we want to use the
				 * existing one
				 * Delete the new one and return early so that
				 * the tnode isn't changed
				 */
				yaffs_chunk_del(dev, nand_chunk, 1, __LINE__);
				return YAFFS_OK;
			}
		}

	}

	if (existing_cunk == 0)
		in->n_data_chunks++;

	yaffs_load_tnode_0(dev, tn, inode_chunk, nand_chunk);

	return YAFFS_OK;
}

static void yaffs_soft_del_chunk(struct yaffs_dev *dev, int chunk)
{
	struct yaffs_block_info *the_block;
	unsigned block_no;

	yaffs_trace(YAFFS_TRACE_DELETION, "soft delete chunk %d", chunk);

	block_no = chunk / dev->param.chunks_per_block;
	the_block = yaffs_get_block_info(dev, block_no);
	if (the_block) {
		the_block->soft_del_pages++;
		dev->n_free_chunks++;
		yaffs2_update_oldest_dirty_seq(dev, block_no, the_block);
	}
}

/* SoftDeleteWorker scans backwards through the tnode tree and soft deletes all
 * the chunks in the file.
 * All soft deleting does is increment the block's softdelete count and pulls
 * the chunk out of the tnode.
 * Thus, essentially this is the same as DeleteWorker except that the chunks
 * are soft deleted.
 */

static int yaffs_soft_del_worker(struct yaffs_obj *in, struct yaffs_tnode *tn,
				 u32 level, int chunk_offset)
{
	int i;
	int the_chunk;
	int all_done = 1;
	struct yaffs_dev *dev = in->my_dev;

	if (!tn)
		return 1;

	if (level > 0) {
		for (i = YAFFS_NTNODES_INTERNAL - 1;
			all_done && i >= 0;
			i--) {
			if (tn->internal[i]) {
				all_done =
				    yaffs_soft_del_worker(in,
					tn->internal[i],
					level - 1,
					(chunk_offset <<
					YAFFS_TNODES_INTERNAL_BITS)
					+ i);
				if (all_done) {
					yaffs_free_tnode(dev,
						tn->internal[i]);
					tn->internal[i] = NULL;
				} else {
					/* Can this happen? */
				}
			}
		}
		return (all_done) ? 1 : 0;
	}

	/* level 0 */
	 for (i = YAFFS_NTNODES_LEVEL0 - 1; i >= 0; i--) {
		the_chunk = yaffs_get_group_base(dev, tn, i);
		if (the_chunk) {
			yaffs_soft_del_chunk(dev, the_chunk);
			yaffs_load_tnode_0(dev, tn, i, 0);
		}
	}
	return 1;
}

static void yaffs_remove_obj_from_dir(struct yaffs_obj *obj)
{
	struct yaffs_dev *dev = obj->my_dev;
	struct yaffs_obj *parent;

	yaffs_verify_obj_in_dir(obj);
	parent = obj->parent;

	yaffs_verify_dir(parent);

	if (dev && dev->param.remove_obj_fn)
		dev->param.remove_obj_fn(obj);

	list_del_init(&obj->siblings);
	obj->parent = NULL;

	yaffs_verify_dir(parent);
}

void yaffs_add_obj_to_dir(struct yaffs_obj *directory, struct yaffs_obj *obj)
{
	if (!directory) {
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"tragedy: Trying to add an object to a null pointer directory"
			);
		BUG();
		return;
	}
	if (directory->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"tragedy: Trying to add an object to a non-directory"
			);
		BUG();
	}

	if (obj->siblings.prev == NULL) {
		/* Not initialised */
		BUG();
	}

	yaffs_verify_dir(directory);

	yaffs_remove_obj_from_dir(obj);

	/* Now add it */
	list_add(&obj->siblings, &directory->variant.dir_variant.children);
	obj->parent = directory;

	if (directory == obj->my_dev->unlinked_dir
	    || directory == obj->my_dev->del_dir) {
		obj->unlinked = 1;
		obj->my_dev->n_unlinked_files++;
		obj->rename_allowed = 0;
	}

	yaffs_verify_dir(directory);
	yaffs_verify_obj_in_dir(obj);
}

static int yaffs_change_obj_name(struct yaffs_obj *obj,
				 struct yaffs_obj *new_dir,
				 const YCHAR *new_name, int force, int shadows)
{
	int unlink_op;
	int del_op;
	struct yaffs_obj *existing_target;

	if (new_dir == NULL)
		new_dir = obj->parent;	/* use the old directory */

	if (new_dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"tragedy: yaffs_change_obj_name: new_dir is not a directory"
			);
		BUG();
	}

	unlink_op = (new_dir == obj->my_dev->unlinked_dir);
	del_op = (new_dir == obj->my_dev->del_dir);

	existing_target = yaffs_find_by_name(new_dir, new_name);

	/* If the object is a file going into the unlinked directory,
	 *   then it is OK to just stuff it in since duplicate names are OK.
	 *   else only proceed if the new name does not exist and we're putting
	 *   it into a directory.
	 */
	if (!(unlink_op || del_op || force ||
	      shadows > 0 || !existing_target) ||
	      new_dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
		return YAFFS_FAIL;

	yaffs_set_obj_name(obj, new_name);
	obj->dirty = 1;
	yaffs_add_obj_to_dir(new_dir, obj);

	if (unlink_op)
		obj->unlinked = 1;

	/* If it is a deletion then we mark it as a shrink for gc  */
	if (yaffs_update_oh(obj, new_name, 0, del_op, shadows, NULL) >= 0)
		return YAFFS_OK;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	return YAFFS_FAIL;
}

<<<<<<< HEAD
/*
 * Check if there's space to allocate...
 * Thinks.... do we need top make this ths same as yaffs_GetFreeChunks()?
 */
int yaffs_CheckSpaceForAllocation(yaffs_Device *dev, int nChunks)
{
	int reservedChunks;
	int reservedBlocks = dev->param.nReservedBlocks;
	int checkpointBlocks;

	checkpointBlocks = yaffs2_CalcCheckpointBlocksRequired(dev);

	reservedChunks = ((reservedBlocks + checkpointBlocks) * dev->param.nChunksPerBlock);

	return (dev->nFreeChunks > (reservedChunks + nChunks));
}

static int yaffs_AllocateChunk(yaffs_Device *dev, int useReserve,
		yaffs_BlockInfo **blockUsedPtr)
{
	int retVal;
	yaffs_BlockInfo *bi;

	if (dev->allocationBlock < 0) {
		/* Get next block to allocate off */
		dev->allocationBlock = yaffs_FindBlockForAllocation(dev);
		dev->allocationPage = 0;
	}

	if (!useReserve && !yaffs_CheckSpaceForAllocation(dev, 1)) {
		/* Not enough space to allocate unless we're allowed to use the reserve. */
		return -1;
	}

	if (dev->nErasedBlocks < dev->param.nReservedBlocks
			&& dev->allocationPage == 0) {
		T(YAFFS_TRACE_ALLOCATE, (TSTR("Allocating reserve" TENDSTR)));
	}

	/* Next page please.... */
	if (dev->allocationBlock >= 0) {
		bi = yaffs_GetBlockInfo(dev, dev->allocationBlock);

		retVal = (dev->allocationBlock * dev->param.nChunksPerBlock) +
			dev->allocationPage;
		bi->pagesInUse++;
		yaffs_SetChunkBit(dev, dev->allocationBlock,
				dev->allocationPage);

		dev->allocationPage++;

		dev->nFreeChunks--;

		/* If the block is full set the state to full */
		if (dev->allocationPage >= dev->param.nChunksPerBlock) {
			bi->blockState = YAFFS_BLOCK_STATE_FULL;
			dev->allocationBlock = -1;
		}

		if (blockUsedPtr)
			*blockUsedPtr = bi;

		return retVal;
	}

	T(YAFFS_TRACE_ERROR,
			(TSTR("!!!!!!!!! Allocator out !!!!!!!!!!!!!!!!!" TENDSTR)));

	return -1;
}

static int yaffs_GetErasedChunks(yaffs_Device *dev)
{
	int n;

	n = dev->nErasedBlocks * dev->param.nChunksPerBlock;

	if (dev->allocationBlock > 0)
		n += (dev->param.nChunksPerBlock - dev->allocationPage);

	return n;

}

/*
 * yaffs_SkipRestOfBlock() skips over the rest of the allocation block
 * if we don't want to write to it.
 */
void yaffs_SkipRestOfBlock(yaffs_Device *dev)
{
	if(dev->allocationBlock > 0){
		yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, dev->allocationBlock);
		if(bi->blockState == YAFFS_BLOCK_STATE_ALLOCATING){
			bi->blockState = YAFFS_BLOCK_STATE_FULL;
			dev->allocationBlock = -1;
		}
	}
}


static int yaffs_GarbageCollectBlock(yaffs_Device *dev, int block,
		int wholeBlock)
{
	int oldChunk;
	int newChunk;
	int markNAND;
	int retVal = YAFFS_OK;
	int i;
	int isCheckpointBlock;
	int matchingChunk;
	int maxCopies;

	int chunksBefore = yaffs_GetErasedChunks(dev);
	int chunksAfter;

	yaffs_ExtendedTags tags;

	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, block);

	yaffs_Object *object;

	isCheckpointBlock = (bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT);


	T(YAFFS_TRACE_TRACING,
			(TSTR("Collecting block %d, in use %d, shrink %d, wholeBlock %d" TENDSTR),
			 block,
			 bi->pagesInUse,
			 bi->hasShrinkHeader,
			 wholeBlock));

	/*yaffs_VerifyFreeChunks(dev); */

	if(bi->blockState == YAFFS_BLOCK_STATE_FULL)
		bi->blockState = YAFFS_BLOCK_STATE_COLLECTING;
	
	bi->hasShrinkHeader = 0;	/* clear the flag so that the block can erase */

	dev->gcDisable = 1;

	if (isCheckpointBlock ||
			!yaffs_StillSomeChunkBits(dev, block)) {
		T(YAFFS_TRACE_TRACING,
				(TSTR
				 ("Collecting block %d that has no chunks in use" TENDSTR),
				 block));
		yaffs_BlockBecameDirty(dev, block);
	} else {

		__u8 *buffer = yaffs_GetTempBuffer(dev, __LINE__);

		yaffs_VerifyBlock(dev, bi, block);

		maxCopies = (wholeBlock) ? dev->param.nChunksPerBlock : 5;
		oldChunk = block * dev->param.nChunksPerBlock + dev->gcChunk;

		for (/* init already done */;
		     retVal == YAFFS_OK &&
		     dev->gcChunk < dev->param.nChunksPerBlock &&
		     (bi->blockState == YAFFS_BLOCK_STATE_COLLECTING) &&
		     maxCopies > 0;
		     dev->gcChunk++, oldChunk++) {
			if (yaffs_CheckChunkBit(dev, block, dev->gcChunk)) {

				/* This page is in use and might need to be copied off */

				maxCopies--;

				markNAND = 1;

				yaffs_InitialiseTags(&tags);

				yaffs_ReadChunkWithTagsFromNAND(dev, oldChunk,
								buffer, &tags);

				object =
				    yaffs_FindObjectByNumber(dev,
							     tags.objectId);

				T(YAFFS_TRACE_GC_DETAIL,
				  (TSTR
				   ("Collecting chunk in block %d, %d %d %d " TENDSTR),
				   dev->gcChunk, tags.objectId, tags.chunkId,
				   tags.byteCount));

				if (object && !yaffs_SkipVerification(dev)) {
					if (tags.chunkId == 0)
						matchingChunk = object->hdrChunk;
					else if (object->softDeleted)
						matchingChunk = oldChunk; /* Defeat the test */
					else
						matchingChunk = yaffs_FindChunkInFile(object, tags.chunkId, NULL);

					if (oldChunk != matchingChunk)
						T(YAFFS_TRACE_ERROR,
						  (TSTR("gc: page in gc mismatch: %d %d %d %d"TENDSTR),
						  oldChunk, matchingChunk, tags.objectId, tags.chunkId));

				}

				if (!object) {
					T(YAFFS_TRACE_ERROR,
					  (TSTR
					   ("page %d in gc has no object: %d %d %d "
					    TENDSTR), oldChunk,
					    tags.objectId, tags.chunkId, tags.byteCount));
				}

				if (object &&
				    object->deleted &&
				    object->softDeleted &&
				    tags.chunkId != 0) {
					/* Data chunk in a soft deleted file, throw it away
					 * It's a soft deleted data chunk,
					 * No need to copy this, just forget about it and
					 * fix up the object.
					 */
					 
					/* Free chunks already includes softdeleted chunks.
					 * How ever this chunk is going to soon be really deleted
					 * which will increment free chunks.
					 * We have to decrement free chunks so this works out properly.
					 */
					dev->nFreeChunks--;
					bi->softDeletions--;

					object->nDataChunks--;

					if (object->nDataChunks <= 0) {
						/* remeber to clean up the object */
						dev->gcCleanupList[dev->nCleanups] =
						    tags.objectId;
						dev->nCleanups++;
					}
					markNAND = 0;
				} else if (0) {
					/* Todo object && object->deleted && object->nDataChunks == 0 */
					/* Deleted object header with no data chunks.
					 * Can be discarded and the file deleted.
					 */
					object->hdrChunk = 0;
					yaffs_FreeTnode(object->myDev,
							object->variant.
							fileVariant.top);
					object->variant.fileVariant.top = NULL;
					yaffs_DoGenericObjectDeletion(object);

				} else if (object) {
					/* It's either a data chunk in a live file or
					 * an ObjectHeader, so we're interested in it.
					 * NB Need to keep the ObjectHeaders of deleted files
					 * until the whole file has been deleted off
					 */
					tags.serialNumber++;

					dev->nGCCopies++;

					if (tags.chunkId == 0) {
						/* It is an object Id,
						 * We need to nuke the shrinkheader flags first
						 * Also need to clean up shadowing.
						 * We no longer want the shrinkHeader flag since its work is done
						 * and if it is left in place it will mess up scanning.
						 */

						yaffs_ObjectHeader *oh;
						oh = (yaffs_ObjectHeader *)buffer;

						oh->isShrink = 0;
						tags.extraIsShrinkHeader = 0;

						oh->shadowsObject = 0;
						oh->inbandShadowsObject = 0;
						tags.extraShadows = 0;

						/* Update file size */
						if(object->variantType == YAFFS_OBJECT_TYPE_FILE){
							oh->fileSize = object->variant.fileVariant.fileSize;
							tags.extraFileLength = oh->fileSize;
						}

						yaffs_VerifyObjectHeader(object, oh, &tags, 1);
						newChunk =
						    yaffs_WriteNewChunkWithTagsToNAND(dev,(__u8 *) oh, &tags, 1);
					} else
						newChunk =
						    yaffs_WriteNewChunkWithTagsToNAND(dev, buffer, &tags, 1);

					if (newChunk < 0) {
						retVal = YAFFS_FAIL;
					} else {

						/* Ok, now fix up the Tnodes etc. */

						if (tags.chunkId == 0) {
							/* It's a header */
							object->hdrChunk =  newChunk;
							object->serial =   tags.serialNumber;
						} else {
							/* It's a data chunk */
							int ok;
							ok = yaffs_PutChunkIntoFile
							    (object,
							     tags.chunkId,
							     newChunk, 0);
						}
					}
				}

				if (retVal == YAFFS_OK)
					yaffs_DeleteChunk(dev, oldChunk, markNAND, __LINE__);

			}
		}

		yaffs_ReleaseTempBuffer(dev, buffer, __LINE__);



	}

	yaffs_VerifyCollectedBlock(dev, bi, block);



	if (bi->blockState == YAFFS_BLOCK_STATE_COLLECTING) {
		/*
		 * The gc did not complete. Set block state back to FULL
		 * because checkpointing does not restore gc.
		 */
		bi->blockState = YAFFS_BLOCK_STATE_FULL;
	} else {
		/* The gc completed. */
		/* Do any required cleanups */
		for (i = 0; i < dev->nCleanups; i++) {
			/* Time to delete the file too */
			object =
			    yaffs_FindObjectByNumber(dev,
						     dev->gcCleanupList[i]);
			if (object) {
				yaffs_FreeTnode(dev,
						object->variant.fileVariant.
						top);
				object->variant.fileVariant.top = NULL;
				T(YAFFS_TRACE_GC,
				  (TSTR
				   ("yaffs: About to finally delete object %d"
				    TENDSTR), object->objectId));
				yaffs_DoGenericObjectDeletion(object);
				object->myDev->nDeletedFiles--;
			}

		}


		chunksAfter = yaffs_GetErasedChunks(dev);
		if (chunksBefore >= chunksAfter) {
			T(YAFFS_TRACE_GC,
			  (TSTR
			   ("gc did not increase free chunks before %d after %d"
			    TENDSTR), chunksBefore, chunksAfter));
		}
		dev->gcBlock = 0;
		dev->gcChunk = 0;
		dev->nCleanups = 0;
	}

	dev->gcDisable = 0;

	return retVal;
}

/*
 * FindBlockForgarbageCollection is used to select the dirtiest block (or close enough)
 * for garbage collection.
 */

static unsigned yaffs_FindBlockForGarbageCollection(yaffs_Device *dev,
					int aggressive,
					int background)
{
	int i;
	int iterations;
	unsigned selected = 0;
	int prioritised = 0;
	int prioritisedExists = 0;
	yaffs_BlockInfo *bi;
	int threshold;

	/* First let's see if we need to grab a prioritised block */
	if (dev->hasPendingPrioritisedGCs && !aggressive) {
		dev->gcDirtiest = 0;
		bi = dev->blockInfo;
		for (i = dev->internalStartBlock;
			i <= dev->internalEndBlock && !selected;
			i++) {

			if (bi->gcPrioritise) {
				prioritisedExists = 1;
				if (bi->blockState == YAFFS_BLOCK_STATE_FULL &&
				   yaffs2_BlockNotDisqualifiedFromGC(dev, bi)) {
					selected = i;
					prioritised = 1;
				}
			}
			bi++;
		}

		/*
		 * If there is a prioritised block and none was selected then
		 * this happened because there is at least one old dirty block gumming
		 * up the works. Let's gc the oldest dirty block.
		 */

		if(prioritisedExists &&
			!selected &&
			dev->oldestDirtyBlock > 0)
			selected = dev->oldestDirtyBlock;

		if (!prioritisedExists) /* None found, so we can clear this */
			dev->hasPendingPrioritisedGCs = 0;
	}

	/* If we're doing aggressive GC then we are happy to take a less-dirty block, and
	 * search harder.
	 * else (we're doing a leasurely gc), then we only bother to do this if the
	 * block has only a few pages in use.
	 */

	if (!selected){
		int pagesUsed;
		int nBlocks = dev->internalEndBlock - dev->internalStartBlock + 1;
		if (aggressive){
			threshold = dev->param.nChunksPerBlock;
			iterations = nBlocks;
		} else {
			int maxThreshold;

			if(background)
				maxThreshold = dev->param.nChunksPerBlock/2;
			else
				maxThreshold = dev->param.nChunksPerBlock/8;

			if(maxThreshold <  YAFFS_GC_PASSIVE_THRESHOLD)
				maxThreshold = YAFFS_GC_PASSIVE_THRESHOLD;

			threshold = background ?
				(dev->gcNotDone + 2) * 2 : 0;
			if(threshold <YAFFS_GC_PASSIVE_THRESHOLD)
				threshold = YAFFS_GC_PASSIVE_THRESHOLD;
			if(threshold > maxThreshold)
				threshold = maxThreshold;

			iterations = nBlocks / 16 + 1;
			if (iterations > 100)
				iterations = 100;
		}

		for (i = 0;
			i < iterations &&
			(dev->gcDirtiest < 1 ||
				dev->gcPagesInUse > YAFFS_GC_GOOD_ENOUGH);
			i++) {
			dev->gcBlockFinder++;
			if (dev->gcBlockFinder < dev->internalStartBlock ||
				dev->gcBlockFinder > dev->internalEndBlock)
				dev->gcBlockFinder = dev->internalStartBlock;

			bi = yaffs_GetBlockInfo(dev, dev->gcBlockFinder);

			pagesUsed = bi->pagesInUse - bi->softDeletions;

			if (bi->blockState == YAFFS_BLOCK_STATE_FULL &&
				pagesUsed < dev->param.nChunksPerBlock &&
				(dev->gcDirtiest < 1 || pagesUsed < dev->gcPagesInUse) &&
				yaffs2_BlockNotDisqualifiedFromGC(dev, bi)) {
				dev->gcDirtiest = dev->gcBlockFinder;
				dev->gcPagesInUse = pagesUsed;
			}
		}

		if(dev->gcDirtiest > 0 && dev->gcPagesInUse <= threshold)
			selected = dev->gcDirtiest;
	}

	/*
	 * If nothing has been selected for a while, try selecting the oldest dirty
	 * because that's gumming up the works.
	 */

	if(!selected && dev->param.isYaffs2 &&
		dev->gcNotDone >= ( background ? 10 : 20)){
		yaffs2_FindOldestDirtySequence(dev);
		if(dev->oldestDirtyBlock > 0) {
			selected = dev->oldestDirtyBlock;
			dev->gcDirtiest = selected;
			dev->oldestDirtyGCs++;
			bi = yaffs_GetBlockInfo(dev, selected);
			dev->gcPagesInUse =  bi->pagesInUse - bi->softDeletions;
		} else
			dev->gcNotDone = 0;
	}

	if(selected){
		T(YAFFS_TRACE_GC,
		  (TSTR("GC Selected block %d with %d free, prioritised:%d" TENDSTR),
		  selected,
		  dev->param.nChunksPerBlock - dev->gcPagesInUse,
		  prioritised));

		dev->nGCBlocks++;
		if(background)
			dev->backgroundGCs++;

		dev->gcDirtiest = 0;
		dev->gcPagesInUse = 0;
		dev->gcNotDone = 0;
		if(dev->refreshSkip > 0)
			dev->refreshSkip--;
	} else{
		dev->gcNotDone++;
		T(YAFFS_TRACE_GC,
		  (TSTR("GC none: finder %d skip %d threshold %d dirtiest %d using %d oldest %d%s" TENDSTR),
		  dev->gcBlockFinder, dev->gcNotDone,
		  threshold,
		  dev->gcDirtiest, dev->gcPagesInUse,
		  dev->oldestDirtyBlock,
		  background ? " bg" : ""));
	}

	return selected;
}

/* New garbage collector
 * If we're very low on erased blocks then we do aggressive garbage collection
 * otherwise we do "leasurely" garbage collection.
 * Aggressive gc looks further (whole array) and will accept less dirty blocks.
 * Passive gc only inspects smaller areas and will only accept more dirty blocks.
 *
 * The idea is to help clear out space in a more spread-out manner.
 * Dunno if it really does anything useful.
 */
static int yaffs_CheckGarbageCollection(yaffs_Device *dev, int background)
{
	int aggressive = 0;
	int gcOk = YAFFS_OK;
	int maxTries = 0;
	int minErased;
	int erasedChunks;
	int checkpointBlockAdjust;

	if(dev->param.gcControl &&
		(dev->param.gcControl(dev) & 1) == 0)
		return YAFFS_OK;

	if (dev->gcDisable) {
		/* Bail out so we don't get recursive gc */
		return YAFFS_OK;
	}

	/* This loop should pass the first time.
	 * We'll only see looping here if the collection does not increase space.
	 */

	do {
		maxTries++;

		checkpointBlockAdjust = yaffs2_CalcCheckpointBlocksRequired(dev);

		minErased  = dev->param.nReservedBlocks + checkpointBlockAdjust + 1;
		erasedChunks = dev->nErasedBlocks * dev->param.nChunksPerBlock;

		/* If we need a block soon then do aggressive gc.*/
		if (dev->nErasedBlocks < minErased)
			aggressive = 1;
		else {
			if(!background && erasedChunks > (dev->nFreeChunks / 4))
				break;

			if(dev->gcSkip > 20)
				dev->gcSkip = 20;
			if(erasedChunks < dev->nFreeChunks/2 ||
				dev->gcSkip < 1 ||
				background)
				aggressive = 0;
			else {
				dev->gcSkip--;
				break;
			}
		}

		dev->gcSkip = 5;

                /* If we don't already have a block being gc'd then see if we should start another */

		if (dev->gcBlock < 1 && !aggressive) {
			dev->gcBlock = yaffs2_FindRefreshBlock(dev);
			dev->gcChunk = 0;
			dev->nCleanups=0;
		}
		if (dev->gcBlock < 1) {
			dev->gcBlock = yaffs_FindBlockForGarbageCollection(dev, aggressive, background);
			dev->gcChunk = 0;
			dev->nCleanups=0;
		}

		if (dev->gcBlock > 0) {
			dev->allGCs++;
			if (!aggressive)
				dev->passiveGCs++;

			T(YAFFS_TRACE_GC,
			  (TSTR
			   ("yaffs: GC erasedBlocks %d aggressive %d" TENDSTR),
			   dev->nErasedBlocks, aggressive));

			gcOk = yaffs_GarbageCollectBlock(dev, dev->gcBlock, aggressive);
		}

		if (dev->nErasedBlocks < (dev->param.nReservedBlocks) && dev->gcBlock > 0) {
			T(YAFFS_TRACE_GC,
			  (TSTR
			   ("yaffs: GC !!!no reclaim!!! erasedBlocks %d after try %d block %d"
			    TENDSTR), dev->nErasedBlocks, maxTries, dev->gcBlock));
		}
	} while ((dev->nErasedBlocks < dev->param.nReservedBlocks) &&
		 (dev->gcBlock > 0) &&
		 (maxTries < 2));

	return aggressive ? gcOk : YAFFS_OK;
}

/*
 * yaffs_BackgroundGarbageCollect()
 * Garbage collects. Intended to be called from a background thread.
 * Returns non-zero if at least half the free chunks are erased.
 */
int yaffs_BackgroundGarbageCollect(yaffs_Device *dev, unsigned urgency)
{
	int erasedChunks = dev->nErasedBlocks * dev->param.nChunksPerBlock;

	T(YAFFS_TRACE_BACKGROUND, (TSTR("Background gc %u" TENDSTR),urgency));

	yaffs_CheckGarbageCollection(dev, 1);
	return erasedChunks > dev->nFreeChunks/2;
}

/*-------------------------  TAGS --------------------------------*/

static int yaffs_TagsMatch(const yaffs_ExtendedTags *tags, int objectId,
			   int chunkInObject)
{
	return (tags->chunkId == chunkInObject &&
		tags->objectId == objectId && !tags->chunkDeleted) ? 1 : 0;

}


/*-------------------- Data file manipulation -----------------*/

static int yaffs_FindChunkInFile(yaffs_Object *in, int chunkInInode,
				 yaffs_ExtendedTags *tags)
{
	/*Get the Tnode, then get the level 0 offset chunk offset */
	yaffs_Tnode *tn;
	int theChunk = -1;
	yaffs_ExtendedTags localTags;
	int retVal = -1;

	yaffs_Device *dev = in->myDev;

	if (!tags) {
		/* Passed a NULL, so use our own tags space */
		tags = &localTags;
	}

	tn = yaffs_FindLevel0Tnode(dev, &in->variant.fileVariant, chunkInInode);

	if (tn) {
		theChunk = yaffs_GetChunkGroupBase(dev, tn, chunkInInode);

		retVal =
		    yaffs_FindChunkInGroup(dev, theChunk, tags, in->objectId,
					   chunkInInode);
	}
	return retVal;
}

static int yaffs_FindAndDeleteChunkInFile(yaffs_Object *in, int chunkInInode,
					  yaffs_ExtendedTags *tags)
{
	/* Get the Tnode, then get the level 0 offset chunk offset */
	yaffs_Tnode *tn;
	int theChunk = -1;
	yaffs_ExtendedTags localTags;

	yaffs_Device *dev = in->myDev;
	int retVal = -1;

	if (!tags) {
		/* Passed a NULL, so use our own tags space */
		tags = &localTags;
	}

	tn = yaffs_FindLevel0Tnode(dev, &in->variant.fileVariant, chunkInInode);

	if (tn) {

		theChunk = yaffs_GetChunkGroupBase(dev, tn, chunkInInode);

		retVal =
		    yaffs_FindChunkInGroup(dev, theChunk, tags, in->objectId,
					   chunkInInode);

		/* Delete the entry in the filestructure (if found) */
		if (retVal != -1)
			yaffs_LoadLevel0Tnode(dev, tn, chunkInInode, 0);
	}

	return retVal;
}
=======
/*------------------------ Short Operations Cache ------------------------------
 *   In many situations where there is no high level buffering  a lot of
 *   reads might be short sequential reads, and a lot of writes may be short
 *   sequential writes. eg. scanning/writing a jpeg file.
 *   In these cases, a short read/write cache can provide a huge perfomance
 *   benefit with dumb-as-a-rock code.
 *   In Linux, the page cache provides read buffering and the short op cache
 *   provides write buffering.
 *
 *   There are a small number (~10) of cache chunks per device so that we don't
 *   need a very intelligent search.
 */

static int yaffs_obj_cache_dirty(struct yaffs_obj *obj)
{
	struct yaffs_dev *dev = obj->my_dev;
	int i;
	struct yaffs_cache *cache;
	int n_caches = obj->my_dev->param.n_caches;

	for (i = 0; i < n_caches; i++) {
		cache = &dev->cache[i];
		if (cache->object == obj && cache->dirty)
			return 1;
	}

	return 0;
}

static void yaffs_flush_file_cache(struct yaffs_obj *obj)
{
	struct yaffs_dev *dev = obj->my_dev;
	int lowest = -99;	/* Stop compiler whining. */
	int i;
	struct yaffs_cache *cache;
	int chunk_written = 0;
	int n_caches = obj->my_dev->param.n_caches;

	if (n_caches < 1)
		return;
	do {
		cache = NULL;

		/* Find the lowest dirty chunk for this object */
		for (i = 0; i < n_caches; i++) {
			if (dev->cache[i].object == obj &&
			    dev->cache[i].dirty) {
				if (!cache ||
				    dev->cache[i].chunk_id < lowest) {
					cache = &dev->cache[i];
					lowest = cache->chunk_id;
				}
			}
		}

		if (cache && !cache->locked) {
			/* Write it out and free it up */
			chunk_written =
			    yaffs_wr_data_obj(cache->object,
					      cache->chunk_id,
					      cache->data,
					      cache->n_bytes, 1);
			cache->dirty = 0;
			cache->object = NULL;
		}
	} while (cache && chunk_written > 0);

	if (cache)
		/* Hoosterman, disk full while writing cache out. */
		yaffs_trace(YAFFS_TRACE_ERROR,
			"yaffs tragedy: no space during cache write");
}

/*yaffs_flush_whole_cache(dev)
 *
 *
 */

void yaffs_flush_whole_cache(struct yaffs_dev *dev)
{
	struct yaffs_obj *obj;
	int n_caches = dev->param.n_caches;
	int i;

	/* Find a dirty object in the cache and flush it...
	 * until there are no further dirty objects.
	 */
	do {
		obj = NULL;
		for (i = 0; i < n_caches && !obj; i++) {
			if (dev->cache[i].object && dev->cache[i].dirty)
				obj = dev->cache[i].object;
		}
		if (obj)
			yaffs_flush_file_cache(obj);
	} while (obj);

}

/* Grab us a cache chunk for use.
 * First look for an empty one.
 * Then look for the least recently used non-dirty one.
 * Then look for the least recently used dirty one...., flush and look again.
 */
static struct yaffs_cache *yaffs_grab_chunk_worker(struct yaffs_dev *dev)
{
	int i;

	if (dev->param.n_caches > 0) {
		for (i = 0; i < dev->param.n_caches; i++) {
			if (!dev->cache[i].object)
				return &dev->cache[i];
		}
	}
	return NULL;
}

static struct yaffs_cache *yaffs_grab_chunk_cache(struct yaffs_dev *dev)
{
	struct yaffs_cache *cache;
	struct yaffs_obj *the_obj;
	int usage;
	int i;
	int pushout;

	if (dev->param.n_caches < 1)
		return NULL;

	/* Try find a non-dirty one... */

	cache = yaffs_grab_chunk_worker(dev);

	if (!cache) {
		/* They were all dirty, find the LRU object and flush
		 * its cache, then  find again.
		 * NB what's here is not very accurate,
		 * we actually flush the object with the LRU chunk.
		 */

		/* With locking we can't assume we can use entry zero,
		 * Set the_obj to a valid pointer for Coverity. */
		the_obj = dev->cache[0].object;
		usage = -1;
		cache = NULL;
		pushout = -1;

		for (i = 0; i < dev->param.n_caches; i++) {
			if (dev->cache[i].object &&
			    !dev->cache[i].locked &&
			    (dev->cache[i].last_use < usage ||
			    !cache)) {
				usage = dev->cache[i].last_use;
				the_obj = dev->cache[i].object;
				cache = &dev->cache[i];
				pushout = i;
			}
		}

		if (!cache || cache->dirty) {
			/* Flush and try again */
			yaffs_flush_file_cache(the_obj);
			cache = yaffs_grab_chunk_worker(dev);
		}
	}
	return cache;
}

/* Find a cached chunk */
static struct yaffs_cache *yaffs_find_chunk_cache(const struct yaffs_obj *obj,
						  int chunk_id)
{
	struct yaffs_dev *dev = obj->my_dev;
	int i;

	if (dev->param.n_caches < 1)
		return NULL;

	for (i = 0; i < dev->param.n_caches; i++) {
		if (dev->cache[i].object == obj &&
		    dev->cache[i].chunk_id == chunk_id) {
			dev->cache_hits++;

			return &dev->cache[i];
		}
	}
	return NULL;
}

/* Mark the chunk for the least recently used algorithym */
static void yaffs_use_cache(struct yaffs_dev *dev, struct yaffs_cache *cache,
			    int is_write)
{
	int i;

	if (dev->param.n_caches < 1)
		return;

	if (dev->cache_last_use < 0 ||
		dev->cache_last_use > 100000000) {
		/* Reset the cache usages */
		for (i = 1; i < dev->param.n_caches; i++)
			dev->cache[i].last_use = 0;

		dev->cache_last_use = 0;
	}
	dev->cache_last_use++;
	cache->last_use = dev->cache_last_use;

	if (is_write)
		cache->dirty = 1;
}

/* Invalidate a single cache page.
 * Do this when a whole page gets written,
 * ie the short cache for this page is no longer valid.
 */
static void yaffs_invalidate_chunk_cache(struct yaffs_obj *object, int chunk_id)
{
	struct yaffs_cache *cache;

	if (object->my_dev->param.n_caches > 0) {
		cache = yaffs_find_chunk_cache(object, chunk_id);

		if (cache)
			cache->object = NULL;
	}
}

/* Invalidate all the cache pages associated with this object
 * Do this whenever ther file is deleted or resized.
 */
static void yaffs_invalidate_whole_cache(struct yaffs_obj *in)
{
	int i;
	struct yaffs_dev *dev = in->my_dev;

	if (dev->param.n_caches > 0) {
		/* Invalidate it. */
		for (i = 0; i < dev->param.n_caches; i++) {
			if (dev->cache[i].object == in)
				dev->cache[i].object = NULL;
		}
	}
}

static void yaffs_unhash_obj(struct yaffs_obj *obj)
{
	int bucket;
	struct yaffs_dev *dev = obj->my_dev;

	/* If it is still linked into the bucket list, free from the list */
	if (!list_empty(&obj->hash_link)) {
		list_del_init(&obj->hash_link);
		bucket = yaffs_hash_fn(obj->obj_id);
		dev->obj_bucket[bucket].count--;
	}
}

/*  FreeObject frees up a Object and puts it back on the free list */
static void yaffs_free_obj(struct yaffs_obj *obj)
{
	struct yaffs_dev *dev;

	if (!obj) {
		BUG();
		return;
	}
	dev = obj->my_dev;
	yaffs_trace(YAFFS_TRACE_OS, "FreeObject %p inode %p",
		obj, obj->my_inode);
	if (obj->parent)
		BUG();
	if (!list_empty(&obj->siblings))
		BUG();

	if (obj->my_inode) {
		/* We're still hooked up to a cached inode.
		 * Don't delete now, but mark for later deletion
		 */
		obj->defered_free = 1;
		return;
	}

	yaffs_unhash_obj(obj);

	yaffs_free_raw_obj(dev, obj);
	dev->n_obj--;
	dev->checkpoint_blocks_required = 0;	/* force recalculation */
}

void yaffs_handle_defered_free(struct yaffs_obj *obj)
{
	if (obj->defered_free)
		yaffs_free_obj(obj);
}

static int yaffs_generic_obj_del(struct yaffs_obj *in)
{
	/* Iinvalidate the file's data in the cache, without flushing. */
	yaffs_invalidate_whole_cache(in);

	if (in->my_dev->param.is_yaffs2 && in->parent != in->my_dev->del_dir) {
		/* Move to unlinked directory so we have a deletion record */
		yaffs_change_obj_name(in, in->my_dev->del_dir, _Y("deleted"), 0,
				      0);
	}

	yaffs_remove_obj_from_dir(in);
	yaffs_chunk_del(in->my_dev, in->hdr_chunk, 1, __LINE__);
	in->hdr_chunk = 0;

	yaffs_free_obj(in);
	return YAFFS_OK;

}

static void yaffs_soft_del_file(struct yaffs_obj *obj)
{
	if (!obj->deleted ||
	    obj->variant_type != YAFFS_OBJECT_TYPE_FILE ||
	    obj->soft_del)
		return;

	if (obj->n_data_chunks <= 0) {
		/* Empty file with no duplicate object headers,
		 * just delete it immediately */
		yaffs_free_tnode(obj->my_dev, obj->variant.file_variant.top);
		obj->variant.file_variant.top = NULL;
		yaffs_trace(YAFFS_TRACE_TRACING,
			"yaffs: Deleting empty file %d",
			obj->obj_id);
		yaffs_generic_obj_del(obj);
	} else {
		yaffs_soft_del_worker(obj,
				      obj->variant.file_variant.top,
				      obj->variant.
				      file_variant.top_level, 0);
		obj->soft_del = 1;
	}
}

/* Pruning removes any part of the file structure tree that is beyond the
 * bounds of the file (ie that does not point to chunks).
 *
 * A file should only get pruned when its size is reduced.
 *
 * Before pruning, the chunks must be pulled from the tree and the
 * level 0 tnode entries must be zeroed out.
 * Could also use this for file deletion, but that's probably better handled
 * by a special case.
 *
 * This function is recursive. For levels > 0 the function is called again on
 * any sub-tree. For level == 0 we just check if the sub-tree has data.
 * If there is no data in a subtree then it is pruned.
 */

static struct yaffs_tnode *yaffs_prune_worker(struct yaffs_dev *dev,
					      struct yaffs_tnode *tn, u32 level,
					      int del0)
{
	int i;
	int has_data;

	if (!tn)
		return tn;

	has_data = 0;

	if (level > 0) {
		for (i = 0; i < YAFFS_NTNODES_INTERNAL; i++) {
			if (tn->internal[i]) {
				tn->internal[i] =
				    yaffs_prune_worker(dev,
						tn->internal[i],
						level - 1,
						(i == 0) ? del0 : 1);
			}

			if (tn->internal[i])
				has_data++;
		}
	} else {
		int tnode_size_u32 = dev->tnode_size / sizeof(u32);
		u32 *map = (u32 *) tn;

		for (i = 0; !has_data && i < tnode_size_u32; i++) {
			if (map[i])
				has_data++;
		}
	}

	if (has_data == 0 && del0) {
		/* Free and return NULL */
		yaffs_free_tnode(dev, tn);
		tn = NULL;
	}
	return tn;
}

static int yaffs_prune_tree(struct yaffs_dev *dev,
			    struct yaffs_file_var *file_struct)
{
	int i;
	int has_data;
	int done = 0;
	struct yaffs_tnode *tn;

	if (file_struct->top_level < 1)
		return YAFFS_OK;

	file_struct->top =
	   yaffs_prune_worker(dev, file_struct->top, file_struct->top_level, 0);

	/* Now we have a tree with all the non-zero branches NULL but
	 * the height is the same as it was.
	 * Let's see if we can trim internal tnodes to shorten the tree.
	 * We can do this if only the 0th element in the tnode is in use
	 * (ie all the non-zero are NULL)
	 */

	while (file_struct->top_level && !done) {
		tn = file_struct->top;

		has_data = 0;
		for (i = 1; i < YAFFS_NTNODES_INTERNAL; i++) {
			if (tn->internal[i])
				has_data++;
		}

		if (!has_data) {
			file_struct->top = tn->internal[0];
			file_struct->top_level--;
			yaffs_free_tnode(dev, tn);
		} else {
			done = 1;
		}
	}

	return YAFFS_OK;
}

/*-------------------- End of File Structure functions.-------------------*/

/* alloc_empty_obj gets us a clean Object.*/
static struct yaffs_obj *yaffs_alloc_empty_obj(struct yaffs_dev *dev)
{
	struct yaffs_obj *obj = yaffs_alloc_raw_obj(dev);

	if (!obj)
		return obj;

	dev->n_obj++;

	/* Now sweeten it up... */

	memset(obj, 0, sizeof(struct yaffs_obj));
	obj->being_created = 1;

	obj->my_dev = dev;
	obj->hdr_chunk = 0;
	obj->variant_type = YAFFS_OBJECT_TYPE_UNKNOWN;
	INIT_LIST_HEAD(&(obj->hard_links));
	INIT_LIST_HEAD(&(obj->hash_link));
	INIT_LIST_HEAD(&obj->siblings);

	/* Now make the directory sane */
	if (dev->root_dir) {
		obj->parent = dev->root_dir;
		list_add(&(obj->siblings),
			 &dev->root_dir->variant.dir_variant.children);
	}

	/* Add it to the lost and found directory.
	 * NB Can't put root or lost-n-found in lost-n-found so
	 * check if lost-n-found exists first
	 */
	if (dev->lost_n_found)
		yaffs_add_obj_to_dir(dev->lost_n_found, obj);

	obj->being_created = 0;

	dev->checkpoint_blocks_required = 0;	/* force recalculation */

	return obj;
}

static int yaffs_find_nice_bucket(struct yaffs_dev *dev)
{
	int i;
	int l = 999;
	int lowest = 999999;

	/* Search for the shortest list or one that
	 * isn't too long.
	 */

	for (i = 0; i < 10 && lowest > 4; i++) {
		dev->bucket_finder++;
		dev->bucket_finder %= YAFFS_NOBJECT_BUCKETS;
		if (dev->obj_bucket[dev->bucket_finder].count < lowest) {
			lowest = dev->obj_bucket[dev->bucket_finder].count;
			l = dev->bucket_finder;
		}
	}

	return l;
}

static int yaffs_new_obj_id(struct yaffs_dev *dev)
{
	int bucket = yaffs_find_nice_bucket(dev);
	int found = 0;
	struct list_head *i;
	u32 n = (u32) bucket;

	/* Now find an object value that has not already been taken
	 * by scanning the list.
	 */

	while (!found) {
		found = 1;
		n += YAFFS_NOBJECT_BUCKETS;
		if (1 || dev->obj_bucket[bucket].count > 0) {
			list_for_each(i, &dev->obj_bucket[bucket].list) {
				/* If there is already one in the list */
				if (i && list_entry(i, struct yaffs_obj,
						    hash_link)->obj_id == n) {
					found = 0;
				}
			}
		}
	}
	return n;
}

static void yaffs_hash_obj(struct yaffs_obj *in)
{
	int bucket = yaffs_hash_fn(in->obj_id);
	struct yaffs_dev *dev = in->my_dev;

	list_add(&in->hash_link, &dev->obj_bucket[bucket].list);
	dev->obj_bucket[bucket].count++;
}

struct yaffs_obj *yaffs_find_by_number(struct yaffs_dev *dev, u32 number)
{
	int bucket = yaffs_hash_fn(number);
	struct list_head *i;
	struct yaffs_obj *in;

	list_for_each(i, &dev->obj_bucket[bucket].list) {
		/* Look if it is in the list */
		in = list_entry(i, struct yaffs_obj, hash_link);
		if (in->obj_id == number) {
			/* Don't show if it is defered free */
			if (in->defered_free)
				return NULL;
			return in;
		}
	}

	return NULL;
}

struct yaffs_obj *yaffs_new_obj(struct yaffs_dev *dev, int number,
				enum yaffs_obj_type type)
{
	struct yaffs_obj *the_obj = NULL;
	struct yaffs_tnode *tn = NULL;

	if (number < 0)
		number = yaffs_new_obj_id(dev);

	if (type == YAFFS_OBJECT_TYPE_FILE) {
		tn = yaffs_get_tnode(dev);
		if (!tn)
			return NULL;
	}

	the_obj = yaffs_alloc_empty_obj(dev);
	if (!the_obj) {
		if (tn)
			yaffs_free_tnode(dev, tn);
		return NULL;
	}

	the_obj->fake = 0;
	the_obj->rename_allowed = 1;
	the_obj->unlink_allowed = 1;
	the_obj->obj_id = number;
	yaffs_hash_obj(the_obj);
	the_obj->variant_type = type;
	yaffs_load_current_time(the_obj, 1, 1);

	switch (type) {
	case YAFFS_OBJECT_TYPE_FILE:
		the_obj->variant.file_variant.file_size = 0;
		the_obj->variant.file_variant.scanned_size = 0;
		the_obj->variant.file_variant.shrink_size = ~0; /* max */
		the_obj->variant.file_variant.top_level = 0;
		the_obj->variant.file_variant.top = tn;
		break;
	case YAFFS_OBJECT_TYPE_DIRECTORY:
		INIT_LIST_HEAD(&the_obj->variant.dir_variant.children);
		INIT_LIST_HEAD(&the_obj->variant.dir_variant.dirty);
		break;
	case YAFFS_OBJECT_TYPE_SYMLINK:
	case YAFFS_OBJECT_TYPE_HARDLINK:
	case YAFFS_OBJECT_TYPE_SPECIAL:
		/* No action required */
		break;
	case YAFFS_OBJECT_TYPE_UNKNOWN:
		/* todo this should not happen */
		break;
	}
	return the_obj;
}

static struct yaffs_obj *yaffs_create_fake_dir(struct yaffs_dev *dev,
					       int number, u32 mode)
{

	struct yaffs_obj *obj =
	    yaffs_new_obj(dev, number, YAFFS_OBJECT_TYPE_DIRECTORY);

	if (!obj)
		return NULL;

	obj->fake = 1;	/* it is fake so it might not use NAND */
	obj->rename_allowed = 0;
	obj->unlink_allowed = 0;
	obj->deleted = 0;
	obj->unlinked = 0;
	obj->yst_mode = mode;
	obj->my_dev = dev;
	obj->hdr_chunk = 0;	/* Not a valid chunk. */
	return obj;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

}

<<<<<<< HEAD
int yaffs_PutChunkIntoFile(yaffs_Object *in, int chunkInInode,
			        int chunkInNAND, int inScan)
{
	/* NB inScan is zero unless scanning.
	 * For forward scanning, inScan is > 0;
	 * for backward scanning inScan is < 0
	 *
	 * chunkInNAND = 0 is a dummy insert to make sure the tnodes are there.
	 */

	yaffs_Tnode *tn;
	yaffs_Device *dev = in->myDev;
	int existingChunk;
	yaffs_ExtendedTags existingTags;
	yaffs_ExtendedTags newTags;
	unsigned existingSerial, newSerial;

	if (in->variantType != YAFFS_OBJECT_TYPE_FILE) {
		/* Just ignore an attempt at putting a chunk into a non-file during scanning
		 * If it is not during Scanning then something went wrong!
		 */
		if (!inScan) {
			T(YAFFS_TRACE_ERROR,
			  (TSTR
			   ("yaffs tragedy:attempt to put data chunk into a non-file"
			    TENDSTR)));
			YBUG();
		}

		yaffs_DeleteChunk(dev, chunkInNAND, 1, __LINE__);
		return YAFFS_OK;
=======

static void yaffs_init_tnodes_and_objs(struct yaffs_dev *dev)
{
	int i;

	dev->n_obj = 0;
	dev->n_tnodes = 0;
	yaffs_init_raw_tnodes_and_objs(dev);

	for (i = 0; i < YAFFS_NOBJECT_BUCKETS; i++) {
		INIT_LIST_HEAD(&dev->obj_bucket[i].list);
		dev->obj_bucket[i].count = 0;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	}
}

<<<<<<< HEAD
	tn = yaffs_AddOrFindLevel0Tnode(dev,
					&in->variant.fileVariant,
					chunkInInode,
					NULL);
	if (!tn)
		return YAFFS_FAIL;
	
	if(!chunkInNAND)
		/* Dummy insert, bail now */
		return YAFFS_OK;

	existingChunk = yaffs_GetChunkGroupBase(dev, tn, chunkInInode);

	if (inScan != 0) {
		/* If we're scanning then we need to test for duplicates
		 * NB This does not need to be efficient since it should only ever
		 * happen when the power fails during a write, then only one
		 * chunk should ever be affected.
		 *
		 * Correction for YAFFS2: This could happen quite a lot and we need to think about efficiency! TODO
		 * Update: For backward scanning we don't need to re-read tags so this is quite cheap.
		 */

		if (existingChunk > 0) {
			/* NB Right now existing chunk will not be real chunkId if the chunk group size > 1
			 *    thus we have to do a FindChunkInFile to get the real chunk id.
			 *
			 * We have a duplicate now we need to decide which one to use:
			 *
			 * Backwards scanning YAFFS2: The old one is what we use, dump the new one.
			 * Forward scanning YAFFS2: The new one is what we use, dump the old one.
			 * YAFFS1: Get both sets of tags and compare serial numbers.
			 */

			if (inScan > 0) {
				/* Only do this for forward scanning */
				yaffs_ReadChunkWithTagsFromNAND(dev,
								chunkInNAND,
								NULL, &newTags);

				/* Do a proper find */
				existingChunk =
				    yaffs_FindChunkInFile(in, chunkInInode,
							  &existingTags);
			}

			if (existingChunk <= 0) {
				/*Hoosterman - how did this happen? */

				T(YAFFS_TRACE_ERROR,
				  (TSTR
				   ("yaffs tragedy: existing chunk < 0 in scan"
				    TENDSTR)));

			}

			/* NB The deleted flags should be false, otherwise the chunks will
			 * not be loaded during a scan
			 */

			if (inScan > 0) {
				newSerial = newTags.serialNumber;
				existingSerial = existingTags.serialNumber;
			}

			if ((inScan > 0) &&
			    (existingChunk <= 0 ||
			     ((existingSerial + 1) & 3) == newSerial)) {
				/* Forward scanning.
				 * Use new
				 * Delete the old one and drop through to update the tnode
				 */
				yaffs_DeleteChunk(dev, existingChunk, 1,
						  __LINE__);
			} else {
				/* Backward scanning or we want to use the existing one
				 * Use existing.
				 * Delete the new one and return early so that the tnode isn't changed
				 */
				yaffs_DeleteChunk(dev, chunkInNAND, 1,
						  __LINE__);
				return YAFFS_OK;
			}
		}

	}

	if (existingChunk == 0)
		in->nDataChunks++;

	yaffs_LoadLevel0Tnode(dev, tn, chunkInInode, chunkInNAND);

	return YAFFS_OK;
}

static int yaffs_ReadChunkDataFromObject(yaffs_Object *in, int chunkInInode,
					__u8 *buffer)
{
	int chunkInNAND = yaffs_FindChunkInFile(in, chunkInInode, NULL);

	if (chunkInNAND >= 0)
		return yaffs_ReadChunkWithTagsFromNAND(in->myDev, chunkInNAND,
						buffer, NULL);
	else {
		T(YAFFS_TRACE_NANDACCESS,
		  (TSTR("Chunk %d not found zero instead" TENDSTR),
		   chunkInNAND));
		/* get sane (zero) data if you read a hole */
		memset(buffer, 0, in->myDev->nDataBytesPerChunk);
		return 0;
	}

}

void yaffs_DeleteChunk(yaffs_Device *dev, int chunkId, int markNAND, int lyn)
{
	int block;
	int page;
	yaffs_ExtendedTags tags;
	yaffs_BlockInfo *bi;

	if (chunkId <= 0)
		return;

	dev->nDeletions++;
	block = chunkId / dev->param.nChunksPerBlock;
	page = chunkId % dev->param.nChunksPerBlock;


	if (!yaffs_CheckChunkBit(dev, block, page))
		T(YAFFS_TRACE_VERIFY,
			(TSTR("Deleting invalid chunk %d"TENDSTR),
			 chunkId));

	bi = yaffs_GetBlockInfo(dev, block);
	
	yaffs2_UpdateOldestDirtySequence(dev, block, bi);

	T(YAFFS_TRACE_DELETION,
	  (TSTR("line %d delete of chunk %d" TENDSTR), lyn, chunkId));

	if (!dev->param.isYaffs2 && markNAND &&
	    bi->blockState != YAFFS_BLOCK_STATE_COLLECTING) {

		yaffs_InitialiseTags(&tags);

		tags.chunkDeleted = 1;

		yaffs_WriteChunkWithTagsToNAND(dev, chunkId, NULL, &tags);
		yaffs_HandleUpdateChunk(dev, chunkId, &tags);
	} else {
		dev->nUnmarkedDeletions++;
	}

	/* Pull out of the management area.
	 * If the whole block became dirty, this will kick off an erasure.
	 */
	if (bi->blockState == YAFFS_BLOCK_STATE_ALLOCATING ||
	    bi->blockState == YAFFS_BLOCK_STATE_FULL ||
	    bi->blockState == YAFFS_BLOCK_STATE_NEEDS_SCANNING ||
	    bi->blockState == YAFFS_BLOCK_STATE_COLLECTING) {
		dev->nFreeChunks++;

		yaffs_ClearChunkBit(dev, block, page);

		bi->pagesInUse--;

		if (bi->pagesInUse == 0 &&
		    !bi->hasShrinkHeader &&
		    bi->blockState != YAFFS_BLOCK_STATE_ALLOCATING &&
		    bi->blockState != YAFFS_BLOCK_STATE_NEEDS_SCANNING) {
			yaffs_BlockBecameDirty(dev, block);
		}

	}

}

static int yaffs_WriteChunkDataToObject(yaffs_Object *in, int chunkInInode,
					const __u8 *buffer, int nBytes,
					int useReserve)
{
	/* Find old chunk Need to do this to get serial number
	 * Write new one and patch into tree.
	 * Invalidate old tags.
	 */

	int prevChunkId;
	yaffs_ExtendedTags prevTags;

	int newChunkId;
	yaffs_ExtendedTags newTags;

	yaffs_Device *dev = in->myDev;

	yaffs_CheckGarbageCollection(dev,0);

	/* Get the previous chunk at this location in the file if it exists.
	 * If it does not exist then put a zero into the tree. This creates
	 * the tnode now, rather than later when it is harder to clean up.
	 */
	prevChunkId = yaffs_FindChunkInFile(in, chunkInInode, &prevTags);
	if(prevChunkId < 1 &&
		!yaffs_PutChunkIntoFile(in, chunkInInode, 0, 0))
		return 0;

	/* Set up new tags */
	yaffs_InitialiseTags(&newTags);

	newTags.chunkId = chunkInInode;
	newTags.objectId = in->objectId;
	newTags.serialNumber =
	    (prevChunkId > 0) ? prevTags.serialNumber + 1 : 1;
	newTags.byteCount = nBytes;

	if (nBytes < 1 || nBytes > dev->param.totalBytesPerChunk) {
		T(YAFFS_TRACE_ERROR,
		(TSTR("Writing %d bytes to chunk!!!!!!!!!" TENDSTR), nBytes));
		YBUG();
	}
	
		
	newChunkId =
	    yaffs_WriteNewChunkWithTagsToNAND(dev, buffer, &newTags,
					      useReserve);

	if (newChunkId > 0) {
		yaffs_PutChunkIntoFile(in, chunkInInode, newChunkId, 0);

		if (prevChunkId > 0)
			yaffs_DeleteChunk(dev, prevChunkId, 1, __LINE__);

		yaffs_VerifyFileSanity(in);
	}
	return newChunkId;

}

/* UpdateObjectHeader updates the header on NAND for an object.
 * If name is not NULL, then that new name is used.
 */
int yaffs_UpdateObjectHeader(yaffs_Object *in, const YCHAR *name, int force,
			     int isShrink, int shadows, yaffs_XAttrMod *xmod)
{

	yaffs_BlockInfo *bi;

	yaffs_Device *dev = in->myDev;

	int prevChunkId;
	int retVal = 0;
	int result = 0;

	int newChunkId;
	yaffs_ExtendedTags newTags;
	yaffs_ExtendedTags oldTags;
	const YCHAR *alias = NULL;

	__u8 *buffer = NULL;
	YCHAR oldName[YAFFS_MAX_NAME_LENGTH + 1];

	yaffs_ObjectHeader *oh = NULL;

	yaffs_strcpy(oldName, _Y("silly old name"));


	if (!in->fake ||
		in == dev->rootDir || /* The rootDir should also be saved */
		force  || xmod) {

		yaffs_CheckGarbageCollection(dev,0);
		yaffs_CheckObjectDetailsLoaded(in);

		buffer = yaffs_GetTempBuffer(in->myDev, __LINE__);
		oh = (yaffs_ObjectHeader *) buffer;

		prevChunkId = in->hdrChunk;

		if (prevChunkId > 0) {
			result = yaffs_ReadChunkWithTagsFromNAND(dev, prevChunkId,
							buffer, &oldTags);

			yaffs_VerifyObjectHeader(in, oh, &oldTags, 0);

			memcpy(oldName, oh->name, sizeof(oh->name));
			memset(buffer, 0xFF, sizeof(yaffs_ObjectHeader));
		} else
			memset(buffer, 0xFF, dev->nDataBytesPerChunk);

		oh->type = in->variantType;
		oh->yst_mode = in->yst_mode;
		oh->shadowsObject = oh->inbandShadowsObject = shadows;

#ifdef CONFIG_YAFFS_WINCE
		oh->win_atime[0] = in->win_atime[0];
		oh->win_ctime[0] = in->win_ctime[0];
		oh->win_mtime[0] = in->win_mtime[0];
		oh->win_atime[1] = in->win_atime[1];
		oh->win_ctime[1] = in->win_ctime[1];
		oh->win_mtime[1] = in->win_mtime[1];
#else
		oh->yst_uid = in->yst_uid;
		oh->yst_gid = in->yst_gid;
		oh->yst_atime = in->yst_atime;
		oh->yst_mtime = in->yst_mtime;
		oh->yst_ctime = in->yst_ctime;
		oh->yst_rdev = in->yst_rdev;
#endif
		if (in->parent)
			oh->parentObjectId = in->parent->objectId;
		else
			oh->parentObjectId = 0;

		if (name && *name) {
			memset(oh->name, 0, sizeof(oh->name));
			yaffs_LoadObjectHeaderFromName(dev,oh->name,name);
		} else if (prevChunkId > 0)
			memcpy(oh->name, oldName, sizeof(oh->name));
		else
			memset(oh->name, 0, sizeof(oh->name));

		oh->isShrink = isShrink;

		switch (in->variantType) {
		case YAFFS_OBJECT_TYPE_UNKNOWN:
			/* Should not happen */
			break;
		case YAFFS_OBJECT_TYPE_FILE:
			oh->fileSize =
			    (oh->parentObjectId == YAFFS_OBJECTID_DELETED
			     || oh->parentObjectId ==
			     YAFFS_OBJECTID_UNLINKED) ? 0 : in->variant.
			    fileVariant.fileSize;
			break;
		case YAFFS_OBJECT_TYPE_HARDLINK:
			oh->equivalentObjectId =
			    in->variant.hardLinkVariant.equivalentObjectId;
			break;
		case YAFFS_OBJECT_TYPE_SPECIAL:
			/* Do nothing */
			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			/* Do nothing */
			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
			alias = in->variant.symLinkVariant.alias;
			if(!alias)
				alias = _Y("no alias");
			yaffs_strncpy(oh->alias,
					alias,
				      YAFFS_MAX_ALIAS_LENGTH);
			oh->alias[YAFFS_MAX_ALIAS_LENGTH] = 0;
			break;
		}

		/* process any xattrib modifications */
		if(xmod)
			yaffs_ApplyXMod(in, (char *)buffer, xmod);


		/* Tags */
		yaffs_InitialiseTags(&newTags);
		in->serial++;
		newTags.chunkId = 0;
		newTags.objectId = in->objectId;
		newTags.serialNumber = in->serial;

		/* Add extra info for file header */

		newTags.extraHeaderInfoAvailable = 1;
		newTags.extraParentObjectId = oh->parentObjectId;
		newTags.extraFileLength = oh->fileSize;
		newTags.extraIsShrinkHeader = oh->isShrink;
		newTags.extraEquivalentObjectId = oh->equivalentObjectId;
		newTags.extraShadows = (oh->shadowsObject > 0) ? 1 : 0;
		newTags.extraObjectType = in->variantType;

		yaffs_VerifyObjectHeader(in, oh, &newTags, 1);

		/* Create new chunk in NAND */
		newChunkId =
		    yaffs_WriteNewChunkWithTagsToNAND(dev, buffer, &newTags,
						      (prevChunkId > 0) ? 1 : 0);

		if (newChunkId >= 0) {

			in->hdrChunk = newChunkId;

			if (prevChunkId > 0) {
				yaffs_DeleteChunk(dev, prevChunkId, 1,
						  __LINE__);
			}

			if (!yaffs_ObjectHasCachedWriteData(in))
				in->dirty = 0;

			/* If this was a shrink, then mark the block that the chunk lives on */
			if (isShrink) {
				bi = yaffs_GetBlockInfo(in->myDev,
					newChunkId / in->myDev->param.nChunksPerBlock);
				bi->hasShrinkHeader = 1;
			}

		}

		retVal = newChunkId;

	}

	if (buffer)
		yaffs_ReleaseTempBuffer(dev, buffer, __LINE__);

	return retVal;
}

/*------------------------ Short Operations Cache ----------------------------------------
 *   In many situations where there is no high level buffering (eg WinCE) a lot of
 *   reads might be short sequential reads, and a lot of writes may be short
 *   sequential writes. eg. scanning/writing a jpeg file.
 *   In these cases, a short read/write cache can provide a huge perfomance benefit
 *   with dumb-as-a-rock code.
 *   In Linux, the page cache provides read buffering aand the short op cache provides write
 *   buffering.
 *
 *   There are a limited number (~10) of cache chunks per device so that we don't
 *   need a very intelligent search.
 */

static int yaffs_ObjectHasCachedWriteData(yaffs_Object *obj)
{
	yaffs_Device *dev = obj->myDev;
	int i;
	yaffs_ChunkCache *cache;
	int nCaches = obj->myDev->param.nShortOpCaches;

	for (i = 0; i < nCaches; i++) {
		cache = &dev->srCache[i];
		if (cache->object == obj &&
		    cache->dirty)
			return 1;
	}

	return 0;
}


static void yaffs_FlushFilesChunkCache(yaffs_Object *obj)
{
	yaffs_Device *dev = obj->myDev;
	int lowest = -99;	/* Stop compiler whining. */
	int i;
	yaffs_ChunkCache *cache;
	int chunkWritten = 0;
	int nCaches = obj->myDev->param.nShortOpCaches;

	if (nCaches > 0) {
		do {
			cache = NULL;

			/* Find the dirty cache for this object with the lowest chunk id. */
			for (i = 0; i < nCaches; i++) {
				if (dev->srCache[i].object == obj &&
				    dev->srCache[i].dirty) {
					if (!cache
					    || dev->srCache[i].chunkId <
					    lowest) {
						cache = &dev->srCache[i];
						lowest = cache->chunkId;
					}
				}
			}

			if (cache && !cache->locked) {
				/* Write it out and free it up */

				chunkWritten =
				    yaffs_WriteChunkDataToObject(cache->object,
								 cache->chunkId,
								 cache->data,
								 cache->nBytes,
								 1);
				cache->dirty = 0;
				cache->object = NULL;
			}

		} while (cache && chunkWritten > 0);

		if (cache) {
			/* Hoosterman, disk full while writing cache out. */
			T(YAFFS_TRACE_ERROR,
			  (TSTR("yaffs tragedy: no space during cache write" TENDSTR)));

		}
	}

}

/*yaffs_FlushEntireDeviceCache(dev)
 *
 *
 */

void yaffs_FlushEntireDeviceCache(yaffs_Device *dev)
{
	yaffs_Object *obj;
	int nCaches = dev->param.nShortOpCaches;
	int i;

	/* Find a dirty object in the cache and flush it...
	 * until there are no further dirty objects.
	 */
	do {
		obj = NULL;
		for (i = 0; i < nCaches && !obj; i++) {
			if (dev->srCache[i].object &&
			    dev->srCache[i].dirty)
				obj = dev->srCache[i].object;

		}
		if (obj)
			yaffs_FlushFilesChunkCache(obj);

	} while (obj);
=======
struct yaffs_obj *yaffs_find_or_create_by_number(struct yaffs_dev *dev,
						 int number,
						 enum yaffs_obj_type type)
{
	struct yaffs_obj *the_obj = NULL;

	if (number > 0)
		the_obj = yaffs_find_by_number(dev, number);

	if (!the_obj)
		the_obj = yaffs_new_obj(dev, number, type);

	return the_obj;

}

YCHAR *yaffs_clone_str(const YCHAR *str)
{
	YCHAR *new_str = NULL;
	int len;

	if (!str)
		str = _Y("");

	len = strnlen(str, YAFFS_MAX_ALIAS_LENGTH);
	new_str = kmalloc((len + 1) * sizeof(YCHAR), GFP_NOFS);
	if (new_str) {
		strncpy(new_str, str, len);
		new_str[len] = 0;
	}
	return new_str;

}
/*
 *yaffs_update_parent() handles fixing a directories mtime and ctime when a new
 * link (ie. name) is created or deleted in the directory.
 *
 * ie.
 *   create dir/a : update dir's mtime/ctime
 *   rm dir/a:   update dir's mtime/ctime
 *   modify dir/a: don't update dir's mtimme/ctime
 *
 * This can be handled immediately or defered. Defering helps reduce the number
 * of updates when many files in a directory are changed within a brief period.
 *
 * If the directory updating is defered then yaffs_update_dirty_dirs must be
 * called periodically.
 */

static void yaffs_update_parent(struct yaffs_obj *obj)
{
	struct yaffs_dev *dev;

	if (!obj)
		return;
	dev = obj->my_dev;
	obj->dirty = 1;
	yaffs_load_current_time(obj, 0, 1);
	if (dev->param.defered_dir_update) {
		struct list_head *link = &obj->variant.dir_variant.dirty;

		if (list_empty(link)) {
			list_add(link, &dev->dirty_dirs);
			yaffs_trace(YAFFS_TRACE_BACKGROUND,
			  "Added object %d to dirty directories",
			   obj->obj_id);
		}

	} else {
		yaffs_update_oh(obj, NULL, 0, 0, 0, NULL);
	}
}

void yaffs_update_dirty_dirs(struct yaffs_dev *dev)
{
	struct list_head *link;
	struct yaffs_obj *obj;
	struct yaffs_dir_var *d_s;
	union yaffs_obj_var *o_v;

	yaffs_trace(YAFFS_TRACE_BACKGROUND, "Update dirty directories");

	while (!list_empty(&dev->dirty_dirs)) {
		link = dev->dirty_dirs.next;
		list_del_init(link);

		d_s = list_entry(link, struct yaffs_dir_var, dirty);
		o_v = list_entry(d_s, union yaffs_obj_var, dir_variant);
		obj = list_entry(o_v, struct yaffs_obj, variant);

		yaffs_trace(YAFFS_TRACE_BACKGROUND, "Update directory %d",
			obj->obj_id);

		if (obj->dirty)
			yaffs_update_oh(obj, NULL, 0, 0, 0, NULL);
	}
}

/*
 * Mknod (create) a new object.
 * equiv_obj only has meaning for a hard link;
 * alias_str only has meaning for a symlink.
 * rdev only has meaning for devices (a subset of special objects)
 */

static struct yaffs_obj *yaffs_create_obj(enum yaffs_obj_type type,
					  struct yaffs_obj *parent,
					  const YCHAR *name,
					  u32 mode,
					  u32 uid,
					  u32 gid,
					  struct yaffs_obj *equiv_obj,
					  const YCHAR *alias_str, u32 rdev)
{
	struct yaffs_obj *in;
	YCHAR *str = NULL;
	struct yaffs_dev *dev = parent->my_dev;

	/* Check if the entry exists.
	 * If it does then fail the call since we don't want a dup. */
	if (yaffs_find_by_name(parent, name))
		return NULL;

	if (type == YAFFS_OBJECT_TYPE_SYMLINK) {
		str = yaffs_clone_str(alias_str);
		if (!str)
			return NULL;
	}

	in = yaffs_new_obj(dev, -1, type);

	if (!in) {
		kfree(str);
		return NULL;
	}

	in->hdr_chunk = 0;
	in->valid = 1;
	in->variant_type = type;

	in->yst_mode = mode;

	yaffs_attribs_init(in, gid, uid, rdev);

	in->n_data_chunks = 0;

	yaffs_set_obj_name(in, name);
	in->dirty = 1;

	yaffs_add_obj_to_dir(parent, in);

	in->my_dev = parent->my_dev;

	switch (type) {
	case YAFFS_OBJECT_TYPE_SYMLINK:
		in->variant.symlink_variant.alias = str;
		break;
	case YAFFS_OBJECT_TYPE_HARDLINK:
		in->variant.hardlink_variant.equiv_obj = equiv_obj;
		in->variant.hardlink_variant.equiv_id = equiv_obj->obj_id;
		list_add(&in->hard_links, &equiv_obj->hard_links);
		break;
	case YAFFS_OBJECT_TYPE_FILE:
	case YAFFS_OBJECT_TYPE_DIRECTORY:
	case YAFFS_OBJECT_TYPE_SPECIAL:
	case YAFFS_OBJECT_TYPE_UNKNOWN:
		/* do nothing */
		break;
	}

	if (yaffs_update_oh(in, name, 0, 0, 0, NULL) < 0) {
		/* Could not create the object header, fail */
		yaffs_del_obj(in);
		in = NULL;
	}

	if (in)
		yaffs_update_parent(parent);

	return in;
}

struct yaffs_obj *yaffs_create_file(struct yaffs_obj *parent,
				    const YCHAR *name, u32 mode, u32 uid,
				    u32 gid)
{
	return yaffs_create_obj(YAFFS_OBJECT_TYPE_FILE, parent, name, mode,
				uid, gid, NULL, NULL, 0);
}

struct yaffs_obj *yaffs_create_dir(struct yaffs_obj *parent, const YCHAR *name,
				   u32 mode, u32 uid, u32 gid)
{
	return yaffs_create_obj(YAFFS_OBJECT_TYPE_DIRECTORY, parent, name,
				mode, uid, gid, NULL, NULL, 0);
}

struct yaffs_obj *yaffs_create_special(struct yaffs_obj *parent,
				       const YCHAR *name, u32 mode, u32 uid,
				       u32 gid, u32 rdev)
{
	return yaffs_create_obj(YAFFS_OBJECT_TYPE_SPECIAL, parent, name, mode,
				uid, gid, NULL, NULL, rdev);
}

struct yaffs_obj *yaffs_create_symlink(struct yaffs_obj *parent,
				       const YCHAR *name, u32 mode, u32 uid,
				       u32 gid, const YCHAR *alias)
{
	return yaffs_create_obj(YAFFS_OBJECT_TYPE_SYMLINK, parent, name, mode,
				uid, gid, NULL, alias, 0);
}

/* yaffs_link_obj returns the object id of the equivalent object.*/
struct yaffs_obj *yaffs_link_obj(struct yaffs_obj *parent, const YCHAR * name,
				 struct yaffs_obj *equiv_obj)
{
	/* Get the real object in case we were fed a hard link obj */
	equiv_obj = yaffs_get_equivalent_obj(equiv_obj);

	if (yaffs_create_obj(YAFFS_OBJECT_TYPE_HARDLINK,
			parent, name, 0, 0, 0,
			equiv_obj, NULL, 0))
		return equiv_obj;

	return NULL;

}



/*---------------------- Block Management and Page Allocation -------------*/

static void yaffs_deinit_blocks(struct yaffs_dev *dev)
{
	if (dev->block_info_alt && dev->block_info)
		vfree(dev->block_info);
	else
		kfree(dev->block_info);

	dev->block_info_alt = 0;

	dev->block_info = NULL;

	if (dev->chunk_bits_alt && dev->chunk_bits)
		vfree(dev->chunk_bits);
	else
		kfree(dev->chunk_bits);
	dev->chunk_bits_alt = 0;
	dev->chunk_bits = NULL;
}

static int yaffs_init_blocks(struct yaffs_dev *dev)
{
	int n_blocks = dev->internal_end_block - dev->internal_start_block + 1;

	dev->block_info = NULL;
	dev->chunk_bits = NULL;
	dev->alloc_block = -1;	/* force it to get a new one */

	/* If the first allocation strategy fails, thry the alternate one */
	dev->block_info =
		kmalloc(n_blocks * sizeof(struct yaffs_block_info), GFP_NOFS);
	if (!dev->block_info) {
		dev->block_info =
		    vmalloc(n_blocks * sizeof(struct yaffs_block_info));
		dev->block_info_alt = 1;
	} else {
		dev->block_info_alt = 0;
	}

	if (!dev->block_info)
		goto alloc_error;

	/* Set up dynamic blockinfo stuff. Round up bytes. */
	dev->chunk_bit_stride = (dev->param.chunks_per_block + 7) / 8;
	dev->chunk_bits =
		kmalloc(dev->chunk_bit_stride * n_blocks, GFP_NOFS);
	if (!dev->chunk_bits) {
		dev->chunk_bits =
		    vmalloc(dev->chunk_bit_stride * n_blocks);
		dev->chunk_bits_alt = 1;
	} else {
		dev->chunk_bits_alt = 0;
	}
	if (!dev->chunk_bits)
		goto alloc_error;


	memset(dev->block_info, 0, n_blocks * sizeof(struct yaffs_block_info));
	memset(dev->chunk_bits, 0, dev->chunk_bit_stride * n_blocks);
	return YAFFS_OK;

alloc_error:
	yaffs_deinit_blocks(dev);
	return YAFFS_FAIL;
}


void yaffs_block_became_dirty(struct yaffs_dev *dev, int block_no)
{
	struct yaffs_block_info *bi = yaffs_get_block_info(dev, block_no);
	int erased_ok = 0;
	int i;

	/* If the block is still healthy erase it and mark as clean.
	 * If the block has had a data failure, then retire it.
	 */

	yaffs_trace(YAFFS_TRACE_GC | YAFFS_TRACE_ERASE,
		"yaffs_block_became_dirty block %d state %d %s",
		block_no, bi->block_state,
		(bi->needs_retiring) ? "needs retiring" : "");

	yaffs2_clear_oldest_dirty_seq(dev, bi);

	bi->block_state = YAFFS_BLOCK_STATE_DIRTY;

	/* If this is the block being garbage collected then stop gc'ing */
	if (block_no == dev->gc_block)
		dev->gc_block = 0;

	/* If this block is currently the best candidate for gc
	 * then drop as a candidate */
	if (block_no == dev->gc_dirtiest) {
		dev->gc_dirtiest = 0;
		dev->gc_pages_in_use = 0;
	}

	if (!bi->needs_retiring) {
		yaffs2_checkpt_invalidate(dev);
		erased_ok = yaffs_erase_block(dev, block_no);
		if (!erased_ok) {
			dev->n_erase_failures++;
			yaffs_trace(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
			  "**>> Erasure failed %d", block_no);
		}
	}

	/* Verify erasure if needed */
	if (erased_ok &&
	    ((yaffs_trace_mask & YAFFS_TRACE_ERASE) ||
	     !yaffs_skip_verification(dev))) {
		for (i = 0; i < dev->param.chunks_per_block; i++) {
			if (!yaffs_check_chunk_erased(dev,
				block_no * dev->param.chunks_per_block + i)) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					">>Block %d erasure supposedly OK, but chunk %d not erased",
					block_no, i);
			}
		}
	}

	if (!erased_ok) {
		/* We lost a block of free space */
		dev->n_free_chunks -= dev->param.chunks_per_block;
		yaffs_retire_block(dev, block_no);
		yaffs_trace(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
			"**>> Block %d retired", block_no);
		return;
	}

	/* Clean it up... */
	bi->block_state = YAFFS_BLOCK_STATE_EMPTY;
	bi->seq_number = 0;
	dev->n_erased_blocks++;
	bi->pages_in_use = 0;
	bi->soft_del_pages = 0;
	bi->has_shrink_hdr = 0;
	bi->skip_erased_check = 1;	/* Clean, so no need to check */
	bi->gc_prioritise = 0;
	yaffs_clear_chunk_bits(dev, block_no);

	yaffs_trace(YAFFS_TRACE_ERASE, "Erased block %d", block_no);
}

static inline int yaffs_gc_process_chunk(struct yaffs_dev *dev,
					struct yaffs_block_info *bi,
					int old_chunk, u8 *buffer)
{
	int new_chunk;
	int mark_flash = 1;
	struct yaffs_ext_tags tags;
	struct yaffs_obj *object;
	int matching_chunk;
	int ret_val = YAFFS_OK;

	memset(&tags, 0, sizeof(tags));
	yaffs_rd_chunk_tags_nand(dev, old_chunk,
				 buffer, &tags);
	object = yaffs_find_by_number(dev, tags.obj_id);

	yaffs_trace(YAFFS_TRACE_GC_DETAIL,
		"Collecting chunk in block %d, %d %d %d ",
		dev->gc_chunk, tags.obj_id,
		tags.chunk_id, tags.n_bytes);

	if (object && !yaffs_skip_verification(dev)) {
		if (tags.chunk_id == 0)
			matching_chunk =
			    object->hdr_chunk;
		else if (object->soft_del)
			/* Defeat the test */
			matching_chunk = old_chunk;
		else
			matching_chunk =
			    yaffs_find_chunk_in_file
			    (object, tags.chunk_id,
			     NULL);

		if (old_chunk != matching_chunk)
			yaffs_trace(YAFFS_TRACE_ERROR,
				"gc: page in gc mismatch: %d %d %d %d",
				old_chunk,
				matching_chunk,
				tags.obj_id,
				tags.chunk_id);
	}

	if (!object) {
		yaffs_trace(YAFFS_TRACE_ERROR,
			"page %d in gc has no object: %d %d %d ",
			old_chunk,
			tags.obj_id, tags.chunk_id,
			tags.n_bytes);
	}

	if (object &&
	    object->deleted &&
	    object->soft_del && tags.chunk_id != 0) {
		/* Data chunk in a soft deleted file,
		 * throw it away.
		 * It's a soft deleted data chunk,
		 * No need to copy this, just forget
		 * about it and fix up the object.
		 */

		/* Free chunks already includes
		 * softdeleted chunks, how ever this
		 * chunk is going to soon be really
		 * deleted which will increment free
		 * chunks. We have to decrement free
		 * chunks so this works out properly.
		 */
		dev->n_free_chunks--;
		bi->soft_del_pages--;

		object->n_data_chunks--;
		if (object->n_data_chunks <= 0) {
			/* remeber to clean up obj */
			dev->gc_cleanup_list[dev->n_clean_ups] = tags.obj_id;
			dev->n_clean_ups++;
		}
		mark_flash = 0;
	} else if (object) {
		/* It's either a data chunk in a live
		 * file or an ObjectHeader, so we're
		 * interested in it.
		 * NB Need to keep the ObjectHeaders of
		 * deleted files until the whole file
		 * has been deleted off
		 */
		tags.serial_number++;
		dev->n_gc_copies++;

		if (tags.chunk_id == 0) {
			/* It is an object Id,
			 * We need to nuke the
			 * shrinkheader flags since its
			 * work is done.
			 * Also need to clean up
			 * shadowing.
			 */
			struct yaffs_obj_hdr *oh;
			oh = (struct yaffs_obj_hdr *) buffer;

			oh->is_shrink = 0;
			tags.extra_is_shrink = 0;
			oh->shadows_obj = 0;
			oh->inband_shadowed_obj_id = 0;
			tags.extra_shadows = 0;

			/* Update file size */
			if (object->variant_type == YAFFS_OBJECT_TYPE_FILE) {
				oh->file_size =
				    object->variant.file_variant.file_size;
				tags.extra_length = oh->file_size;
			}

			yaffs_verify_oh(object, oh, &tags, 1);
			new_chunk =
			    yaffs_write_new_chunk(dev, (u8 *) oh, &tags, 1);
		} else {
			new_chunk =
			    yaffs_write_new_chunk(dev, buffer, &tags, 1);
		}

		if (new_chunk < 0) {
			ret_val = YAFFS_FAIL;
		} else {

			/* Now fix up the Tnodes etc. */

			if (tags.chunk_id == 0) {
				/* It's a header */
				object->hdr_chunk = new_chunk;
				object->serial = tags.serial_number;
			} else {
				/* It's a data chunk */
				yaffs_put_chunk_in_file(object, tags.chunk_id,
							new_chunk, 0);
			}
		}
	}
	if (ret_val == YAFFS_OK)
		yaffs_chunk_del(dev, old_chunk, mark_flash, __LINE__);
	return ret_val;
}

static int yaffs_gc_block(struct yaffs_dev *dev, int block, int whole_block)
{
	int old_chunk;
	int ret_val = YAFFS_OK;
	int i;
	int is_checkpt_block;
	int max_copies;
	int chunks_before = yaffs_get_erased_chunks(dev);
	int chunks_after;
	struct yaffs_block_info *bi = yaffs_get_block_info(dev, block);

	is_checkpt_block = (bi->block_state == YAFFS_BLOCK_STATE_CHECKPOINT);

	yaffs_trace(YAFFS_TRACE_TRACING,
		"Collecting block %d, in use %d, shrink %d, whole_block %d",
		block, bi->pages_in_use, bi->has_shrink_hdr,
		whole_block);

	/*yaffs_verify_free_chunks(dev); */

	if (bi->block_state == YAFFS_BLOCK_STATE_FULL)
		bi->block_state = YAFFS_BLOCK_STATE_COLLECTING;

	bi->has_shrink_hdr = 0;	/* clear the flag so that the block can erase */

	dev->gc_disable = 1;

	if (is_checkpt_block || !yaffs_still_some_chunks(dev, block)) {
		yaffs_trace(YAFFS_TRACE_TRACING,
			"Collecting block %d that has no chunks in use",
			block);
		yaffs_block_became_dirty(dev, block);
	} else {

		u8 *buffer = yaffs_get_temp_buffer(dev);

		yaffs_verify_blk(dev, bi, block);

		max_copies = (whole_block) ? dev->param.chunks_per_block : 5;
		old_chunk = block * dev->param.chunks_per_block + dev->gc_chunk;

		for (/* init already done */ ;
		     ret_val == YAFFS_OK &&
		     dev->gc_chunk < dev->param.chunks_per_block &&
		     (bi->block_state == YAFFS_BLOCK_STATE_COLLECTING) &&
		     max_copies > 0;
		     dev->gc_chunk++, old_chunk++) {
			if (yaffs_check_chunk_bit(dev, block, dev->gc_chunk)) {
				/* Page is in use and might need to be copied */
				max_copies--;
				ret_val = yaffs_gc_process_chunk(dev, bi,
							old_chunk, buffer);
			}
		}
		yaffs_release_temp_buffer(dev, buffer);
	}

	yaffs_verify_collected_blk(dev, bi, block);

	if (bi->block_state == YAFFS_BLOCK_STATE_COLLECTING) {
		/*
		 * The gc did not complete. Set block state back to FULL
		 * because checkpointing does not restore gc.
		 */
		bi->block_state = YAFFS_BLOCK_STATE_FULL;
	} else {
		/* The gc completed. */
		/* Do any required cleanups */
		for (i = 0; i < dev->n_clean_ups; i++) {
			/* Time to delete the file too */
			struct yaffs_obj *object =
			    yaffs_find_by_number(dev, dev->gc_cleanup_list[i]);
			if (object) {
				yaffs_free_tnode(dev,
					  object->variant.file_variant.top);
				object->variant.file_variant.top = NULL;
				yaffs_trace(YAFFS_TRACE_GC,
					"yaffs: About to finally delete object %d",
					object->obj_id);
				yaffs_generic_obj_del(object);
				object->my_dev->n_deleted_files--;
			}

		}
		chunks_after = yaffs_get_erased_chunks(dev);
		if (chunks_before >= chunks_after)
			yaffs_trace(YAFFS_TRACE_GC,
				"gc did not increase free chunks before %d after %d",
				chunks_before, chunks_after);
		dev->gc_block = 0;
		dev->gc_chunk = 0;
		dev->n_clean_ups = 0;
	}

	dev->gc_disable = 0;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	return ret_val;
}

<<<<<<< HEAD

/* Grab us a cache chunk for use.
 * First look for an empty one.
 * Then look for the least recently used non-dirty one.
 * Then look for the least recently used dirty one...., flush and look again.
 */
static yaffs_ChunkCache *yaffs_GrabChunkCacheWorker(yaffs_Device *dev)
{
	int i;

	if (dev->param.nShortOpCaches > 0) {
		for (i = 0; i < dev->param.nShortOpCaches; i++) {
			if (!dev->srCache[i].object)
				return &dev->srCache[i];
		}
	}

	return NULL;
}

static yaffs_ChunkCache *yaffs_GrabChunkCache(yaffs_Device *dev)
{
	yaffs_ChunkCache *cache;
	yaffs_Object *theObj;
	int usage;
	int i;
	int pushout;

	if (dev->param.nShortOpCaches > 0) {
		/* Try find a non-dirty one... */

		cache = yaffs_GrabChunkCacheWorker(dev);

		if (!cache) {
			/* They were all dirty, find the last recently used object and flush
			 * its cache, then  find again.
			 * NB what's here is not very accurate, we actually flush the object
			 * the last recently used page.
			 */

			/* With locking we can't assume we can use entry zero */

			theObj = NULL;
			usage = -1;
			cache = NULL;
			pushout = -1;

			for (i = 0; i < dev->param.nShortOpCaches; i++) {
				if (dev->srCache[i].object &&
				    !dev->srCache[i].locked &&
				    (dev->srCache[i].lastUse < usage || !cache)) {
					usage = dev->srCache[i].lastUse;
					theObj = dev->srCache[i].object;
					cache = &dev->srCache[i];
					pushout = i;
				}
			}

			if (!cache || cache->dirty) {
				/* Flush and try again */
				yaffs_FlushFilesChunkCache(theObj);
				cache = yaffs_GrabChunkCacheWorker(dev);
			}

		}
		return cache;
	} else
		return NULL;

}

/* Find a cached chunk */
static yaffs_ChunkCache *yaffs_FindChunkCache(const yaffs_Object *obj,
					      int chunkId)
{
	yaffs_Device *dev = obj->myDev;
	int i;
	if (dev->param.nShortOpCaches > 0) {
		for (i = 0; i < dev->param.nShortOpCaches; i++) {
			if (dev->srCache[i].object == obj &&
			    dev->srCache[i].chunkId == chunkId) {
				dev->cacheHits++;

				return &dev->srCache[i];
			}
=======
/*
 * find_gc_block() selects the dirtiest block (or close enough)
 * for garbage collection.
 */

static unsigned yaffs_find_gc_block(struct yaffs_dev *dev,
				    int aggressive, int background)
{
	int i;
	int iterations;
	unsigned selected = 0;
	int prioritised = 0;
	int prioritised_exist = 0;
	struct yaffs_block_info *bi;
	int threshold;

	/* First let's see if we need to grab a prioritised block */
	if (dev->has_pending_prioritised_gc && !aggressive) {
		dev->gc_dirtiest = 0;
		bi = dev->block_info;
		for (i = dev->internal_start_block;
		     i <= dev->internal_end_block && !selected; i++) {

			if (bi->gc_prioritise) {
				prioritised_exist = 1;
				if (bi->block_state == YAFFS_BLOCK_STATE_FULL &&
				    yaffs_block_ok_for_gc(dev, bi)) {
					selected = i;
					prioritised = 1;
				}
			}
			bi++;
		}

		/*
		 * If there is a prioritised block and none was selected then
		 * this happened because there is at least one old dirty block
		 * gumming up the works. Let's gc the oldest dirty block.
		 */

		if (prioritised_exist &&
		    !selected && dev->oldest_dirty_block > 0)
			selected = dev->oldest_dirty_block;

		if (!prioritised_exist)	/* None found, so we can clear this */
			dev->has_pending_prioritised_gc = 0;
	}

	/* If we're doing aggressive GC then we are happy to take a less-dirty
	 * block, and search harder.
	 * else (leasurely gc), then we only bother to do this if the
	 * block has only a few pages in use.
	 */

	if (!selected) {
		int pages_used;
		int n_blocks =
		    dev->internal_end_block - dev->internal_start_block + 1;
		if (aggressive) {
			threshold = dev->param.chunks_per_block;
			iterations = n_blocks;
		} else {
			int max_threshold;

			if (background)
				max_threshold = dev->param.chunks_per_block / 2;
			else
				max_threshold = dev->param.chunks_per_block / 8;

			if (max_threshold < YAFFS_GC_PASSIVE_THRESHOLD)
				max_threshold = YAFFS_GC_PASSIVE_THRESHOLD;

			threshold = background ? (dev->gc_not_done + 2) * 2 : 0;
			if (threshold < YAFFS_GC_PASSIVE_THRESHOLD)
				threshold = YAFFS_GC_PASSIVE_THRESHOLD;
			if (threshold > max_threshold)
				threshold = max_threshold;

			iterations = n_blocks / 16 + 1;
			if (iterations > 100)
				iterations = 100;
		}

		for (i = 0;
		     i < iterations &&
		     (dev->gc_dirtiest < 1 ||
		      dev->gc_pages_in_use > YAFFS_GC_GOOD_ENOUGH);
		     i++) {
			dev->gc_block_finder++;
			if (dev->gc_block_finder < dev->internal_start_block ||
			    dev->gc_block_finder > dev->internal_end_block)
				dev->gc_block_finder =
				    dev->internal_start_block;

			bi = yaffs_get_block_info(dev, dev->gc_block_finder);

			pages_used = bi->pages_in_use - bi->soft_del_pages;

			if (bi->block_state == YAFFS_BLOCK_STATE_FULL &&
			    pages_used < dev->param.chunks_per_block &&
			    (dev->gc_dirtiest < 1 ||
			     pages_used < dev->gc_pages_in_use) &&
			    yaffs_block_ok_for_gc(dev, bi)) {
				dev->gc_dirtiest = dev->gc_block_finder;
				dev->gc_pages_in_use = pages_used;
			}
		}

		if (dev->gc_dirtiest > 0 && dev->gc_pages_in_use <= threshold)
			selected = dev->gc_dirtiest;
	}

	/*
	 * If nothing has been selected for a while, try the oldest dirty
	 * because that's gumming up the works.
	 */

	if (!selected && dev->param.is_yaffs2 &&
	    dev->gc_not_done >= (background ? 10 : 20)) {
		yaffs2_find_oldest_dirty_seq(dev);
		if (dev->oldest_dirty_block > 0) {
			selected = dev->oldest_dirty_block;
			dev->gc_dirtiest = selected;
			dev->oldest_dirty_gc_count++;
			bi = yaffs_get_block_info(dev, selected);
			dev->gc_pages_in_use =
			    bi->pages_in_use - bi->soft_del_pages;
		} else {
			dev->gc_not_done = 0;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		}
	}
	return NULL;
}

<<<<<<< HEAD
/* Mark the chunk for the least recently used algorithym */
static void yaffs_UseChunkCache(yaffs_Device *dev, yaffs_ChunkCache *cache,
				int isAWrite)
{

	if (dev->param.nShortOpCaches > 0) {
		if (dev->srLastUse < 0 || dev->srLastUse > 100000000) {
			/* Reset the cache usages */
			int i;
			for (i = 1; i < dev->param.nShortOpCaches; i++)
				dev->srCache[i].lastUse = 0;

			dev->srLastUse = 0;
		}

		dev->srLastUse++;

		cache->lastUse = dev->srLastUse;

		if (isAWrite)
			cache->dirty = 1;
	}
}

/* Invalidate a single cache page.
 * Do this when a whole page gets written,
 * ie the short cache for this page is no longer valid.
 */
static void yaffs_InvalidateChunkCache(yaffs_Object *object, int chunkId)
{
	if (object->myDev->param.nShortOpCaches > 0) {
		yaffs_ChunkCache *cache = yaffs_FindChunkCache(object, chunkId);

		if (cache)
			cache->object = NULL;
	}
}

/* Invalidate all the cache pages associated with this object
 * Do this whenever ther file is deleted or resized.
 */
static void yaffs_InvalidateWholeChunkCache(yaffs_Object *in)
{
	int i;
	yaffs_Device *dev = in->myDev;

	if (dev->param.nShortOpCaches > 0) {
		/* Invalidate it. */
		for (i = 0; i < dev->param.nShortOpCaches; i++) {
			if (dev->srCache[i].object == in)
				dev->srCache[i].object = NULL;
=======
	if (selected) {
		yaffs_trace(YAFFS_TRACE_GC,
			"GC Selected block %d with %d free, prioritised:%d",
			selected,
			dev->param.chunks_per_block - dev->gc_pages_in_use,
			prioritised);

		dev->n_gc_blocks++;
		if (background)
			dev->bg_gcs++;

		dev->gc_dirtiest = 0;
		dev->gc_pages_in_use = 0;
		dev->gc_not_done = 0;
		if (dev->refresh_skip > 0)
			dev->refresh_skip--;
	} else {
		dev->gc_not_done++;
		yaffs_trace(YAFFS_TRACE_GC,
			"GC none: finder %d skip %d threshold %d dirtiest %d using %d oldest %d%s",
			dev->gc_block_finder, dev->gc_not_done, threshold,
			dev->gc_dirtiest, dev->gc_pages_in_use,
			dev->oldest_dirty_block, background ? " bg" : "");
	}

	return selected;
}

/* New garbage collector
 * If we're very low on erased blocks then we do aggressive garbage collection
 * otherwise we do "leasurely" garbage collection.
 * Aggressive gc looks further (whole array) and will accept less dirty blocks.
 * Passive gc only inspects smaller areas and only accepts more dirty blocks.
 *
 * The idea is to help clear out space in a more spread-out manner.
 * Dunno if it really does anything useful.
 */
static int yaffs_check_gc(struct yaffs_dev *dev, int background)
{
	int aggressive = 0;
	int gc_ok = YAFFS_OK;
	int max_tries = 0;
	int min_erased;
	int erased_chunks;
	int checkpt_block_adjust;

	if (dev->param.gc_control && (dev->param.gc_control(dev) & 1) == 0)
		return YAFFS_OK;

	if (dev->gc_disable)
		/* Bail out so we don't get recursive gc */
		return YAFFS_OK;

	/* This loop should pass the first time.
	 * Only loops here if the collection does not increase space.
	 */

	do {
		max_tries++;

		checkpt_block_adjust = yaffs_calc_checkpt_blocks_required(dev);

		min_erased =
		    dev->param.n_reserved_blocks + checkpt_block_adjust + 1;
		erased_chunks =
		    dev->n_erased_blocks * dev->param.chunks_per_block;

		/* If we need a block soon then do aggressive gc. */
		if (dev->n_erased_blocks < min_erased)
			aggressive = 1;
		else {
			if (!background
			    && erased_chunks > (dev->n_free_chunks / 4))
				break;

			if (dev->gc_skip > 20)
				dev->gc_skip = 20;
			if (erased_chunks < dev->n_free_chunks / 2 ||
			    dev->gc_skip < 1 || background)
				aggressive = 0;
			else {
				dev->gc_skip--;
				break;
			}
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		}

		dev->gc_skip = 5;

<<<<<<< HEAD
/*--------------------- File read/write ------------------------
 * Read and write have very similar structures.
 * In general the read/write has three parts to it
 * An incomplete chunk to start with (if the read/write is not chunk-aligned)
 * Some complete chunks
 * An incomplete chunk to end off with
 *
 * Curve-balls: the first chunk might also be the last chunk.
 */

int yaffs_ReadDataFromFile(yaffs_Object *in, __u8 *buffer, loff_t offset,
			int nBytes)
{

	int chunk;
	__u32 start;
	int nToCopy;
	int n = nBytes;
	int nDone = 0;
	yaffs_ChunkCache *cache;

	yaffs_Device *dev;

	dev = in->myDev;

	while (n > 0) {
		/* chunk = offset / dev->nDataBytesPerChunk + 1; */
		/* start = offset % dev->nDataBytesPerChunk; */
		yaffs_AddrToChunk(dev, offset, &chunk, &start);
		chunk++;

		/* OK now check for the curveball where the start and end are in
		 * the same chunk.
		 */
		if ((start + n) < dev->nDataBytesPerChunk)
			nToCopy = n;
		else
			nToCopy = dev->nDataBytesPerChunk - start;

		cache = yaffs_FindChunkCache(in, chunk);

		/* If the chunk is already in the cache or it is less than a whole chunk
		 * or we're using inband tags then use the cache (if there is caching)
		 * else bypass the cache.
		 */
		if (cache || nToCopy != dev->nDataBytesPerChunk || dev->param.inbandTags) {
			if (dev->param.nShortOpCaches > 0) {

				/* If we can't find the data in the cache, then load it up. */

				if (!cache) {
					cache = yaffs_GrabChunkCache(in->myDev);
					cache->object = in;
					cache->chunkId = chunk;
					cache->dirty = 0;
					cache->locked = 0;
					yaffs_ReadChunkDataFromObject(in, chunk,
								      cache->
								      data);
					cache->nBytes = 0;
				}

				yaffs_UseChunkCache(dev, cache, 0);

				cache->locked = 1;
=======
		/* If we don't already have a block being gc'd then see if we
		 * should start another */

		if (dev->gc_block < 1 && !aggressive) {
			dev->gc_block = yaffs2_find_refresh_block(dev);
			dev->gc_chunk = 0;
			dev->n_clean_ups = 0;
		}
		if (dev->gc_block < 1) {
			dev->gc_block =
			    yaffs_find_gc_block(dev, aggressive, background);
			dev->gc_chunk = 0;
			dev->n_clean_ups = 0;
		}

		if (dev->gc_block > 0) {
			dev->all_gcs++;
			if (!aggressive)
				dev->passive_gc_count++;

			yaffs_trace(YAFFS_TRACE_GC,
				"yaffs: GC n_erased_blocks %d aggressive %d",
				dev->n_erased_blocks, aggressive);

			gc_ok = yaffs_gc_block(dev, dev->gc_block, aggressive);
		}

		if (dev->n_erased_blocks < (dev->param.n_reserved_blocks) &&
		    dev->gc_block > 0) {
			yaffs_trace(YAFFS_TRACE_GC,
				"yaffs: GC !!!no reclaim!!! n_erased_blocks %d after try %d block %d",
				dev->n_erased_blocks, max_tries,
				dev->gc_block);
		}
	} while ((dev->n_erased_blocks < dev->param.n_reserved_blocks) &&
		 (dev->gc_block > 0) && (max_tries < 2));

	return aggressive ? gc_ok : YAFFS_OK;
}

/*
 * yaffs_bg_gc()
 * Garbage collects. Intended to be called from a background thread.
 * Returns non-zero if at least half the free chunks are erased.
 */
int yaffs_bg_gc(struct yaffs_dev *dev, unsigned urgency)
{
	int erased_chunks = dev->n_erased_blocks * dev->param.chunks_per_block;

	yaffs_trace(YAFFS_TRACE_BACKGROUND, "Background gc %u", urgency);

	yaffs_check_gc(dev, 1);
	return erased_chunks > dev->n_free_chunks / 2;
}

/*-------------------- Data file manipulation -----------------*/

static int yaffs_rd_data_obj(struct yaffs_obj *in, int inode_chunk, u8 * buffer)
{
	int nand_chunk = yaffs_find_chunk_in_file(in, inode_chunk, NULL);

	if (nand_chunk >= 0)
		return yaffs_rd_chunk_tags_nand(in->my_dev, nand_chunk,
						buffer, NULL);
	else {
		yaffs_trace(YAFFS_TRACE_NANDACCESS,
			"Chunk %d not found zero instead",
			nand_chunk);
		/* get sane (zero) data if you read a hole */
		memset(buffer, 0, in->my_dev->data_bytes_per_chunk);
		return 0;
	}

}
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

void yaffs_chunk_del(struct yaffs_dev *dev, int chunk_id, int mark_flash,
		     int lyn)
{
	int block;
	int page;
	struct yaffs_ext_tags tags;
	struct yaffs_block_info *bi;

<<<<<<< HEAD
				memcpy(buffer, &cache->data[start], nToCopy);

				cache->locked = 0;
			} else {
				/* Read into the local buffer then copy..*/

				__u8 *localBuffer =
				    yaffs_GetTempBuffer(dev, __LINE__);
				yaffs_ReadChunkDataFromObject(in, chunk,
							      localBuffer);

				memcpy(buffer, &localBuffer[start], nToCopy);


				yaffs_ReleaseTempBuffer(dev, localBuffer,
							__LINE__);
			}

		} else {

			/* A full chunk. Read directly into the supplied buffer. */
			yaffs_ReadChunkDataFromObject(in, chunk, buffer);

		}

		n -= nToCopy;
		offset += nToCopy;
		buffer += nToCopy;
		nDone += nToCopy;

	}

	return nDone;
}

int yaffs_DoWriteDataToFile(yaffs_Object *in, const __u8 *buffer, loff_t offset,
			int nBytes, int writeThrough)
{

	int chunk;
	__u32 start;
	int nToCopy;
	int n = nBytes;
	int nDone = 0;
	int nToWriteBack;
	int startOfWrite = offset;
	int chunkWritten = 0;
	__u32 nBytesRead;
	__u32 chunkStart;

	yaffs_Device *dev;

	dev = in->myDev;

	while (n > 0 && chunkWritten >= 0) {
		yaffs_AddrToChunk(dev, offset, &chunk, &start);

		if (chunk * dev->nDataBytesPerChunk + start != offset ||
				start >= dev->nDataBytesPerChunk) {
			T(YAFFS_TRACE_ERROR, (
			   TSTR("AddrToChunk of offset %d gives chunk %d start %d"
			   TENDSTR),
			   (int)offset, chunk, start));
		}
		chunk++; /* File pos to chunk in file offset */

		/* OK now check for the curveball where the start and end are in
		 * the same chunk.
		 */

		if ((start + n) < dev->nDataBytesPerChunk) {
			nToCopy = n;

			/* Now folks, to calculate how many bytes to write back....
			 * If we're overwriting and not writing to then end of file then
			 * we need to write back as much as was there before.
			 */

			chunkStart = ((chunk - 1) * dev->nDataBytesPerChunk);

			if (chunkStart > in->variant.fileVariant.fileSize)
				nBytesRead = 0; /* Past end of file */
			else
				nBytesRead = in->variant.fileVariant.fileSize - chunkStart;

			if (nBytesRead > dev->nDataBytesPerChunk)
				nBytesRead = dev->nDataBytesPerChunk;

			nToWriteBack =
			    (nBytesRead >
			     (start + n)) ? nBytesRead : (start + n);

			if (nToWriteBack < 0 || nToWriteBack > dev->nDataBytesPerChunk)
				YBUG();

		} else {
			nToCopy = dev->nDataBytesPerChunk - start;
			nToWriteBack = dev->nDataBytesPerChunk;
		}

		if (nToCopy != dev->nDataBytesPerChunk || dev->param.inbandTags) {
			/* An incomplete start or end chunk (or maybe both start and end chunk),
			 * or we're using inband tags, so we want to use the cache buffers.
			 */
			if (dev->param.nShortOpCaches > 0) {
				yaffs_ChunkCache *cache;
				/* If we can't find the data in the cache, then load the cache */
				cache = yaffs_FindChunkCache(in, chunk);

				if (!cache
				    && yaffs_CheckSpaceForAllocation(dev, 1)) {
					cache = yaffs_GrabChunkCache(dev);
					cache->object = in;
					cache->chunkId = chunk;
					cache->dirty = 0;
					cache->locked = 0;
					yaffs_ReadChunkDataFromObject(in, chunk,
								      cache->data);
				} else if (cache &&
					!cache->dirty &&
					!yaffs_CheckSpaceForAllocation(dev, 1)) {
					/* Drop the cache if it was a read cache item and
					 * no space check has been made for it.
					 */
					 cache = NULL;
				}

				if (cache) {
					yaffs_UseChunkCache(dev, cache, 1);
					cache->locked = 1;


					memcpy(&cache->data[start], buffer,
					       nToCopy);


					cache->locked = 0;
					cache->nBytes = nToWriteBack;

					if (writeThrough) {
						chunkWritten =
						    yaffs_WriteChunkDataToObject
						    (cache->object,
						     cache->chunkId,
						     cache->data, cache->nBytes,
						     1);
						cache->dirty = 0;
					}

				} else {
					chunkWritten = -1;	/* fail the write */
				}
			} else {
				/* An incomplete start or end chunk (or maybe both start and end chunk)
				 * Read into the local buffer then copy, then copy over and write back.
				 */

				__u8 *localBuffer =
				    yaffs_GetTempBuffer(dev, __LINE__);

				yaffs_ReadChunkDataFromObject(in, chunk,
							      localBuffer);



				memcpy(&localBuffer[start], buffer, nToCopy);

				chunkWritten =
				    yaffs_WriteChunkDataToObject(in, chunk,
								 localBuffer,
								 nToWriteBack,
								 0);

				yaffs_ReleaseTempBuffer(dev, localBuffer,
							__LINE__);

			}

		} else {
			/* A full chunk. Write directly from the supplied buffer. */



			chunkWritten =
			    yaffs_WriteChunkDataToObject(in, chunk, buffer,
							 dev->nDataBytesPerChunk,
							 0);

			/* Since we've overwritten the cached data, we better invalidate it. */
			yaffs_InvalidateChunkCache(in, chunk);
		}

		if (chunkWritten >= 0) {
			n -= nToCopy;
			offset += nToCopy;
			buffer += nToCopy;
			nDone += nToCopy;
		}
=======
	if (chunk_id <= 0)
		return;

	dev->n_deletions++;
	block = chunk_id / dev->param.chunks_per_block;
	page = chunk_id % dev->param.chunks_per_block;

	if (!yaffs_check_chunk_bit(dev, block, page))
		yaffs_trace(YAFFS_TRACE_VERIFY,
			"Deleting invalid chunk %d", chunk_id);

	bi = yaffs_get_block_info(dev, block);

	yaffs2_update_oldest_dirty_seq(dev, block, bi);

	yaffs_trace(YAFFS_TRACE_DELETION,
		"line %d delete of chunk %d",
		lyn, chunk_id);

	if (!dev->param.is_yaffs2 && mark_flash &&
	    bi->block_state != YAFFS_BLOCK_STATE_COLLECTING) {

		memset(&tags, 0, sizeof(tags));
		tags.is_deleted = 1;
		yaffs_wr_chunk_tags_nand(dev, chunk_id, NULL, &tags);
		yaffs_handle_chunk_update(dev, chunk_id, &tags);
	} else {
		dev->n_unmarked_deletions++;
	}

	/* Pull out of the management area.
	 * If the whole block became dirty, this will kick off an erasure.
	 */
	if (bi->block_state == YAFFS_BLOCK_STATE_ALLOCATING ||
	    bi->block_state == YAFFS_BLOCK_STATE_FULL ||
	    bi->block_state == YAFFS_BLOCK_STATE_NEEDS_SCAN ||
	    bi->block_state == YAFFS_BLOCK_STATE_COLLECTING) {
		dev->n_free_chunks++;
		yaffs_clear_chunk_bit(dev, block, page);
		bi->pages_in_use--;

		if (bi->pages_in_use == 0 &&
		    !bi->has_shrink_hdr &&
		    bi->block_state != YAFFS_BLOCK_STATE_ALLOCATING &&
		    bi->block_state != YAFFS_BLOCK_STATE_NEEDS_SCAN) {
			yaffs_block_became_dirty(dev, block);
		}
	}
}

static int yaffs_wr_data_obj(struct yaffs_obj *in, int inode_chunk,
			     const u8 *buffer, int n_bytes, int use_reserve)
{
	/* Find old chunk Need to do this to get serial number
	 * Write new one and patch into tree.
	 * Invalidate old tags.
	 */

	int prev_chunk_id;
	struct yaffs_ext_tags prev_tags;
	int new_chunk_id;
	struct yaffs_ext_tags new_tags;
	struct yaffs_dev *dev = in->my_dev;

	yaffs_check_gc(dev, 0);

	/* Get the previous chunk at this location in the file if it exists.
	 * If it does not exist then put a zero into the tree. This creates
	 * the tnode now, rather than later when it is harder to clean up.
	 */
	prev_chunk_id = yaffs_find_chunk_in_file(in, inode_chunk, &prev_tags);
	if (prev_chunk_id < 1 &&
	    !yaffs_put_chunk_in_file(in, inode_chunk, 0, 0))
		return 0;

	/* Set up new tags */
	memset(&new_tags, 0, sizeof(new_tags));

	new_tags.chunk_id = inode_chunk;
	new_tags.obj_id = in->obj_id;
	new_tags.serial_number =
	    (prev_chunk_id > 0) ? prev_tags.serial_number + 1 : 1;
	new_tags.n_bytes = n_bytes;

	if (n_bytes < 1 || n_bytes > dev->param.total_bytes_per_chunk) {
		yaffs_trace(YAFFS_TRACE_ERROR,
		  "Writing %d bytes to chunk!!!!!!!!!",
		   n_bytes);
		BUG();
	}

	new_chunk_id =
	    yaffs_write_new_chunk(dev, buffer, &new_tags, use_reserve);

	if (new_chunk_id > 0) {
		yaffs_put_chunk_in_file(in, inode_chunk, new_chunk_id, 0);

		if (prev_chunk_id > 0)
			yaffs_chunk_del(dev, prev_chunk_id, 1, __LINE__);

		yaffs_verify_file_sane(in);
	}
	return new_chunk_id;

}



static int yaffs_do_xattrib_mod(struct yaffs_obj *obj, int set,
				const YCHAR *name, const void *value, int size,
				int flags)
{
	struct yaffs_xattr_mod xmod;
	int result;

	xmod.set = set;
	xmod.name = name;
	xmod.data = value;
	xmod.size = size;
	xmod.flags = flags;
	xmod.result = -ENOSPC;

	result = yaffs_update_oh(obj, NULL, 0, 0, 0, &xmod);

	if (result > 0)
		return xmod.result;
	else
		return -ENOSPC;
}

static int yaffs_apply_xattrib_mod(struct yaffs_obj *obj, char *buffer,
				   struct yaffs_xattr_mod *xmod)
{
	int retval = 0;
	int x_offs = sizeof(struct yaffs_obj_hdr);
	struct yaffs_dev *dev = obj->my_dev;
	int x_size = dev->data_bytes_per_chunk - sizeof(struct yaffs_obj_hdr);
	char *x_buffer = buffer + x_offs;

	if (xmod->set)
		retval =
		    nval_set(x_buffer, x_size, xmod->name, xmod->data,
			     xmod->size, xmod->flags);
	else
		retval = nval_del(x_buffer, x_size, xmod->name);

	obj->has_xattr = nval_hasvalues(x_buffer, x_size);
	obj->xattr_known = 1;
	xmod->result = retval;

	return retval;
}

static int yaffs_do_xattrib_fetch(struct yaffs_obj *obj, const YCHAR *name,
				  void *value, int size)
{
	char *buffer = NULL;
	int result;
	struct yaffs_ext_tags tags;
	struct yaffs_dev *dev = obj->my_dev;
	int x_offs = sizeof(struct yaffs_obj_hdr);
	int x_size = dev->data_bytes_per_chunk - sizeof(struct yaffs_obj_hdr);
	char *x_buffer;
	int retval = 0;

	if (obj->hdr_chunk < 1)
		return -ENODATA;

	/* If we know that the object has no xattribs then don't do all the
	 * reading and parsing.
	 */
	if (obj->xattr_known && !obj->has_xattr) {
		if (name)
			return -ENODATA;
		else
			return 0;
	}

	buffer = (char *)yaffs_get_temp_buffer(dev);
	if (!buffer)
		return -ENOMEM;

	result =
	    yaffs_rd_chunk_tags_nand(dev, obj->hdr_chunk, (u8 *) buffer, &tags);

	if (result != YAFFS_OK)
		retval = -ENOENT;
	else {
		x_buffer = buffer + x_offs;

		if (!obj->xattr_known) {
			obj->has_xattr = nval_hasvalues(x_buffer, x_size);
			obj->xattr_known = 1;
		}

		if (name)
			retval = nval_get(x_buffer, x_size, name, value, size);
		else
			retval = nval_list(x_buffer, x_size, value, size);
	}
	yaffs_release_temp_buffer(dev, (u8 *) buffer);
	return retval;
}

int yaffs_set_xattrib(struct yaffs_obj *obj, const YCHAR * name,
		      const void *value, int size, int flags)
{
	return yaffs_do_xattrib_mod(obj, 1, name, value, size, flags);
}

int yaffs_remove_xattrib(struct yaffs_obj *obj, const YCHAR * name)
{
	return yaffs_do_xattrib_mod(obj, 0, name, NULL, 0, 0);
}

int yaffs_get_xattrib(struct yaffs_obj *obj, const YCHAR * name, void *value,
		      int size)
{
	return yaffs_do_xattrib_fetch(obj, name, value, size);
}

int yaffs_list_xattrib(struct yaffs_obj *obj, char *buffer, int size)
{
	return yaffs_do_xattrib_fetch(obj, NULL, buffer, size);
}

static void yaffs_check_obj_details_loaded(struct yaffs_obj *in)
{
	u8 *buf;
	struct yaffs_obj_hdr *oh;
	struct yaffs_dev *dev;
	struct yaffs_ext_tags tags;
	int result;
	int alloc_failed = 0;

	if (!in || !in->lazy_loaded || in->hdr_chunk < 1)
		return;

	dev = in->my_dev;
	in->lazy_loaded = 0;
	buf = yaffs_get_temp_buffer(dev);

	result = yaffs_rd_chunk_tags_nand(dev, in->hdr_chunk, buf, &tags);
	oh = (struct yaffs_obj_hdr *)buf;

	in->yst_mode = oh->yst_mode;
	yaffs_load_attribs(in, oh);
	yaffs_set_obj_name_from_oh(in, oh);

	if (in->variant_type == YAFFS_OBJECT_TYPE_SYMLINK) {
		in->variant.symlink_variant.alias =
		    yaffs_clone_str(oh->alias);
		if (!in->variant.symlink_variant.alias)
			alloc_failed = 1;	/* Not returned */
	}
	yaffs_release_temp_buffer(dev, buf);
}

static void yaffs_load_name_from_oh(struct yaffs_dev *dev, YCHAR *name,
				    const YCHAR *oh_name, int buff_size)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE
	if (dev->param.auto_unicode) {
		if (*oh_name) {
			/* It is an ASCII name, do an ASCII to
			 * unicode conversion */
			const char *ascii_oh_name = (const char *)oh_name;
			int n = buff_size - 1;
			while (n > 0 && *ascii_oh_name) {
				*name = *ascii_oh_name;
				name++;
				ascii_oh_name++;
				n--;
			}
		} else {
			strncpy(name, oh_name + 1, buff_size - 1);
		}
	} else {
#else
	{
#endif
		strncpy(name, oh_name, buff_size - 1);
	}
}

static void yaffs_load_oh_from_name(struct yaffs_dev *dev, YCHAR *oh_name,
				    const YCHAR *name)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE

	int is_ascii;
	YCHAR *w;

	if (dev->param.auto_unicode) {

		is_ascii = 1;
		w = name;

		/* Figure out if the name will fit in ascii character set */
		while (is_ascii && *w) {
			if ((*w) & 0xff00)
				is_ascii = 0;
			w++;
		}

		if (is_ascii) {
			/* It is an ASCII name, so convert unicode to ascii */
			char *ascii_oh_name = (char *)oh_name;
			int n = YAFFS_MAX_NAME_LENGTH - 1;
			while (n > 0 && *name) {
				*ascii_oh_name = *name;
				name++;
				ascii_oh_name++;
				n--;
			}
		} else {
			/* Unicode name, so save starting at the second YCHAR */
			*oh_name = 0;
			strncpy(oh_name + 1, name, YAFFS_MAX_NAME_LENGTH - 2);
		}
	} else {
#else
	{
#endif
		strncpy(oh_name, name, YAFFS_MAX_NAME_LENGTH - 1);
	}
}

/* UpdateObjectHeader updates the header on NAND for an object.
 * If name is not NULL, then that new name is used.
 */
int yaffs_update_oh(struct yaffs_obj *in, const YCHAR *name, int force,
		    int is_shrink, int shadows, struct yaffs_xattr_mod *xmod)
{

	struct yaffs_block_info *bi;
	struct yaffs_dev *dev = in->my_dev;
	int prev_chunk_id;
	int ret_val = 0;
	int result = 0;
	int new_chunk_id;
	struct yaffs_ext_tags new_tags;
	struct yaffs_ext_tags old_tags;
	const YCHAR *alias = NULL;
	u8 *buffer = NULL;
	YCHAR old_name[YAFFS_MAX_NAME_LENGTH + 1];
	struct yaffs_obj_hdr *oh = NULL;

	strcpy(old_name, _Y("silly old name"));

	if (in->fake && in != dev->root_dir && !force && !xmod)
		return ret_val;

	yaffs_check_gc(dev, 0);
	yaffs_check_obj_details_loaded(in);

	buffer = yaffs_get_temp_buffer(in->my_dev);
	oh = (struct yaffs_obj_hdr *)buffer;

	prev_chunk_id = in->hdr_chunk;

	if (prev_chunk_id > 0) {
		result = yaffs_rd_chunk_tags_nand(dev, prev_chunk_id,
						  buffer, &old_tags);

		yaffs_verify_oh(in, oh, &old_tags, 0);
		memcpy(old_name, oh->name, sizeof(oh->name));
		memset(buffer, 0xff, sizeof(struct yaffs_obj_hdr));
	} else {
		memset(buffer, 0xff, dev->data_bytes_per_chunk);
	}

	oh->type = in->variant_type;
	oh->yst_mode = in->yst_mode;
	oh->shadows_obj = oh->inband_shadowed_obj_id = shadows;

	yaffs_load_attribs_oh(oh, in);

	if (in->parent)
		oh->parent_obj_id = in->parent->obj_id;
	else
		oh->parent_obj_id = 0;

	if (name && *name) {
		memset(oh->name, 0, sizeof(oh->name));
		yaffs_load_oh_from_name(dev, oh->name, name);
	} else if (prev_chunk_id > 0) {
		memcpy(oh->name, old_name, sizeof(oh->name));
	} else {
		memset(oh->name, 0, sizeof(oh->name));
	}

	oh->is_shrink = is_shrink;

	switch (in->variant_type) {
	case YAFFS_OBJECT_TYPE_UNKNOWN:
		/* Should not happen */
		break;
	case YAFFS_OBJECT_TYPE_FILE:
		oh->file_size =
		    (oh->parent_obj_id == YAFFS_OBJECTID_DELETED ||
		     oh->parent_obj_id == YAFFS_OBJECTID_UNLINKED) ?
		     0 : in->variant.file_variant.file_size;
		break;
	case YAFFS_OBJECT_TYPE_HARDLINK:
		oh->equiv_id = in->variant.hardlink_variant.equiv_id;
		break;
	case YAFFS_OBJECT_TYPE_SPECIAL:
		/* Do nothing */
		break;
	case YAFFS_OBJECT_TYPE_DIRECTORY:
		/* Do nothing */
		break;
	case YAFFS_OBJECT_TYPE_SYMLINK:
		alias = in->variant.symlink_variant.alias;
		if (!alias)
			alias = _Y("no alias");
		strncpy(oh->alias, alias, YAFFS_MAX_ALIAS_LENGTH);
		oh->alias[YAFFS_MAX_ALIAS_LENGTH] = 0;
		break;
	}

	/* process any xattrib modifications */
	if (xmod)
		yaffs_apply_xattrib_mod(in, (char *)buffer, xmod);

	/* Tags */
	memset(&new_tags, 0, sizeof(new_tags));
	in->serial++;
	new_tags.chunk_id = 0;
	new_tags.obj_id = in->obj_id;
	new_tags.serial_number = in->serial;

	/* Add extra info for file header */
	new_tags.extra_available = 1;
	new_tags.extra_parent_id = oh->parent_obj_id;
	new_tags.extra_length = oh->file_size;
	new_tags.extra_is_shrink = oh->is_shrink;
	new_tags.extra_equiv_id = oh->equiv_id;
	new_tags.extra_shadows = (oh->shadows_obj > 0) ? 1 : 0;
	new_tags.extra_obj_type = in->variant_type;
	yaffs_verify_oh(in, oh, &new_tags, 1);

	/* Create new chunk in NAND */
	new_chunk_id =
	    yaffs_write_new_chunk(dev, buffer, &new_tags,
				  (prev_chunk_id > 0) ? 1 : 0);

	if (buffer)
		yaffs_release_temp_buffer(dev, buffer);

	if (new_chunk_id < 0)
		return new_chunk_id;

	in->hdr_chunk = new_chunk_id;

	if (prev_chunk_id > 0)
		yaffs_chunk_del(dev, prev_chunk_id, 1, __LINE__);

	if (!yaffs_obj_cache_dirty(in))
		in->dirty = 0;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	/* If this was a shrink, then mark the block
	 * that the chunk lives on */
	if (is_shrink) {
		bi = yaffs_get_block_info(in->my_dev,
					  new_chunk_id /
					  in->my_dev->param.chunks_per_block);
		bi->has_shrink_hdr = 1;
	}

	/* Update file object */

<<<<<<< HEAD
	if ((startOfWrite + nDone) > in->variant.fileVariant.fileSize)
		in->variant.fileVariant.fileSize = (startOfWrite + nDone);

	in->dirty = 1;

	return nDone;
}

int yaffs_WriteDataToFile(yaffs_Object *in, const __u8 *buffer, loff_t offset,
			int nBytes, int writeThrough)
{
	yaffs2_HandleHole(in,offset);
	return yaffs_DoWriteDataToFile(in,buffer,offset,nBytes,writeThrough);
}



/* ---------------------- File resizing stuff ------------------ */

static void yaffs_PruneResizedChunks(yaffs_Object *in, int newSize)
{

	yaffs_Device *dev = in->myDev;
	int oldFileSize = in->variant.fileVariant.fileSize;

	int lastDel = 1 + (oldFileSize - 1) / dev->nDataBytesPerChunk;

	int startDel = 1 + (newSize + dev->nDataBytesPerChunk - 1) /
	    dev->nDataBytesPerChunk;
	int i;
	int chunkId;

	/* Delete backwards so that we don't end up with holes if
	 * power is lost part-way through the operation.
	 */
	for (i = lastDel; i >= startDel; i--) {
		/* NB this could be optimised somewhat,
		 * eg. could retrieve the tags and write them without
		 * using yaffs_DeleteChunk
		 */

		chunkId = yaffs_FindAndDeleteChunkInFile(in, i, NULL);
		if (chunkId > 0) {
			if (chunkId <
			    (dev->internalStartBlock * dev->param.nChunksPerBlock)
			    || chunkId >=
			    ((dev->internalEndBlock +
			      1) * dev->param.nChunksPerBlock)) {
				T(YAFFS_TRACE_ALWAYS,
				  (TSTR("Found daft chunkId %d for %d" TENDSTR),
				   chunkId, i));
			} else {
				in->nDataChunks--;
				yaffs_DeleteChunk(dev, chunkId, 1, __LINE__);
			}
		}
	}

}


void yaffs_ResizeDown( yaffs_Object *obj, loff_t newSize)
{
	int newFullChunks;
	__u32 newSizeOfPartialChunk;
	yaffs_Device *dev = obj->myDev;

	yaffs_AddrToChunk(dev, newSize, &newFullChunks, &newSizeOfPartialChunk);

	yaffs_PruneResizedChunks(obj, newSize);

	if (newSizeOfPartialChunk != 0) {
		int lastChunk = 1 + newFullChunks;
		__u8 *localBuffer = yaffs_GetTempBuffer(dev, __LINE__);

		/* Got to read and rewrite the last chunk with its new size and zero pad */
		yaffs_ReadChunkDataFromObject(obj, lastChunk, localBuffer);
		memset(localBuffer + newSizeOfPartialChunk, 0,
			dev->nDataBytesPerChunk - newSizeOfPartialChunk);

		yaffs_WriteChunkDataToObject(obj, lastChunk, localBuffer,
					     newSizeOfPartialChunk, 1);

		yaffs_ReleaseTempBuffer(dev, localBuffer, __LINE__);
	}

	obj->variant.fileVariant.fileSize = newSize;

	yaffs_PruneFileStructure(dev, &obj->variant.fileVariant);
}


int yaffs_ResizeFile(yaffs_Object *in, loff_t newSize)
{
	yaffs_Device *dev = in->myDev;
	int oldFileSize = in->variant.fileVariant.fileSize;

	yaffs_FlushFilesChunkCache(in);
	yaffs_InvalidateWholeChunkCache(in);

	yaffs_CheckGarbageCollection(dev,0);

	if (in->variantType != YAFFS_OBJECT_TYPE_FILE)
		return YAFFS_FAIL;

	if (newSize == oldFileSize)
		return YAFFS_OK;
		
	if(newSize > oldFileSize){
		yaffs2_HandleHole(in,newSize);
		in->variant.fileVariant.fileSize = newSize;
	} else {
		/* newSize < oldFileSize */ 
		yaffs_ResizeDown(in, newSize);
	} 

	/* Write a new object header to reflect the resize.
	 * show we've shrunk the file, if need be
	 * Do this only if the file is not in the deleted directories
	 * and is not shadowed.
	 */
	if (in->parent &&
	    !in->isShadowed &&
	    in->parent->objectId != YAFFS_OBJECTID_UNLINKED &&
	    in->parent->objectId != YAFFS_OBJECTID_DELETED)
		yaffs_UpdateObjectHeader(in, NULL, 0, 0, 0, NULL);


	return YAFFS_OK;
}

loff_t yaffs_GetFileSize(yaffs_Object *obj)
{
	YCHAR *alias = NULL;
	obj = yaffs_GetEquivalentObject(obj);

	switch (obj->variantType) {
	case YAFFS_OBJECT_TYPE_FILE:
		return obj->variant.fileVariant.fileSize;
	case YAFFS_OBJECT_TYPE_SYMLINK:
		alias = obj->variant.symLinkVariant.alias;
		if(!alias)
			return 0;
		return yaffs_strnlen(alias,YAFFS_MAX_ALIAS_LENGTH);
	default:
		return 0;
	}
}



int yaffs_FlushFile(yaffs_Object *in, int updateTime, int dataSync)
{
	int retVal;
	if (in->dirty) {
		yaffs_FlushFilesChunkCache(in);
		if(dataSync) /* Only sync data */
			retVal=YAFFS_OK;
		else {
			if (updateTime) {
#ifdef CONFIG_YAFFS_WINCE
				yfsd_WinFileTimeNow(in->win_mtime);
#else

				in->yst_mtime = Y_CURRENT_TIME;

#endif
			}

			retVal = (yaffs_UpdateObjectHeader(in, NULL, 0, 0, 0, NULL) >=
				0) ? YAFFS_OK : YAFFS_FAIL;
=======
	return new_chunk_id;
}

/*--------------------- File read/write ------------------------
 * Read and write have very similar structures.
 * In general the read/write has three parts to it
 * An incomplete chunk to start with (if the read/write is not chunk-aligned)
 * Some complete chunks
 * An incomplete chunk to end off with
 *
 * Curve-balls: the first chunk might also be the last chunk.
 */

int yaffs_file_rd(struct yaffs_obj *in, u8 * buffer, loff_t offset, int n_bytes)
{
	int chunk;
	u32 start;
	int n_copy;
	int n = n_bytes;
	int n_done = 0;
	struct yaffs_cache *cache;
	struct yaffs_dev *dev;

	dev = in->my_dev;

	while (n > 0) {
		yaffs_addr_to_chunk(dev, offset, &chunk, &start);
		chunk++;

		/* OK now check for the curveball where the start and end are in
		 * the same chunk.
		 */
		if ((start + n) < dev->data_bytes_per_chunk)
			n_copy = n;
		else
			n_copy = dev->data_bytes_per_chunk - start;

		cache = yaffs_find_chunk_cache(in, chunk);

		/* If the chunk is already in the cache or it is less than
		 * a whole chunk or we're using inband tags then use the cache
		 * (if there is caching) else bypass the cache.
		 */
		if (cache || n_copy != dev->data_bytes_per_chunk ||
		    dev->param.inband_tags) {
			if (dev->param.n_caches > 0) {

				/* If we can't find the data in the cache,
				 * then load it up. */

				if (!cache) {
					cache =
					    yaffs_grab_chunk_cache(in->my_dev);
					cache->object = in;
					cache->chunk_id = chunk;
					cache->dirty = 0;
					cache->locked = 0;
					yaffs_rd_data_obj(in, chunk,
							  cache->data);
					cache->n_bytes = 0;
				}

				yaffs_use_cache(dev, cache, 0);

				cache->locked = 1;

				memcpy(buffer, &cache->data[start], n_copy);

				cache->locked = 0;
			} else {
				/* Read into the local buffer then copy.. */

				u8 *local_buffer =
				    yaffs_get_temp_buffer(dev);
				yaffs_rd_data_obj(in, chunk, local_buffer);

				memcpy(buffer, &local_buffer[start], n_copy);

				yaffs_release_temp_buffer(dev, local_buffer);
			}
		} else {
			/* A full chunk. Read directly into the buffer. */
			yaffs_rd_data_obj(in, chunk, buffer);
		}
		n -= n_copy;
		offset += n_copy;
		buffer += n_copy;
		n_done += n_copy;
	}
	return n_done;
}

int yaffs_do_file_wr(struct yaffs_obj *in, const u8 *buffer, loff_t offset,
		     int n_bytes, int write_trhrough)
{

	int chunk;
	u32 start;
	int n_copy;
	int n = n_bytes;
	int n_done = 0;
	int n_writeback;
	int start_write = offset;
	int chunk_written = 0;
	u32 n_bytes_read;
	u32 chunk_start;
	struct yaffs_dev *dev;

	dev = in->my_dev;

	while (n > 0 && chunk_written >= 0) {
		yaffs_addr_to_chunk(dev, offset, &chunk, &start);

		if (chunk * dev->data_bytes_per_chunk + start != offset ||
		    start >= dev->data_bytes_per_chunk) {
			yaffs_trace(YAFFS_TRACE_ERROR,
				"AddrToChunk of offset %d gives chunk %d start %d",
				(int)offset, chunk, start);
		}
		chunk++;	/* File pos to chunk in file offset */

		/* OK now check for the curveball where the start and end are in
		 * the same chunk.
		 */

		if ((start + n) < dev->data_bytes_per_chunk) {
			n_copy = n;

			/* Now calculate how many bytes to write back....
			 * If we're overwriting and not writing to then end of
			 * file then we need to write back as much as was there
			 * before.
			 */

			chunk_start = ((chunk - 1) * dev->data_bytes_per_chunk);

			if (chunk_start > in->variant.file_variant.file_size)
				n_bytes_read = 0;	/* Past end of file */
			else
				n_bytes_read =
				    in->variant.file_variant.file_size -
				    chunk_start;

			if (n_bytes_read > dev->data_bytes_per_chunk)
				n_bytes_read = dev->data_bytes_per_chunk;

			n_writeback =
			    (n_bytes_read >
			     (start + n)) ? n_bytes_read : (start + n);

			if (n_writeback < 0 ||
			    n_writeback > dev->data_bytes_per_chunk)
				BUG();

		} else {
			n_copy = dev->data_bytes_per_chunk - start;
			n_writeback = dev->data_bytes_per_chunk;
		}

		if (n_copy != dev->data_bytes_per_chunk ||
		    dev->param.inband_tags) {
			/* An incomplete start or end chunk (or maybe both
			 * start and end chunk), or we're using inband tags,
			 * so we want to use the cache buffers.
			 */
			if (dev->param.n_caches > 0) {
				struct yaffs_cache *cache;

				/* If we can't find the data in the cache, then
				 * load the cache */
				cache = yaffs_find_chunk_cache(in, chunk);

				if (!cache &&
				    yaffs_check_alloc_available(dev, 1)) {
					cache = yaffs_grab_chunk_cache(dev);
					cache->object = in;
					cache->chunk_id = chunk;
					cache->dirty = 0;
					cache->locked = 0;
					yaffs_rd_data_obj(in, chunk,
							  cache->data);
				} else if (cache &&
					   !cache->dirty &&
					   !yaffs_check_alloc_available(dev,
									1)) {
					/* Drop the cache if it was a read cache
					 * item and no space check has been made
					 * for it.
					 */
					cache = NULL;
				}

				if (cache) {
					yaffs_use_cache(dev, cache, 1);
					cache->locked = 1;

					memcpy(&cache->data[start], buffer,
					       n_copy);

					cache->locked = 0;
					cache->n_bytes = n_writeback;

					if (write_trhrough) {
						chunk_written =
						    yaffs_wr_data_obj
						    (cache->object,
						     cache->chunk_id,
						     cache->data,
						     cache->n_bytes, 1);
						cache->dirty = 0;
					}
				} else {
					chunk_written = -1;	/* fail write */
				}
			} else {
				/* An incomplete start or end chunk (or maybe
				 * both start and end chunk). Read into the
				 * local buffer then copy over and write back.
				 */

				u8 *local_buffer = yaffs_get_temp_buffer(dev);

				yaffs_rd_data_obj(in, chunk, local_buffer);
				memcpy(&local_buffer[start], buffer, n_copy);

				chunk_written =
				    yaffs_wr_data_obj(in, chunk,
						      local_buffer,
						      n_writeback, 0);

				yaffs_release_temp_buffer(dev, local_buffer);
			}
		} else {
			/* A full chunk. Write directly from the buffer. */

			chunk_written =
			    yaffs_wr_data_obj(in, chunk, buffer,
					      dev->data_bytes_per_chunk, 0);

			/* Since we've overwritten the cached data,
			 * we better invalidate it. */
			yaffs_invalidate_chunk_cache(in, chunk);
		}

		if (chunk_written >= 0) {
			n -= n_copy;
			offset += n_copy;
			buffer += n_copy;
			n_done += n_copy;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		}
	} else {
		retVal = YAFFS_OK;
	}

<<<<<<< HEAD
	return retVal;

}

static int yaffs_DoGenericObjectDeletion(yaffs_Object *in)
{

	/* First off, invalidate the file's data in the cache, without flushing. */
	yaffs_InvalidateWholeChunkCache(in);

	if (in->myDev->param.isYaffs2 && (in->parent != in->myDev->deletedDir)) {
		/* Move to the unlinked directory so we have a record that it was deleted. */
		yaffs_ChangeObjectName(in, in->myDev->deletedDir, _Y("deleted"), 0, 0);

	}

	yaffs_RemoveObjectFromDirectory(in);
	yaffs_DeleteChunk(in->myDev, in->hdrChunk, 1, __LINE__);
	in->hdrChunk = 0;

	yaffs_FreeObject(in);
	return YAFFS_OK;

}

/* yaffs_DeleteFile deletes the whole file data
 * and the inode associated with the file.
 * It does not delete the links associated with the file.
 */
static int yaffs_UnlinkFileIfNeeded(yaffs_Object *in)
{

	int retVal;
	int immediateDeletion = 0;
	yaffs_Device *dev = in->myDev;

	if (!in->myInode)
		immediateDeletion = 1;

	if (immediateDeletion) {
		retVal =
		    yaffs_ChangeObjectName(in, in->myDev->deletedDir,
					   _Y("deleted"), 0, 0);
		T(YAFFS_TRACE_TRACING,
		  (TSTR("yaffs: immediate deletion of file %d" TENDSTR),
		   in->objectId));
		in->deleted = 1;
		in->myDev->nDeletedFiles++;
		if (dev->param.disableSoftDelete || dev->param.isYaffs2)
			yaffs_ResizeFile(in, 0);
		yaffs_SoftDeleteFile(in);
	} else {
		retVal =
		    yaffs_ChangeObjectName(in, in->myDev->unlinkedDir,
					   _Y("unlinked"), 0, 0);
	}


	return retVal;
}

int yaffs_DeleteFile(yaffs_Object *in)
{
	int retVal = YAFFS_OK;
	int deleted; /* Need to cache value on stack if in is freed */
	yaffs_Device *dev = in->myDev;

	if (dev->param.disableSoftDelete || dev->param.isYaffs2)
		yaffs_ResizeFile(in, 0);

	if (in->nDataChunks > 0) {
		/* Use soft deletion if there is data in the file.
		 * That won't be the case if it has been resized to zero.
		 */
		if (!in->unlinked)
			retVal = yaffs_UnlinkFileIfNeeded(in);

		deleted = in->deleted;

		if (retVal == YAFFS_OK && in->unlinked && !in->deleted) {
			in->deleted = 1;
			deleted = 1;
			in->myDev->nDeletedFiles++;
			yaffs_SoftDeleteFile(in);
		}
		return deleted ? YAFFS_OK : YAFFS_FAIL;
	} else {
		/* The file has no data chunks so we toss it immediately */
		yaffs_FreeTnode(in->myDev, in->variant.fileVariant.top);
		in->variant.fileVariant.top = NULL;
		yaffs_DoGenericObjectDeletion(in);

		return YAFFS_OK;
	}
}

static int yaffs_IsNonEmptyDirectory(yaffs_Object *obj)
{
	return (obj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY) &&
		!(ylist_empty(&obj->variant.directoryVariant.children));
}

static int yaffs_DeleteDirectory(yaffs_Object *obj)
{
	/* First check that the directory is empty. */
	if (yaffs_IsNonEmptyDirectory(obj))
		return YAFFS_FAIL;

	return yaffs_DoGenericObjectDeletion(obj);
}

static int yaffs_DeleteSymLink(yaffs_Object *in)
{
	if(in->variant.symLinkVariant.alias)
		YFREE(in->variant.symLinkVariant.alias);
	in->variant.symLinkVariant.alias=NULL;

	return yaffs_DoGenericObjectDeletion(in);
}

static int yaffs_DeleteHardLink(yaffs_Object *in)
{
	/* remove this hardlink from the list assocaited with the equivalent
	 * object
	 */
	ylist_del_init(&in->hardLinks);
	return yaffs_DoGenericObjectDeletion(in);
}

int yaffs_DeleteObject(yaffs_Object *obj)
{
int retVal = -1;
	switch (obj->variantType) {
	case YAFFS_OBJECT_TYPE_FILE:
		retVal = yaffs_DeleteFile(obj);
		break;
	case YAFFS_OBJECT_TYPE_DIRECTORY:
		if(!ylist_empty(&obj->variant.directoryVariant.dirty)){
			T(YAFFS_TRACE_BACKGROUND, (TSTR("Remove object %d from dirty directories" TENDSTR),obj->objectId));
			ylist_del_init(&obj->variant.directoryVariant.dirty);
		}
		return yaffs_DeleteDirectory(obj);
		break;
	case YAFFS_OBJECT_TYPE_SYMLINK:
		retVal = yaffs_DeleteSymLink(obj);
		break;
	case YAFFS_OBJECT_TYPE_HARDLINK:
		retVal = yaffs_DeleteHardLink(obj);
		break;
	case YAFFS_OBJECT_TYPE_SPECIAL:
		retVal = yaffs_DoGenericObjectDeletion(obj);
		break;
	case YAFFS_OBJECT_TYPE_UNKNOWN:
		retVal = 0;
		break;		/* should not happen. */
	}

	return retVal;
}

static int yaffs_UnlinkWorker(yaffs_Object *obj)
{

	int immediateDeletion = 0;

	if (!obj->myInode)
		immediateDeletion = 1;

	if(obj)
		yaffs_UpdateParent(obj->parent);

	if (obj->variantType == YAFFS_OBJECT_TYPE_HARDLINK) {
		return yaffs_DeleteHardLink(obj);
	} else if (!ylist_empty(&obj->hardLinks)) {
		/* Curve ball: We're unlinking an object that has a hardlink.
		 *
		 * This problem arises because we are not strictly following
		 * The Linux link/inode model.
		 *
		 * We can't really delete the object.
		 * Instead, we do the following:
		 * - Select a hardlink.
		 * - Unhook it from the hard links
		 * - Move it from its parent directory (so that the rename can work)
		 * - Rename the object to the hardlink's name.
		 * - Delete the hardlink
		 */

		yaffs_Object *hl;
		yaffs_Object *parent;
		int retVal;
		YCHAR name[YAFFS_MAX_NAME_LENGTH + 1];

		hl = ylist_entry(obj->hardLinks.next, yaffs_Object, hardLinks);

		yaffs_GetObjectName(hl, name, YAFFS_MAX_NAME_LENGTH + 1);
		parent = hl->parent;

		ylist_del_init(&hl->hardLinks);

 		yaffs_AddObjectToDirectory(obj->myDev->unlinkedDir, hl);

		retVal = yaffs_ChangeObjectName(obj,parent, name, 0, 0);

		if (retVal == YAFFS_OK)
			retVal = yaffs_DoGenericObjectDeletion(hl);

		return retVal;

	} else if (immediateDeletion) {
		switch (obj->variantType) {
		case YAFFS_OBJECT_TYPE_FILE:
			return yaffs_DeleteFile(obj);
			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			ylist_del_init(&obj->variant.directoryVariant.dirty);
			return yaffs_DeleteDirectory(obj);
			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
			return yaffs_DeleteSymLink(obj);
			break;
		case YAFFS_OBJECT_TYPE_SPECIAL:
			return yaffs_DoGenericObjectDeletion(obj);
			break;
		case YAFFS_OBJECT_TYPE_HARDLINK:
		case YAFFS_OBJECT_TYPE_UNKNOWN:
		default:
			return YAFFS_FAIL;
		}
	} else if(yaffs_IsNonEmptyDirectory(obj))
		return YAFFS_FAIL;
	else
		return yaffs_ChangeObjectName(obj, obj->myDev->unlinkedDir,
					   _Y("unlinked"), 0, 0);
}


static int yaffs_UnlinkObject(yaffs_Object *obj)
{

	if (obj && obj->unlinkAllowed)
		return yaffs_UnlinkWorker(obj);

	return YAFFS_FAIL;

}
int yaffs_Unlink(yaffs_Object *dir, const YCHAR *name)
{
	yaffs_Object *obj;

	obj = yaffs_FindObjectByName(dir, name);
	return yaffs_UnlinkObject(obj);
}

/*----------------------- Initialisation Scanning ---------------------- */

void yaffs_HandleShadowedObject(yaffs_Device *dev, int objId,
				int backwardScanning)
{
	yaffs_Object *obj;

	if (!backwardScanning) {
		/* Handle YAFFS1 forward scanning case
		 * For YAFFS1 we always do the deletion
		 */

	} else {
		/* Handle YAFFS2 case (backward scanning)
		 * If the shadowed object exists then ignore.
		 */
		obj = yaffs_FindObjectByNumber(dev, objId);
		if(obj)
			return;
	}

	/* Let's create it (if it does not exist) assuming it is a file so that it can do shrinking etc.
	 * We put it in unlinked dir to be cleaned up after the scanning
	 */
	obj =
	    yaffs_FindOrCreateObjectByNumber(dev, objId,
					     YAFFS_OBJECT_TYPE_FILE);
	if (!obj)
		return;
	obj->isShadowed = 1;
	yaffs_AddObjectToDirectory(dev->unlinkedDir, obj);
	obj->variant.fileVariant.shrinkSize = 0;
	obj->valid = 1;		/* So that we don't read any other info for this file */

}


void yaffs_HardlinkFixup(yaffs_Device *dev, yaffs_Object *hardList)
{
	yaffs_Object *hl;
	yaffs_Object *in;

	while (hardList) {
		hl = hardList;
		hardList = (yaffs_Object *) (hardList->hardLinks.next);

		in = yaffs_FindObjectByNumber(dev,
					      hl->variant.hardLinkVariant.
					      equivalentObjectId);

		if (in) {
			/* Add the hardlink pointers */
			hl->variant.hardLinkVariant.equivalentObject = in;
			ylist_add(&hl->hardLinks, &in->hardLinks);
		} else {
			/* Todo Need to report/handle this better.
			 * Got a problem... hardlink to a non-existant object
			 */
			hl->variant.hardLinkVariant.equivalentObject = NULL;
			YINIT_LIST_HEAD(&hl->hardLinks);

		}
	}
}


static void yaffs_StripDeletedObjects(yaffs_Device *dev)
{
	/*
	*  Sort out state of unlinked and deleted objects after scanning.
	*/
	struct ylist_head *i;
	struct ylist_head *n;
	yaffs_Object *l;

	if (dev->readOnly)
		return;

	/* Soft delete all the unlinked files */
	ylist_for_each_safe(i, n,
		&dev->unlinkedDir->variant.directoryVariant.children) {
		if (i) {
			l = ylist_entry(i, yaffs_Object, siblings);
			yaffs_DeleteObject(l);
		}
	}

	ylist_for_each_safe(i, n,
		&dev->deletedDir->variant.directoryVariant.children) {
		if (i) {
			l = ylist_entry(i, yaffs_Object, siblings);
			yaffs_DeleteObject(l);
		}
	}

}

/*
 * This code iterates through all the objects making sure that they are rooted.
 * Any unrooted objects are re-rooted in lost+found.
 * An object needs to be in one of:
 * - Directly under deleted, unlinked
 * - Directly or indirectly under root.
 *
 * Note:
 *  This code assumes that we don't ever change the current relationships between
 *  directories:
 *   rootDir->parent == unlinkedDir->parent == deletedDir->parent == NULL
 *   lostNfound->parent == rootDir
 *
 * This fixes the problem where directories might have inadvertently been deleted
 * leaving the object "hanging" without being rooted in the directory tree.
 */
 
static int yaffs_HasNULLParent(yaffs_Device *dev, yaffs_Object *obj)
{
	return (obj == dev->deletedDir ||
		obj == dev->unlinkedDir||
		obj == dev->rootDir);
}

static void yaffs_FixHangingObjects(yaffs_Device *dev)
{
	yaffs_Object *obj;
	yaffs_Object *parent;
	int i;
	struct ylist_head *lh;
	struct ylist_head *n;
	int depthLimit;
	int hanging;

	if (dev->readOnly)
		return;

	/* Iterate through the objects in each hash entry,
	 * looking at each object.
	 * Make sure it is rooted.
	 */

	for (i = 0; i <  YAFFS_NOBJECT_BUCKETS; i++) {
		ylist_for_each_safe(lh, n, &dev->objectBucket[i].list) {
			if (lh) {
				obj = ylist_entry(lh, yaffs_Object, hashLink);
				parent= obj->parent;
				
				if(yaffs_HasNULLParent(dev,obj)){
					/* These directories are not hanging */
					hanging = 0;
				}
				else if(!parent || parent->variantType != YAFFS_OBJECT_TYPE_DIRECTORY)
					hanging = 1;
				else if(yaffs_HasNULLParent(dev,parent))
					hanging = 0;
				else {
					/*
					 * Need to follow the parent chain to see if it is hanging.
					 */
					hanging = 0;
					depthLimit=100;

					while(parent != dev->rootDir &&
						parent->parent &&
						parent->parent->variantType == YAFFS_OBJECT_TYPE_DIRECTORY &&
						depthLimit > 0){
						parent = parent->parent;
						depthLimit--;
					}
					if(parent != dev->rootDir)
						hanging = 1;
				}
				if(hanging){
					T(YAFFS_TRACE_SCAN,
					  (TSTR("Hanging object %d moved to lost and found" TENDSTR),
					  	obj->objectId));
					yaffs_AddObjectToDirectory(dev->lostNFoundDir,obj);
				}
			}
		}
	}
}


/*
 * Delete directory contents for cleaning up lost and found.
 */
static void yaffs_DeleteDirectoryContents(yaffs_Object *dir)
{
	yaffs_Object *obj;
	struct ylist_head *lh;
	struct ylist_head *n;

	if(dir->variantType != YAFFS_OBJECT_TYPE_DIRECTORY)
		YBUG();
	
	ylist_for_each_safe(lh, n, &dir->variant.directoryVariant.children) {
		if (lh) {
			obj = ylist_entry(lh, yaffs_Object, siblings);
			if(obj->variantType == YAFFS_OBJECT_TYPE_DIRECTORY)
				yaffs_DeleteDirectoryContents(obj);

			T(YAFFS_TRACE_SCAN,
				(TSTR("Deleting lost_found object %d" TENDSTR),
				obj->objectId));

			/* Need to use UnlinkObject since Delete would not handle
			 * hardlinked objects correctly.
			 */
			yaffs_UnlinkObject(obj); 
=======
	/* Update file object */

	if ((start_write + n_done) > in->variant.file_variant.file_size)
		in->variant.file_variant.file_size = (start_write + n_done);

	in->dirty = 1;
	return n_done;
}

int yaffs_wr_file(struct yaffs_obj *in, const u8 *buffer, loff_t offset,
		  int n_bytes, int write_trhrough)
{
	yaffs2_handle_hole(in, offset);
	return yaffs_do_file_wr(in, buffer, offset, n_bytes, write_trhrough);
}

/* ---------------------- File resizing stuff ------------------ */

static void yaffs_prune_chunks(struct yaffs_obj *in, int new_size)
{

	struct yaffs_dev *dev = in->my_dev;
	int old_size = in->variant.file_variant.file_size;
	int i;
	int chunk_id;
	int last_del = 1 + (old_size - 1) / dev->data_bytes_per_chunk;
	int start_del = 1 + (new_size + dev->data_bytes_per_chunk - 1) /
	    dev->data_bytes_per_chunk;


	/* Delete backwards so that we don't end up with holes if
	 * power is lost part-way through the operation.
	 */
	for (i = last_del; i >= start_del; i--) {
		/* NB this could be optimised somewhat,
		 * eg. could retrieve the tags and write them without
		 * using yaffs_chunk_del
		 */

		chunk_id = yaffs_find_del_file_chunk(in, i, NULL);

		if (chunk_id < 1)
			continue;

		if (chunk_id <
		    (dev->internal_start_block * dev->param.chunks_per_block) ||
		    chunk_id >=
		    ((dev->internal_end_block + 1) *
		      dev->param.chunks_per_block)) {
			yaffs_trace(YAFFS_TRACE_ALWAYS,
				"Found daft chunk_id %d for %d",
				chunk_id, i);
		} else {
			in->n_data_chunks--;
			yaffs_chunk_del(dev, chunk_id, 1, __LINE__);
		}
	}
}

void yaffs_resize_file_down(struct yaffs_obj *obj, loff_t new_size)
{
	int new_full;
	u32 new_partial;
	struct yaffs_dev *dev = obj->my_dev;

	yaffs_addr_to_chunk(dev, new_size, &new_full, &new_partial);

	yaffs_prune_chunks(obj, new_size);

	if (new_partial != 0) {
		int last_chunk = 1 + new_full;
		u8 *local_buffer = yaffs_get_temp_buffer(dev);

		/* Rewrite the last chunk with its new size and zero pad */
		yaffs_rd_data_obj(obj, last_chunk, local_buffer);
		memset(local_buffer + new_partial, 0,
		       dev->data_bytes_per_chunk - new_partial);

		yaffs_wr_data_obj(obj, last_chunk, local_buffer,
				  new_partial, 1);

		yaffs_release_temp_buffer(dev, local_buffer);
	}

	obj->variant.file_variant.file_size = new_size;

	yaffs_prune_tree(dev, &obj->variant.file_variant);
}

int yaffs_resize_file(struct yaffs_obj *in, loff_t new_size)
{
	struct yaffs_dev *dev = in->my_dev;
	int old_size = in->variant.file_variant.file_size;

	yaffs_flush_file_cache(in);
	yaffs_invalidate_whole_cache(in);

	yaffs_check_gc(dev, 0);

	if (in->variant_type != YAFFS_OBJECT_TYPE_FILE)
		return YAFFS_FAIL;

	if (new_size == old_size)
		return YAFFS_OK;

	if (new_size > old_size) {
		yaffs2_handle_hole(in, new_size);
		in->variant.file_variant.file_size = new_size;
	} else {
		/* new_size < old_size */
		yaffs_resize_file_down(in, new_size);
	}

	/* Write a new object header to reflect the resize.
	 * show we've shrunk the file, if need be
	 * Do this only if the file is not in the deleted directories
	 * and is not shadowed.
	 */
	if (in->parent &&
	    !in->is_shadowed &&
	    in->parent->obj_id != YAFFS_OBJECTID_UNLINKED &&
	    in->parent->obj_id != YAFFS_OBJECTID_DELETED)
		yaffs_update_oh(in, NULL, 0, 0, 0, NULL);

	return YAFFS_OK;
}

int yaffs_flush_file(struct yaffs_obj *in, int update_time, int data_sync)
{
	if (!in->dirty)
		return YAFFS_OK;

	yaffs_flush_file_cache(in);

	if (data_sync)
		return YAFFS_OK;

	if (update_time)
		yaffs_load_current_time(in, 0, 0);

	return (yaffs_update_oh(in, NULL, 0, 0, 0, NULL) >= 0) ?
				YAFFS_OK : YAFFS_FAIL;
}


/* yaffs_del_file deletes the whole file data
 * and the inode associated with the file.
 * It does not delete the links associated with the file.
 */
static int yaffs_unlink_file_if_needed(struct yaffs_obj *in)
{
	int ret_val;
	int del_now = 0;
	struct yaffs_dev *dev = in->my_dev;

	if (!in->my_inode)
		del_now = 1;

	if (del_now) {
		ret_val =
		    yaffs_change_obj_name(in, in->my_dev->del_dir,
					  _Y("deleted"), 0, 0);
		yaffs_trace(YAFFS_TRACE_TRACING,
			"yaffs: immediate deletion of file %d",
			in->obj_id);
		in->deleted = 1;
		in->my_dev->n_deleted_files++;
		if (dev->param.disable_soft_del || dev->param.is_yaffs2)
			yaffs_resize_file(in, 0);
		yaffs_soft_del_file(in);
	} else {
		ret_val =
		    yaffs_change_obj_name(in, in->my_dev->unlinked_dir,
					  _Y("unlinked"), 0, 0);
	}
	return ret_val;
}

int yaffs_del_file(struct yaffs_obj *in)
{
	int ret_val = YAFFS_OK;
	int deleted;	/* Need to cache value on stack if in is freed */
	struct yaffs_dev *dev = in->my_dev;

	if (dev->param.disable_soft_del || dev->param.is_yaffs2)
		yaffs_resize_file(in, 0);

	if (in->n_data_chunks > 0) {
		/* Use soft deletion if there is data in the file.
		 * That won't be the case if it has been resized to zero.
		 */
		if (!in->unlinked)
			ret_val = yaffs_unlink_file_if_needed(in);

		deleted = in->deleted;

		if (ret_val == YAFFS_OK && in->unlinked && !in->deleted) {
			in->deleted = 1;
			deleted = 1;
			in->my_dev->n_deleted_files++;
			yaffs_soft_del_file(in);
		}
		return deleted ? YAFFS_OK : YAFFS_FAIL;
	} else {
		/* The file has no data chunks so we toss it immediately */
		yaffs_free_tnode(in->my_dev, in->variant.file_variant.top);
		in->variant.file_variant.top = NULL;
		yaffs_generic_obj_del(in);

		return YAFFS_OK;
	}
}

int yaffs_is_non_empty_dir(struct yaffs_obj *obj)
{
	return (obj &&
		obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY) &&
		!(list_empty(&obj->variant.dir_variant.children));
}

static int yaffs_del_dir(struct yaffs_obj *obj)
{
	/* First check that the directory is empty. */
	if (yaffs_is_non_empty_dir(obj))
		return YAFFS_FAIL;

	return yaffs_generic_obj_del(obj);
}

static int yaffs_del_symlink(struct yaffs_obj *in)
{
	kfree(in->variant.symlink_variant.alias);
	in->variant.symlink_variant.alias = NULL;

	return yaffs_generic_obj_del(in);
}

static int yaffs_del_link(struct yaffs_obj *in)
{
	/* remove this hardlink from the list associated with the equivalent
	 * object
	 */
	list_del_init(&in->hard_links);
	return yaffs_generic_obj_del(in);
}

int yaffs_del_obj(struct yaffs_obj *obj)
{
	int ret_val = -1;

	switch (obj->variant_type) {
	case YAFFS_OBJECT_TYPE_FILE:
		ret_val = yaffs_del_file(obj);
		break;
	case YAFFS_OBJECT_TYPE_DIRECTORY:
		if (!list_empty(&obj->variant.dir_variant.dirty)) {
			yaffs_trace(YAFFS_TRACE_BACKGROUND,
				"Remove object %d from dirty directories",
				obj->obj_id);
			list_del_init(&obj->variant.dir_variant.dirty);
		}
		return yaffs_del_dir(obj);
		break;
	case YAFFS_OBJECT_TYPE_SYMLINK:
		ret_val = yaffs_del_symlink(obj);
		break;
	case YAFFS_OBJECT_TYPE_HARDLINK:
		ret_val = yaffs_del_link(obj);
		break;
	case YAFFS_OBJECT_TYPE_SPECIAL:
		ret_val = yaffs_generic_obj_del(obj);
		break;
	case YAFFS_OBJECT_TYPE_UNKNOWN:
		ret_val = 0;
		break;		/* should not happen. */
	}
	return ret_val;
}

static int yaffs_unlink_worker(struct yaffs_obj *obj)
{
	int del_now = 0;

	if (!obj)
		return YAFFS_FAIL;

	if (!obj->my_inode)
		del_now = 1;

	yaffs_update_parent(obj->parent);

	if (obj->variant_type == YAFFS_OBJECT_TYPE_HARDLINK) {
		return yaffs_del_link(obj);
	} else if (!list_empty(&obj->hard_links)) {
		/* Curve ball: We're unlinking an object that has a hardlink.
		 *
		 * This problem arises because we are not strictly following
		 * The Linux link/inode model.
		 *
		 * We can't really delete the object.
		 * Instead, we do the following:
		 * - Select a hardlink.
		 * - Unhook it from the hard links
		 * - Move it from its parent directory so that the rename works.
		 * - Rename the object to the hardlink's name.
		 * - Delete the hardlink
		 */

		struct yaffs_obj *hl;
		struct yaffs_obj *parent;
		int ret_val;
		YCHAR name[YAFFS_MAX_NAME_LENGTH + 1];

		hl = list_entry(obj->hard_links.next, struct yaffs_obj,
				hard_links);

		yaffs_get_obj_name(hl, name, YAFFS_MAX_NAME_LENGTH + 1);
		parent = hl->parent;

		list_del_init(&hl->hard_links);

		yaffs_add_obj_to_dir(obj->my_dev->unlinked_dir, hl);

		ret_val = yaffs_change_obj_name(obj, parent, name, 0, 0);

		if (ret_val == YAFFS_OK)
			ret_val = yaffs_generic_obj_del(hl);

		return ret_val;

	} else if (del_now) {
		switch (obj->variant_type) {
		case YAFFS_OBJECT_TYPE_FILE:
			return yaffs_del_file(obj);
			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			list_del_init(&obj->variant.dir_variant.dirty);
			return yaffs_del_dir(obj);
			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
			return yaffs_del_symlink(obj);
			break;
		case YAFFS_OBJECT_TYPE_SPECIAL:
			return yaffs_generic_obj_del(obj);
			break;
		case YAFFS_OBJECT_TYPE_HARDLINK:
		case YAFFS_OBJECT_TYPE_UNKNOWN:
		default:
			return YAFFS_FAIL;
		}
	} else if (yaffs_is_non_empty_dir(obj)) {
		return YAFFS_FAIL;
	} else {
		return yaffs_change_obj_name(obj, obj->my_dev->unlinked_dir,
						_Y("unlinked"), 0, 0);
	}
}

static int yaffs_unlink_obj(struct yaffs_obj *obj)
{
	if (obj && obj->unlink_allowed)
		return yaffs_unlink_worker(obj);

	return YAFFS_FAIL;
}

int yaffs_unlinker(struct yaffs_obj *dir, const YCHAR *name)
{
	struct yaffs_obj *obj;

	obj = yaffs_find_by_name(dir, name);
	return yaffs_unlink_obj(obj);
}

/* Note:
 * If old_name is NULL then we take old_dir as the object to be renamed.
 */
int yaffs_rename_obj(struct yaffs_obj *old_dir, const YCHAR *old_name,
		     struct yaffs_obj *new_dir, const YCHAR *new_name)
{
	struct yaffs_obj *obj = NULL;
	struct yaffs_obj *existing_target = NULL;
	int force = 0;
	int result;
	struct yaffs_dev *dev;

	if (!old_dir || old_dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		BUG();
		return YAFFS_FAIL;
	}
	if (!new_dir || new_dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		BUG();
		return YAFFS_FAIL;
	}

	dev = old_dir->my_dev;

#ifdef CONFIG_YAFFS_CASE_INSENSITIVE
	/* Special case for case insemsitive systems.
	 * While look-up is case insensitive, the name isn't.
	 * Therefore we might want to change x.txt to X.txt
	 */
	if (old_dir == new_dir &&
		old_name && new_name &&
		strcmp(old_name, new_name) == 0)
		force = 1;
#endif

	if (strnlen(new_name, YAFFS_MAX_NAME_LENGTH + 1) >
	    YAFFS_MAX_NAME_LENGTH)
		/* ENAMETOOLONG */
		return YAFFS_FAIL;

	if (old_name)
		obj = yaffs_find_by_name(old_dir, old_name);
	else{
		obj = old_dir;
		old_dir = obj->parent;
	}

	if (obj && obj->rename_allowed) {
		/* Now handle an existing target, if there is one */
		existing_target = yaffs_find_by_name(new_dir, new_name);
		if (yaffs_is_non_empty_dir(existing_target)) {
			return YAFFS_FAIL;	/* ENOTEMPTY */
		} else if (existing_target && existing_target != obj) {
			/* Nuke the target first, using shadowing,
			 * but only if it isn't the same object.
			 *
			 * Note we must disable gc here otherwise it can mess
			 * up the shadowing.
			 *
			 */
			dev->gc_disable = 1;
			yaffs_change_obj_name(obj, new_dir, new_name, force,
					      existing_target->obj_id);
			existing_target->is_shadowed = 1;
			yaffs_unlink_obj(existing_target);
			dev->gc_disable = 0;
		}

		result = yaffs_change_obj_name(obj, new_dir, new_name, 1, 0);

		yaffs_update_parent(old_dir);
		if (new_dir != old_dir)
			yaffs_update_parent(new_dir);

		return result;
	}
	return YAFFS_FAIL;
}

/*----------------------- Initialisation Scanning ---------------------- */

void yaffs_handle_shadowed_obj(struct yaffs_dev *dev, int obj_id,
			       int backward_scanning)
{
	struct yaffs_obj *obj;

	if (backward_scanning) {
		/* Handle YAFFS2 case (backward scanning)
		 * If the shadowed object exists then ignore.
		 */
		obj = yaffs_find_by_number(dev, obj_id);
		if (obj)
			return;
	}

	/* Let's create it (if it does not exist) assuming it is a file so that
	 * it can do shrinking etc.
	 * We put it in unlinked dir to be cleaned up after the scanning
	 */
	obj =
	    yaffs_find_or_create_by_number(dev, obj_id, YAFFS_OBJECT_TYPE_FILE);
	if (!obj)
		return;
	obj->is_shadowed = 1;
	yaffs_add_obj_to_dir(dev->unlinked_dir, obj);
	obj->variant.file_variant.shrink_size = 0;
	obj->valid = 1;		/* So that we don't read any other info. */
}

void yaffs_link_fixup(struct yaffs_dev *dev, struct list_head *hard_list)
{
	struct list_head *lh;
	struct list_head *save;
	struct yaffs_obj *hl;
	struct yaffs_obj *in;

	list_for_each_safe(lh, save, hard_list) {
		hl = list_entry(lh, struct yaffs_obj, hard_links);
		in = yaffs_find_by_number(dev,
					hl->variant.hardlink_variant.equiv_id);

		if (in) {
			/* Add the hardlink pointers */
			hl->variant.hardlink_variant.equiv_obj = in;
			list_add(&hl->hard_links, &in->hard_links);
		} else {
			/* Todo Need to report/handle this better.
			 * Got a problem... hardlink to a non-existant object
			 */
			hl->variant.hardlink_variant.equiv_obj = NULL;
			INIT_LIST_HEAD(&hl->hard_links);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		}
	}
			
}

<<<<<<< HEAD
static void yaffs_EmptyLostAndFound(yaffs_Device *dev)
{
	yaffs_DeleteDirectoryContents(dev->lostNFoundDir);
}

static void yaffs_CheckObjectDetailsLoaded(yaffs_Object *in)
{
	__u8 *chunkData;
	yaffs_ObjectHeader *oh;
	yaffs_Device *dev;
	yaffs_ExtendedTags tags;
	int result;
	int alloc_failed = 0;

	if (!in)
		return;

	dev = in->myDev;

#if 0
	T(YAFFS_TRACE_SCAN, (TSTR("details for object %d %s loaded" TENDSTR),
		in->objectId,
		in->lazyLoaded ? "not yet" : "already"));
#endif

	if (in->lazyLoaded && in->hdrChunk > 0) {
		in->lazyLoaded = 0;
		chunkData = yaffs_GetTempBuffer(dev, __LINE__);

		result = yaffs_ReadChunkWithTagsFromNAND(dev, in->hdrChunk, chunkData, &tags);
		oh = (yaffs_ObjectHeader *) chunkData;

		in->yst_mode = oh->yst_mode;
#ifdef CONFIG_YAFFS_WINCE
		in->win_atime[0] = oh->win_atime[0];
		in->win_ctime[0] = oh->win_ctime[0];
		in->win_mtime[0] = oh->win_mtime[0];
		in->win_atime[1] = oh->win_atime[1];
		in->win_ctime[1] = oh->win_ctime[1];
		in->win_mtime[1] = oh->win_mtime[1];
#else
		in->yst_uid = oh->yst_uid;
		in->yst_gid = oh->yst_gid;
		in->yst_atime = oh->yst_atime;
		in->yst_mtime = oh->yst_mtime;
		in->yst_ctime = oh->yst_ctime;
		in->yst_rdev = oh->yst_rdev;

#endif
		yaffs_SetObjectNameFromOH(in, oh);

		if (in->variantType == YAFFS_OBJECT_TYPE_SYMLINK) {
			in->variant.symLinkVariant.alias =
						    yaffs_CloneString(oh->alias);
			if (!in->variant.symLinkVariant.alias)
				alloc_failed = 1; /* Not returned to caller */
		}

		yaffs_ReleaseTempBuffer(dev, chunkData, __LINE__);
=======
static void yaffs_strip_deleted_objs(struct yaffs_dev *dev)
{
	/*
	 *  Sort out state of unlinked and deleted objects after scanning.
	 */
	struct list_head *i;
	struct list_head *n;
	struct yaffs_obj *l;

	if (dev->read_only)
		return;

	/* Soft delete all the unlinked files */
	list_for_each_safe(i, n,
			   &dev->unlinked_dir->variant.dir_variant.children) {
		l = list_entry(i, struct yaffs_obj, siblings);
		yaffs_del_obj(l);
	}

	list_for_each_safe(i, n, &dev->del_dir->variant.dir_variant.children) {
		l = list_entry(i, struct yaffs_obj, siblings);
		yaffs_del_obj(l);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	}
}

/*------------------------------  Directory Functions ----------------------------- */

/*
 * This code iterates through all the objects making sure that they are rooted.
 * Any unrooted objects are re-rooted in lost+found.
 * An object needs to be in one of:
 * - Directly under deleted, unlinked
 * - Directly or indirectly under root.
 *
<<<<<<< HEAD
 * ie.
 *   create dir/a : update dir's mtime/ctime
 *   rm dir/a:   update dir's mtime/ctime
 *   modify dir/a: don't update dir's mtimme/ctime
 *
 * This can be handled immediately or defered. Defering helps reduce the number
 * of updates when many files in a directory are changed within a brief period.
 *
 * If the directory updating is defered then yaffs_UpdateDirtyDirecories must be
 * called periodically.
 */
 
static void yaffs_UpdateParent(yaffs_Object *obj)
{
	yaffs_Device *dev;
	if(!obj)
		return;
#ifndef CONFIG_YAFFS_WINCE

	dev = obj->myDev;
	obj->dirty = 1;
	obj->yst_mtime = obj->yst_ctime = Y_CURRENT_TIME;
	if(dev->param.deferDirectoryUpdate){
		struct ylist_head *link = &obj->variant.directoryVariant.dirty; 
	
		if(ylist_empty(link)){
			ylist_add(link,&dev->dirtyDirectories);
			T(YAFFS_TRACE_BACKGROUND, (TSTR("Added object %d to dirty directories" TENDSTR),obj->objectId));
		}

	} else
		yaffs_UpdateObjectHeader(obj, NULL, 0, 0, 0, NULL);
#endif
}

void yaffs_UpdateDirtyDirectories(yaffs_Device *dev)
{
	struct ylist_head *link;
	yaffs_Object *obj;
	yaffs_DirectoryStructure *dS;
	yaffs_ObjectVariant *oV;

	T(YAFFS_TRACE_BACKGROUND, (TSTR("Update dirty directories" TENDSTR)));

	while(!ylist_empty(&dev->dirtyDirectories)){
		link = dev->dirtyDirectories.next;
		ylist_del_init(link);
		
		dS=ylist_entry(link,yaffs_DirectoryStructure,dirty);
		oV = ylist_entry(dS,yaffs_ObjectVariant,directoryVariant);
		obj = ylist_entry(oV,yaffs_Object,variant);

		T(YAFFS_TRACE_BACKGROUND, (TSTR("Update directory %d" TENDSTR), obj->objectId));

		if(obj->dirty)
			yaffs_UpdateObjectHeader(obj, NULL, 0, 0, 0, NULL);
	}
}

static void yaffs_RemoveObjectFromDirectory(yaffs_Object *obj)
=======
 * Note:
 *  This code assumes that we don't ever change the current relationships
 *  between directories:
 *   root_dir->parent == unlinked_dir->parent == del_dir->parent == NULL
 *   lost-n-found->parent == root_dir
 *
 * This fixes the problem where directories might have inadvertently been
 * deleted leaving the object "hanging" without being rooted in the
 * directory tree.
 */

static int yaffs_has_null_parent(struct yaffs_dev *dev, struct yaffs_obj *obj)
{
	return (obj == dev->del_dir ||
		obj == dev->unlinked_dir || obj == dev->root_dir);
}

static void yaffs_fix_hanging_objs(struct yaffs_dev *dev)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
{
	struct yaffs_obj *obj;
	struct yaffs_obj *parent;
	int i;
	struct list_head *lh;
	struct list_head *n;
	int depth_limit;
	int hanging;

<<<<<<< HEAD
	if (dev && dev->param.removeObjectCallback)
		dev->param.removeObjectCallback(obj);
=======
	if (dev->read_only)
		return;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	/* Iterate through the objects in each hash entry,
	 * looking at each object.
	 * Make sure it is rooted.
	 */

	for (i = 0; i < YAFFS_NOBJECT_BUCKETS; i++) {
		list_for_each_safe(lh, n, &dev->obj_bucket[i].list) {
			obj = list_entry(lh, struct yaffs_obj, hash_link);
			parent = obj->parent;

			if (yaffs_has_null_parent(dev, obj)) {
				/* These directories are not hanging */
				hanging = 0;
			} else if (!parent ||
				   parent->variant_type !=
				   YAFFS_OBJECT_TYPE_DIRECTORY) {
				hanging = 1;
			} else if (yaffs_has_null_parent(dev, parent)) {
				hanging = 0;
			} else {
				/*
				 * Need to follow the parent chain to
				 * see if it is hanging.
				 */
				hanging = 0;
				depth_limit = 100;

				while (parent != dev->root_dir &&
				       parent->parent &&
				       parent->parent->variant_type ==
				       YAFFS_OBJECT_TYPE_DIRECTORY &&
				       depth_limit > 0) {
					parent = parent->parent;
					depth_limit--;
				}
				if (parent != dev->root_dir)
					hanging = 1;
			}
			if (hanging) {
				yaffs_trace(YAFFS_TRACE_SCAN,
					"Hanging object %d moved to lost and found",
					obj->obj_id);
				yaffs_add_obj_to_dir(dev->lost_n_found, obj);
			}
		}
	}
}

<<<<<<< HEAD
void yaffs_AddObjectToDirectory(yaffs_Object *directory,
					yaffs_Object *obj)
=======
/*
 * Delete directory contents for cleaning up lost and found.
 */
static void yaffs_del_dir_contents(struct yaffs_obj *dir)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
{
	struct yaffs_obj *obj;
	struct list_head *lh;
	struct list_head *n;

	if (dir->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY)
		BUG();

	list_for_each_safe(lh, n, &dir->variant.dir_variant.children) {
		obj = list_entry(lh, struct yaffs_obj, siblings);
		if (obj->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY)
			yaffs_del_dir_contents(obj);
		yaffs_trace(YAFFS_TRACE_SCAN,
			"Deleting lost_found object %d",
			obj->obj_id);
		yaffs_unlink_obj(obj);
	}
}

static void yaffs_empty_l_n_f(struct yaffs_dev *dev)
{
	yaffs_del_dir_contents(dev->lost_n_found);
}


struct yaffs_obj *yaffs_find_by_name(struct yaffs_obj *directory,
				     const YCHAR *name)
{
	int sum;
	struct list_head *i;
	YCHAR buffer[YAFFS_MAX_NAME_LENGTH + 1];
	struct yaffs_obj *l;

	if (!name)
		return NULL;

	if (!directory) {
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"tragedy: yaffs_find_by_name: null pointer directory"
			);
		BUG();
		return NULL;
	}
	if (directory->variant_type != YAFFS_OBJECT_TYPE_DIRECTORY) {
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"tragedy: yaffs_find_by_name: non-directory"
			);
		BUG();
	}

	sum = yaffs_calc_name_sum(name);

	list_for_each(i, &directory->variant.dir_variant.children) {
		l = list_entry(i, struct yaffs_obj, siblings);

		if (l->parent != directory)
			BUG();

		yaffs_check_obj_details_loaded(l);

		/* Special case for lost-n-found */
		if (l->obj_id == YAFFS_OBJECTID_LOSTNFOUND) {
			if (!strcmp(name, YAFFS_LOSTNFOUND_NAME))
				return l;
		} else if (l->sum == sum || l->hdr_chunk <= 0) {
			/* LostnFound chunk called Objxxx
			 * Do a real check
			 */
			yaffs_get_obj_name(l, buffer,
				YAFFS_MAX_NAME_LENGTH + 1);
			if (strncmp(name, buffer, YAFFS_MAX_NAME_LENGTH) == 0)
				return l;
		}
	}
	return NULL;
}

/* GetEquivalentObject dereferences any hard links to get to the
 * actual object.
 */

struct yaffs_obj *yaffs_get_equivalent_obj(struct yaffs_obj *obj)
{
	if (obj && obj->variant_type == YAFFS_OBJECT_TYPE_HARDLINK) {
		obj = obj->variant.hardlink_variant.equiv_obj;
		yaffs_check_obj_details_loaded(obj);
	}
	return obj;
}

/*
 *  A note or two on object names.
 *  * If the object name is missing, we then make one up in the form objnnn
 *
 *  * ASCII names are stored in the object header's name field from byte zero
 *  * Unicode names are historically stored starting from byte zero.
 *
 * Then there are automatic Unicode names...
 * The purpose of these is to save names in a way that can be read as
 * ASCII or Unicode names as appropriate, thus allowing a Unicode and ASCII
 * system to share files.
 *
 * These automatic unicode are stored slightly differently...
<<<<<<< HEAD
 *  - If the name can fit in the ASCII character space then they are saved as 
=======
 *  - If the name can fit in the ASCII character space then they are saved as
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
 *    ascii names as per above.
 *  - If the name needs Unicode then the name is saved in Unicode
 *    starting at oh->name[1].

 */
<<<<<<< HEAD
static void yaffs_FixNullName(yaffs_Object * obj,YCHAR * name, int buffSize)
{
	/* Create an object name if we could not find one. */
	if(yaffs_strnlen(name,YAFFS_MAX_NAME_LENGTH) == 0){
		YCHAR locName[20];
		YCHAR numString[20];
		YCHAR *x = &numString[19];
		unsigned v = obj->objectId;
		numString[19] = 0;
		while(v>0){
=======
static void yaffs_fix_null_name(struct yaffs_obj *obj, YCHAR *name,
				int buffer_size)
{
	/* Create an object name if we could not find one. */
	if (strnlen(name, YAFFS_MAX_NAME_LENGTH) == 0) {
		YCHAR local_name[20];
		YCHAR num_string[20];
		YCHAR *x = &num_string[19];
		unsigned v = obj->obj_id;
		num_string[19] = 0;
		while (v > 0) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
			x--;
			*x = '0' + (v % 10);
			v /= 10;
		}
		/* make up a name */
<<<<<<< HEAD
		yaffs_strcpy(locName, YAFFS_LOSTNFOUND_PREFIX);
		yaffs_strcat(locName,x);
		yaffs_strncpy(name, locName, buffSize - 1);
	}
}

static void yaffs_LoadNameFromObjectHeader(yaffs_Device *dev,YCHAR *name, const YCHAR *ohName, int bufferSize)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE
	if(dev->param.autoUnicode){
		if(*ohName){
			/* It is an ASCII name, so do an ASCII to unicode conversion */
			const char *asciiOhName = (const char *)ohName;
			int n = bufferSize - 1;
			while(n > 0 && *asciiOhName){
				*name = *asciiOhName;
				name++;
				asciiOhName++;
				n--;
			}
		} else 
			yaffs_strncpy(name,ohName+1, bufferSize -1);
	} else
#endif
		yaffs_strncpy(name, ohName, bufferSize - 1);
}


static void yaffs_LoadObjectHeaderFromName(yaffs_Device *dev, YCHAR *ohName, const YCHAR *name)
{
#ifdef CONFIG_YAFFS_AUTO_UNICODE

	int isAscii;
	YCHAR *w;

	if(dev->param.autoUnicode){

		isAscii = 1;
		w = name;
	
		/* Figure out if the name will fit in ascii character set */
		while(isAscii && *w){
			if((*w) & 0xff00)
				isAscii = 0;
			w++;
		}

		if(isAscii){
			/* It is an ASCII name, so do a unicode to ascii conversion */
			char *asciiOhName = (char *)ohName;
			int n = YAFFS_MAX_NAME_LENGTH  - 1;
			while(n > 0 && *name){
				*asciiOhName= *name;
				name++;
				asciiOhName++;
				n--;
			}
		} else{
			/* It is a unicode name, so save starting at the second YCHAR */
			*ohName = 0;
			yaffs_strncpy(ohName+1,name, YAFFS_MAX_NAME_LENGTH -2);
		}
	}
	else 
#endif
		yaffs_strncpy(ohName,name, YAFFS_MAX_NAME_LENGTH - 1);

}

int yaffs_GetObjectName(yaffs_Object * obj, YCHAR * name, int buffSize)
{
	memset(name, 0, buffSize * sizeof(YCHAR));
	
	yaffs_CheckObjectDetailsLoaded(obj);

	if (obj->objectId == YAFFS_OBJECTID_LOSTNFOUND) {
		yaffs_strncpy(name, YAFFS_LOSTNFOUND_NAME, buffSize - 1);
	} 
#ifdef CONFIG_YAFFS_SHORT_NAMES_IN_RAM
	else if (obj->shortName[0]) {
		yaffs_strcpy(name, obj->shortName);
	}
#endif
	else if(obj->hdrChunk > 0) {
=======
		strcpy(local_name, YAFFS_LOSTNFOUND_PREFIX);
		strcat(local_name, x);
		strncpy(name, local_name, buffer_size - 1);
	}
}

int yaffs_get_obj_name(struct yaffs_obj *obj, YCHAR *name, int buffer_size)
{
	memset(name, 0, buffer_size * sizeof(YCHAR));
	yaffs_check_obj_details_loaded(obj);
	if (obj->obj_id == YAFFS_OBJECTID_LOSTNFOUND)
		strncpy(name, YAFFS_LOSTNFOUND_NAME, buffer_size - 1);
#ifndef CONFIG_YAFFS_NO_SHORT_NAMES
	else if (obj->short_name[0])
		strcpy(name, obj->short_name);
#endif
	else if (obj->hdr_chunk > 0) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		int result;
		u8 *buffer = yaffs_get_temp_buffer(obj->my_dev);

		struct yaffs_obj_hdr *oh = (struct yaffs_obj_hdr *)buffer;

		memset(buffer, 0, obj->my_dev->data_bytes_per_chunk);

		if (obj->hdr_chunk > 0) {
			result = yaffs_rd_chunk_tags_nand(obj->my_dev,
							  obj->hdr_chunk,
							  buffer, NULL);
		}
<<<<<<< HEAD
		yaffs_LoadNameFromObjectHeader(obj->myDev,name,oh->name,buffSize);
=======
		yaffs_load_name_from_oh(obj->my_dev, name, oh->name,
					buffer_size);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

		yaffs_release_temp_buffer(obj->my_dev, buffer);
	}

<<<<<<< HEAD
	yaffs_FixNullName(obj,name,buffSize);

	return yaffs_strnlen(name,YAFFS_MAX_NAME_LENGTH);
}


int yaffs_GetObjectFileLength(yaffs_Object *obj)
=======
	yaffs_fix_null_name(obj, name, buffer_size);

	return strnlen(name, YAFFS_MAX_NAME_LENGTH);
}

int yaffs_get_obj_length(struct yaffs_obj *obj)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
{
	/* Dereference any hard linking */
	obj = yaffs_get_equivalent_obj(obj);

<<<<<<< HEAD
	if (obj->variantType == YAFFS_OBJECT_TYPE_FILE)
		return obj->variant.fileVariant.fileSize;
	if (obj->variantType == YAFFS_OBJECT_TYPE_SYMLINK){
		if(!obj->variant.symLinkVariant.alias)
			return 0;
		return yaffs_strnlen(obj->variant.symLinkVariant.alias,YAFFS_MAX_ALIAS_LENGTH);
=======
	if (obj->variant_type == YAFFS_OBJECT_TYPE_FILE)
		return obj->variant.file_variant.file_size;
	if (obj->variant_type == YAFFS_OBJECT_TYPE_SYMLINK) {
		if (!obj->variant.symlink_variant.alias)
			return 0;
		return strnlen(obj->variant.symlink_variant.alias,
				     YAFFS_MAX_ALIAS_LENGTH);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	} else {
		/* Only a directory should drop through to here */
		return obj->my_dev->data_bytes_per_chunk;
	}
}

int yaffs_get_obj_link_count(struct yaffs_obj *obj)
{
	int count = 0;
	struct list_head *i;

	if (!obj->unlinked)
		count++;	/* the object itself */

	list_for_each(i, &obj->hard_links)
	    count++;		/* add the hard links; */

	return count;
}

int yaffs_get_obj_inode(struct yaffs_obj *obj)
{
	obj = yaffs_get_equivalent_obj(obj);

	return obj->obj_id;
}

unsigned yaffs_get_obj_type(struct yaffs_obj *obj)
{
	obj = yaffs_get_equivalent_obj(obj);

	switch (obj->variant_type) {
	case YAFFS_OBJECT_TYPE_FILE:
		return DT_REG;
		break;
	case YAFFS_OBJECT_TYPE_DIRECTORY:
		return DT_DIR;
		break;
	case YAFFS_OBJECT_TYPE_SYMLINK:
		return DT_LNK;
		break;
	case YAFFS_OBJECT_TYPE_HARDLINK:
		return DT_REG;
		break;
	case YAFFS_OBJECT_TYPE_SPECIAL:
		if (S_ISFIFO(obj->yst_mode))
			return DT_FIFO;
		if (S_ISCHR(obj->yst_mode))
			return DT_CHR;
		if (S_ISBLK(obj->yst_mode))
			return DT_BLK;
		if (S_ISSOCK(obj->yst_mode))
			return DT_SOCK;
		return DT_REG;
		break;
	default:
		return DT_REG;
		break;
	}
}

YCHAR *yaffs_get_symlink_alias(struct yaffs_obj *obj)
{
	obj = yaffs_get_equivalent_obj(obj);
	if (obj->variant_type == YAFFS_OBJECT_TYPE_SYMLINK)
		return yaffs_clone_str(obj->variant.symlink_variant.alias);
	else
<<<<<<< HEAD
		return yaffs_CloneString(_Y(""));
}

#ifndef CONFIG_YAFFS_WINCE

int yaffs_SetAttributes(yaffs_Object *obj, struct iattr *attr)
{
	unsigned int valid = attr->ia_valid;

	if (valid & ATTR_MODE)
		obj->yst_mode = attr->ia_mode;
	if (valid & ATTR_UID)
		obj->yst_uid = attr->ia_uid;
	if (valid & ATTR_GID)
		obj->yst_gid = attr->ia_gid;

	if (valid & ATTR_ATIME)
		obj->yst_atime = Y_TIME_CONVERT(attr->ia_atime);
	if (valid & ATTR_CTIME)
		obj->yst_ctime = Y_TIME_CONVERT(attr->ia_ctime);
	if (valid & ATTR_MTIME)
		obj->yst_mtime = Y_TIME_CONVERT(attr->ia_mtime);

	if (valid & ATTR_SIZE)
		yaffs_ResizeFile(obj, attr->ia_size);

	yaffs_UpdateObjectHeader(obj, NULL, 1, 0, 0, NULL);

	return YAFFS_OK;

}
int yaffs_GetAttributes(yaffs_Object *obj, struct iattr *attr)
{
	unsigned int valid = 0;

	attr->ia_mode = obj->yst_mode;
	valid |= ATTR_MODE;
	attr->ia_uid = obj->yst_uid;
	valid |= ATTR_UID;
	attr->ia_gid = obj->yst_gid;
	valid |= ATTR_GID;

	Y_TIME_CONVERT(attr->ia_atime) = obj->yst_atime;
	valid |= ATTR_ATIME;
	Y_TIME_CONVERT(attr->ia_ctime) = obj->yst_ctime;
	valid |= ATTR_CTIME;
	Y_TIME_CONVERT(attr->ia_mtime) = obj->yst_mtime;
	valid |= ATTR_MTIME;

	attr->ia_size = yaffs_GetFileSize(obj);
	valid |= ATTR_SIZE;

	attr->ia_valid = valid;

	return YAFFS_OK;
}

#endif


static int yaffs_DoXMod(yaffs_Object *obj, int set, const YCHAR *name, const void *value, int size, int flags)
{
	yaffs_XAttrMod xmod;

	int result;

	xmod.set = set;
	xmod.name = name;
	xmod.data = value;
	xmod.size =  size;
	xmod.flags = flags;
	xmod.result = -ENOSPC;

	result = yaffs_UpdateObjectHeader(obj, NULL, 0, 0, 0, &xmod);

	if(result > 0)
		return xmod.result;
	else
		return -ENOSPC;
}

static int yaffs_ApplyXMod(yaffs_Object *obj, char *buffer, yaffs_XAttrMod *xmod)
{
	int retval = 0;
	int x_offs = sizeof(yaffs_ObjectHeader);
	yaffs_Device *dev = obj->myDev;
	int x_size = dev->nDataBytesPerChunk - sizeof(yaffs_ObjectHeader);

	char * x_buffer = buffer + x_offs;

	if(xmod->set)
		retval = nval_set(x_buffer, x_size, xmod->name, xmod->data, xmod->size, xmod->flags);
	else
		retval = nval_del(x_buffer, x_size, xmod->name);

	obj->hasXattr = nval_hasvalues(x_buffer, x_size);
	obj->xattrKnown = 1;

	xmod->result = retval;

	return retval;
}

static int yaffs_DoXFetch(yaffs_Object *obj, const YCHAR *name, void *value, int size)
{
	char *buffer = NULL;
	int result;
	yaffs_ExtendedTags tags;
	yaffs_Device *dev = obj->myDev;
	int x_offs = sizeof(yaffs_ObjectHeader);
	int x_size = dev->nDataBytesPerChunk - sizeof(yaffs_ObjectHeader);

	char * x_buffer;

	int retval = 0;

	if(obj->hdrChunk < 1)
		return -ENODATA;

	/* If we know that the object has no xattribs then don't do all the
	 * reading and parsing.
	 */
	if(obj->xattrKnown && !obj->hasXattr){
		if(name)
			return -ENODATA;
		else
			return 0;
	}

	buffer = (char *) yaffs_GetTempBuffer(dev, __LINE__);
	if(!buffer)
		return -ENOMEM;

	result = yaffs_ReadChunkWithTagsFromNAND(dev,obj->hdrChunk, (__u8 *)buffer, &tags);

	if(result != YAFFS_OK)
		retval = -ENOENT;
	else{
		x_buffer =  buffer + x_offs;

		if (!obj->xattrKnown){
			obj->hasXattr = nval_hasvalues(x_buffer, x_size);
			obj->xattrKnown = 1;
		}

		if(name)
			retval = nval_get(x_buffer, x_size, name, value, size);
		else
			retval = nval_list(x_buffer, x_size, value,size);
	}
	yaffs_ReleaseTempBuffer(dev,(__u8 *)buffer,__LINE__);
	return retval;
}

int yaffs_SetXAttribute(yaffs_Object *obj, const YCHAR *name, const void * value, int size, int flags)
{
	return yaffs_DoXMod(obj, 1, name, value, size, flags);
}

int yaffs_RemoveXAttribute(yaffs_Object *obj, const YCHAR *name)
{
	return yaffs_DoXMod(obj, 0, name, NULL, 0, 0);
}

int yaffs_GetXAttribute(yaffs_Object *obj, const YCHAR *name, void *value, int size)
{
	return yaffs_DoXFetch(obj, name, value, size);
}

int yaffs_ListXAttributes(yaffs_Object *obj, char *buffer, int size)
{
	return yaffs_DoXFetch(obj, NULL, buffer,size);
}



#if 0
int yaffs_DumpObject(yaffs_Object *obj)
{
	YCHAR name[257];

	yaffs_GetObjectName(obj, name, YAFFS_MAX_NAME_LENGTH + 1);

	T(YAFFS_TRACE_ALWAYS,
	  (TSTR
	   ("Object %d, inode %d \"%s\"\n dirty %d valid %d serial %d sum %d"
	    " chunk %d type %d size %d\n"
	    TENDSTR), obj->objectId, yaffs_GetObjectInode(obj), name,
	   obj->dirty, obj->valid, obj->serial, obj->sum, obj->hdrChunk,
	   yaffs_GetObjectType(obj), yaffs_GetObjectFileLength(obj)));

	return YAFFS_OK;
=======
		return yaffs_clone_str(_Y(""));
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
}

/*--------------------------- Initialisation code -------------------------- */

static int yaffs_check_dev_fns(const struct yaffs_dev *dev)
{
	/* Common functions, gotta have */
<<<<<<< HEAD
	if (!dev->param.eraseBlockInNAND || !dev->param.initialiseNAND)
=======
	if (!dev->param.erase_fn || !dev->param.initialise_flash_fn)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		return 0;

#ifdef CONFIG_YAFFS_YAFFS2

	/* Can use the "with tags" style interface for yaffs1 or yaffs2 */
<<<<<<< HEAD
	if (dev->param.writeChunkWithTagsToNAND &&
	    dev->param.readChunkWithTagsFromNAND &&
	    !dev->param.writeChunkToNAND &&
	    !dev->param.readChunkFromNAND &&
	    dev->param.markNANDBlockBad &&
	    dev->param.queryNANDBlock)
=======
	if (dev->param.write_chunk_tags_fn &&
	    dev->param.read_chunk_tags_fn &&
	    !dev->param.write_chunk_fn &&
	    !dev->param.read_chunk_fn &&
	    dev->param.bad_block_fn && dev->param.query_block_fn)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		return 1;
#endif

	/* Can use the "spare" style interface for yaffs1 */
<<<<<<< HEAD
	if (!dev->param.isYaffs2 &&
	    !dev->param.writeChunkWithTagsToNAND &&
	    !dev->param.readChunkWithTagsFromNAND &&
	    dev->param.writeChunkToNAND &&
	    dev->param.readChunkFromNAND &&
	    !dev->param.markNANDBlockBad &&
	    !dev->param.queryNANDBlock)
=======
	if (!dev->param.is_yaffs2 &&
	    !dev->param.write_chunk_tags_fn &&
	    !dev->param.read_chunk_tags_fn &&
	    dev->param.write_chunk_fn &&
	    dev->param.read_chunk_fn &&
	    !dev->param.bad_block_fn && !dev->param.query_block_fn)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		return 1;

	return 0;	/* bad */
}

static int yaffs_create_initial_dir(struct yaffs_dev *dev)
{
	/* Initialise the unlinked, deleted, root and lost+found directories */
	dev->lost_n_found = dev->root_dir = NULL;
	dev->unlinked_dir = dev->del_dir = NULL;
	dev->unlinked_dir =
	    yaffs_create_fake_dir(dev, YAFFS_OBJECTID_UNLINKED, S_IFDIR);
	dev->del_dir =
	    yaffs_create_fake_dir(dev, YAFFS_OBJECTID_DELETED, S_IFDIR);
	dev->root_dir =
	    yaffs_create_fake_dir(dev, YAFFS_OBJECTID_ROOT,
				  YAFFS_ROOT_MODE | S_IFDIR);
	dev->lost_n_found =
	    yaffs_create_fake_dir(dev, YAFFS_OBJECTID_LOSTNFOUND,
				  YAFFS_LOSTNFOUND_MODE | S_IFDIR);

	if (dev->lost_n_found && dev->root_dir && dev->unlinked_dir
	    && dev->del_dir) {
		yaffs_add_obj_to_dir(dev->root_dir, dev->lost_n_found);
		return YAFFS_OK;
	}
	return YAFFS_FAIL;
}

int yaffs_guts_initialise(struct yaffs_dev *dev)
{
	int init_failed = 0;
	unsigned x;
	int bits;

	yaffs_trace(YAFFS_TRACE_TRACING, "yaffs: yaffs_guts_initialise()");

	/* Check stuff that must be set */

	if (!dev) {
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"yaffs: Need a device"
			);
		return YAFFS_FAIL;
	}

	if (dev->is_mounted) {
		yaffs_trace(YAFFS_TRACE_ALWAYS, "device already mounted");
		return YAFFS_FAIL;
	}

<<<<<<< HEAD
	dev->internalStartBlock = dev->param.startBlock;
	dev->internalEndBlock = dev->param.endBlock;
	dev->blockOffset = 0;
	dev->chunkOffset = 0;
	dev->nFreeChunks = 0;

	dev->gcBlock = 0;

	if (dev->param.startBlock == 0) {
		dev->internalStartBlock = dev->param.startBlock + 1;
		dev->internalEndBlock = dev->param.endBlock + 1;
		dev->blockOffset = 1;
		dev->chunkOffset = dev->param.nChunksPerBlock;
=======
	dev->internal_start_block = dev->param.start_block;
	dev->internal_end_block = dev->param.end_block;
	dev->block_offset = 0;
	dev->chunk_offset = 0;
	dev->n_free_chunks = 0;

	dev->gc_block = 0;

	if (dev->param.start_block == 0) {
		dev->internal_start_block = dev->param.start_block + 1;
		dev->internal_end_block = dev->param.end_block + 1;
		dev->block_offset = 1;
		dev->chunk_offset = dev->param.chunks_per_block;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	}

	/* Check geometry parameters. */

<<<<<<< HEAD
	if ((!dev->param.inbandTags && dev->param.isYaffs2 && dev->param.totalBytesPerChunk < 1024) ||
	    (!dev->param.isYaffs2 && dev->param.totalBytesPerChunk < 512) ||
	    (dev->param.inbandTags && !dev->param.isYaffs2) ||
	     dev->param.nChunksPerBlock < 2 ||
	     dev->param.nReservedBlocks < 2 ||
	     dev->internalStartBlock <= 0 ||
	     dev->internalEndBlock <= 0 ||
	     dev->internalEndBlock <= (dev->internalStartBlock + dev->param.nReservedBlocks + 2)) {	/* otherwise it is too small */
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR
		   ("yaffs: NAND geometry problems: chunk size %d, type is yaffs%s, inbandTags %d "
		    TENDSTR), dev->param.totalBytesPerChunk, dev->param.isYaffs2 ? "2" : "", dev->param.inbandTags));
=======
	if ((!dev->param.inband_tags && dev->param.is_yaffs2 &&
		dev->param.total_bytes_per_chunk < 1024) ||
		(!dev->param.is_yaffs2 &&
			dev->param.total_bytes_per_chunk < 512) ||
		(dev->param.inband_tags && !dev->param.is_yaffs2) ||
		 dev->param.chunks_per_block < 2 ||
		 dev->param.n_reserved_blocks < 2 ||
		dev->internal_start_block <= 0 ||
		dev->internal_end_block <= 0 ||
		dev->internal_end_block <=
		(dev->internal_start_block + dev->param.n_reserved_blocks + 2)
		) {
		/* otherwise it is too small */
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"NAND geometry problems: chunk size %d, type is yaffs%s, inband_tags %d ",
			dev->param.total_bytes_per_chunk,
			dev->param.is_yaffs2 ? "2" : "",
			dev->param.inband_tags);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		return YAFFS_FAIL;
	}

	if (yaffs_init_nand(dev) != YAFFS_OK) {
		yaffs_trace(YAFFS_TRACE_ALWAYS, "InitialiseNAND failed");
		return YAFFS_FAIL;
	}

	/* Sort out space for inband tags, if required */
<<<<<<< HEAD
	if (dev->param.inbandTags)
		dev->nDataBytesPerChunk = dev->param.totalBytesPerChunk - sizeof(yaffs_PackedTags2TagsPart);
	else
		dev->nDataBytesPerChunk = dev->param.totalBytesPerChunk;
=======
	if (dev->param.inband_tags)
		dev->data_bytes_per_chunk =
		    dev->param.total_bytes_per_chunk -
		    sizeof(struct yaffs_packed_tags2_tags_only);
	else
		dev->data_bytes_per_chunk = dev->param.total_bytes_per_chunk;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	/* Got the right mix of functions? */
	if (!yaffs_check_dev_fns(dev)) {
		/* Function missing */
		yaffs_trace(YAFFS_TRACE_ALWAYS,
			"device function(s) missing or wrong");

		return YAFFS_FAIL;
	}

	/* Finished with most checks. Further checks happen later on too. */

	dev->is_mounted = 1;

	/* OK now calculate a few things for the device */

	/*
	 *  Calculate all the chunk size manipulation numbers:
	 */
	x = dev->data_bytes_per_chunk;
	/* We always use dev->chunk_shift and dev->chunk_div */
	dev->chunk_shift = calc_shifts(x);
	x >>= dev->chunk_shift;
	dev->chunk_div = x;
	/* We only use chunk mask if chunk_div is 1 */
	dev->chunk_mask = (1 << dev->chunk_shift) - 1;

	/*
	 * Calculate chunk_grp_bits.
	 * We need to find the next power of 2 > than internal_end_block
	 */

<<<<<<< HEAD
	x = dev->param.nChunksPerBlock * (dev->internalEndBlock + 1);
=======
	x = dev->param.chunks_per_block * (dev->internal_end_block + 1);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	bits = calc_shifts_ceiling(x);

	/* Set up tnode width if wide tnodes are enabled. */
<<<<<<< HEAD
	if (!dev->param.wideTnodesDisabled) {
=======
	if (!dev->param.wide_tnodes_disabled) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		/* bits must be even so that we end up with 32-bit words */
		if (bits & 1)
			bits++;
		if (bits < 16)
			dev->tnode_width = 16;
		else
			dev->tnode_width = bits;
	} else {
		dev->tnode_width = 16;
	}

	dev->tnode_mask = (1 << dev->tnode_width) - 1;

	/* Level0 Tnodes are 16 bits or wider (if wide tnodes are enabled),
	 * so if the bitwidth of the
	 * chunk range we're using is greater than 16 we need
	 * to figure out chunk shift and chunk_grp_size
	 */

	if (bits <= dev->tnode_width)
		dev->chunk_grp_bits = 0;
	else
		dev->chunk_grp_bits = bits - dev->tnode_width;

<<<<<<< HEAD
	dev->tnodeSize = (dev->tnodeWidth * YAFFS_NTNODES_LEVEL0)/8;
	if(dev->tnodeSize < sizeof(yaffs_Tnode))
		dev->tnodeSize = sizeof(yaffs_Tnode);
=======
	dev->tnode_size = (dev->tnode_width * YAFFS_NTNODES_LEVEL0) / 8;
	if (dev->tnode_size < sizeof(struct yaffs_tnode))
		dev->tnode_size = sizeof(struct yaffs_tnode);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	dev->chunk_grp_size = 1 << dev->chunk_grp_bits;

<<<<<<< HEAD
	if (dev->param.nChunksPerBlock < dev->chunkGroupSize) {
=======
	if (dev->param.chunks_per_block < dev->chunk_grp_size) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		/* We have a problem because the soft delete won't work if
		 * the chunk group size > chunks per block.
		 * This can be remedied by using larger "virtual blocks".
		 */
		yaffs_trace(YAFFS_TRACE_ALWAYS, "chunk group too large");

		return YAFFS_FAIL;
	}

	/* Finished verifying the device, continue with initialisation */

	/* More device initialisation */
<<<<<<< HEAD
	dev->allGCs = 0;
	dev->passiveGCs = 0;
	dev->oldestDirtyGCs = 0;
	dev->backgroundGCs = 0;
	dev->gcBlockFinder = 0;
	dev->bufferedBlock = -1;
	dev->doingBufferedBlockRewrite = 0;
	dev->nDeletedFiles = 0;
	dev->nBackgroundDeletions = 0;
	dev->nUnlinkedFiles = 0;
	dev->eccFixed = 0;
	dev->eccUnfixed = 0;
	dev->tagsEccFixed = 0;
	dev->tagsEccUnfixed = 0;
	dev->nErasureFailures = 0;
	dev->nErasedBlocks = 0;
	dev->gcDisable= 0;
	dev->hasPendingPrioritisedGCs = 1; /* Assume the worst for now, will get fixed on first GC */
	YINIT_LIST_HEAD(&dev->dirtyDirectories);
	dev->oldestDirtySequence = 0;
	dev->oldestDirtyBlock = 0;
=======
	dev->all_gcs = 0;
	dev->passive_gc_count = 0;
	dev->oldest_dirty_gc_count = 0;
	dev->bg_gcs = 0;
	dev->gc_block_finder = 0;
	dev->buffered_block = -1;
	dev->doing_buffered_block_rewrite = 0;
	dev->n_deleted_files = 0;
	dev->n_bg_deletions = 0;
	dev->n_unlinked_files = 0;
	dev->n_ecc_fixed = 0;
	dev->n_ecc_unfixed = 0;
	dev->n_tags_ecc_fixed = 0;
	dev->n_tags_ecc_unfixed = 0;
	dev->n_erase_failures = 0;
	dev->n_erased_blocks = 0;
	dev->gc_disable = 0;
	dev->has_pending_prioritised_gc = 1;
		/* Assume the worst for now, will get fixed on first GC */
	INIT_LIST_HEAD(&dev->dirty_dirs);
	dev->oldest_dirty_seq = 0;
	dev->oldest_dirty_block = 0;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	/* Initialise temporary buffers and caches. */
	if (!yaffs_init_tmp_buffers(dev))
		init_failed = 1;

	dev->cache = NULL;
	dev->gc_cleanup_list = NULL;

<<<<<<< HEAD
	if (!init_failed &&
	    dev->param.nShortOpCaches > 0) {
		int i;
		void *buf;
		int srCacheBytes = dev->param.nShortOpCaches * sizeof(yaffs_ChunkCache);

		if (dev->param.nShortOpCaches > YAFFS_MAX_SHORT_OP_CACHES)
			dev->param.nShortOpCaches = YAFFS_MAX_SHORT_OP_CACHES;
=======
	if (!init_failed && dev->param.n_caches > 0) {
		int i;
		void *buf;
		int cache_bytes =
		    dev->param.n_caches * sizeof(struct yaffs_cache);

		if (dev->param.n_caches > YAFFS_MAX_SHORT_OP_CACHES)
			dev->param.n_caches = YAFFS_MAX_SHORT_OP_CACHES;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

		dev->cache = kmalloc(cache_bytes, GFP_NOFS);

		buf = (u8 *) dev->cache;

		if (dev->cache)
			memset(dev->cache, 0, cache_bytes);

<<<<<<< HEAD
		for (i = 0; i < dev->param.nShortOpCaches && buf; i++) {
			dev->srCache[i].object = NULL;
			dev->srCache[i].lastUse = 0;
			dev->srCache[i].dirty = 0;
			dev->srCache[i].data = buf = YMALLOC_DMA(dev->param.totalBytesPerChunk);
=======
		for (i = 0; i < dev->param.n_caches && buf; i++) {
			dev->cache[i].object = NULL;
			dev->cache[i].last_use = 0;
			dev->cache[i].dirty = 0;
			dev->cache[i].data = buf =
			    kmalloc(dev->param.total_bytes_per_chunk, GFP_NOFS);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		}
		if (!buf)
			init_failed = 1;

		dev->cache_last_use = 0;
	}

	dev->cache_hits = 0;

	if (!init_failed) {
<<<<<<< HEAD
		dev->gcCleanupList = YMALLOC(dev->param.nChunksPerBlock * sizeof(__u32));
		if (!dev->gcCleanupList)
			init_failed = 1;
	}

	if (dev->param.isYaffs2)
		dev->param.useHeaderFileSize = 1;
=======
		dev->gc_cleanup_list =
		    kmalloc(dev->param.chunks_per_block * sizeof(u32),
					GFP_NOFS);
		if (!dev->gc_cleanup_list)
			init_failed = 1;
	}

	if (dev->param.is_yaffs2)
		dev->param.use_header_file_size = 1;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	if (!init_failed && !yaffs_init_blocks(dev))
		init_failed = 1;

<<<<<<< HEAD
	yaffs_InitialiseTnodesAndObjects(dev);
=======
	yaffs_init_tnodes_and_objs(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	if (!init_failed && !yaffs_create_initial_dir(dev))
		init_failed = 1;

	if (!init_failed) {
		/* Now scan the flash. */
<<<<<<< HEAD
		if (dev->param.isYaffs2) {
			if (yaffs2_CheckpointRestore(dev)) {
				yaffs_CheckObjectDetailsLoaded(dev->rootDir);
				T(YAFFS_TRACE_ALWAYS,
				  (TSTR("yaffs: restored from checkpoint" TENDSTR)));
=======
		if (dev->param.is_yaffs2) {
			if (yaffs2_checkpt_restore(dev)) {
				yaffs_check_obj_details_loaded(dev->root_dir);
				yaffs_trace(YAFFS_TRACE_CHECKPOINT |
					YAFFS_TRACE_MOUNT,
					"yaffs: restored from checkpoint"
					);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
			} else {

				/* Clean up the mess caused by an aborted
				 * checkpoint load then scan backwards.
				 */
<<<<<<< HEAD
				yaffs_DeinitialiseBlocks(dev);

				yaffs_DeinitialiseTnodesAndObjects(dev);
=======
				yaffs_deinit_blocks(dev);

				yaffs_deinit_tnodes_and_objs(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

				dev->n_erased_blocks = 0;
				dev->n_free_chunks = 0;
				dev->alloc_block = -1;
				dev->alloc_page = -1;
				dev->n_deleted_files = 0;
				dev->n_unlinked_files = 0;
				dev->n_bg_deletions = 0;

				if (!init_failed && !yaffs_init_blocks(dev))
					init_failed = 1;

<<<<<<< HEAD
				yaffs_InitialiseTnodesAndObjects(dev);
=======
				yaffs_init_tnodes_and_objs(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

				if (!init_failed
				    && !yaffs_create_initial_dir(dev))
					init_failed = 1;

<<<<<<< HEAD
				if (!init_failed && !yaffs2_ScanBackwards(dev))
					init_failed = 1;
			}
		} else if (!yaffs1_Scan(dev))
				init_failed = 1;

		yaffs_StripDeletedObjects(dev);
		yaffs_FixHangingObjects(dev);
		if(dev->param.emptyLostAndFound)
			yaffs_EmptyLostAndFound(dev);
=======
				if (!init_failed && !yaffs2_scan_backwards(dev))
					init_failed = 1;
			}
		} else if (!yaffs1_scan(dev)) {
			init_failed = 1;
		}

		yaffs_strip_deleted_objs(dev);
		yaffs_fix_hanging_objs(dev);
		if (dev->param.empty_lost_n_found)
			yaffs_empty_l_n_f(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	}

	if (init_failed) {
		/* Clean up the mess */
		yaffs_trace(YAFFS_TRACE_TRACING,
		  "yaffs: yaffs_guts_initialise() aborted.");

		yaffs_deinitialise(dev);
		return YAFFS_FAIL;
	}

	/* Zero out stats */
	dev->n_page_reads = 0;
	dev->n_page_writes = 0;
	dev->n_erasures = 0;
	dev->n_gc_copies = 0;
	dev->n_retired_writes = 0;

	dev->n_retired_blocks = 0;

	yaffs_verify_free_chunks(dev);
	yaffs_verify_blocks(dev);

	/* Clean up any aborted checkpoint data */
<<<<<<< HEAD
	if(!dev->isCheckpointed && dev->blocksInCheckpoint > 0)
		yaffs2_InvalidateCheckpoint(dev);
=======
	if (!dev->is_checkpointed && dev->blocks_in_checkpt > 0)
		yaffs2_checkpt_invalidate(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	yaffs_trace(YAFFS_TRACE_TRACING,
	  "yaffs: yaffs_guts_initialise() done.");
	return YAFFS_OK;
}

void yaffs_deinitialise(struct yaffs_dev *dev)
{
	if (dev->is_mounted) {
		int i;

<<<<<<< HEAD
		yaffs_DeinitialiseBlocks(dev);
		yaffs_DeinitialiseTnodesAndObjects(dev);
		if (dev->param.nShortOpCaches > 0 &&
		    dev->srCache) {

			for (i = 0; i < dev->param.nShortOpCaches; i++) {
				if (dev->srCache[i].data)
					YFREE(dev->srCache[i].data);
				dev->srCache[i].data = NULL;
=======
		yaffs_deinit_blocks(dev);
		yaffs_deinit_tnodes_and_objs(dev);
		if (dev->param.n_caches > 0 && dev->cache) {

			for (i = 0; i < dev->param.n_caches; i++) {
				kfree(dev->cache[i].data);
				dev->cache[i].data = NULL;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
			}

			kfree(dev->cache);
			dev->cache = NULL;
		}

		kfree(dev->gc_cleanup_list);

		for (i = 0; i < YAFFS_N_TEMP_BUFFERS; i++)
			kfree(dev->temp_buffer[i].buffer);

		dev->is_mounted = 0;

<<<<<<< HEAD
		if (dev->param.deinitialiseNAND)
			dev->param.deinitialiseNAND(dev);
	}
}

int yaffs_CountFreeChunks(yaffs_Device *dev)
{
	int nFree=0;
=======
		if (dev->param.deinitialise_flash_fn)
			dev->param.deinitialise_flash_fn(dev);
	}
}

int yaffs_count_free_chunks(struct yaffs_dev *dev)
{
	int n_free = 0;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	int b;
	struct yaffs_block_info *blk;

<<<<<<< HEAD
	yaffs_BlockInfo *blk;

	blk = dev->blockInfo;
	for (b = dev->internalStartBlock; b <= dev->internalEndBlock; b++) {
		switch (blk->blockState) {
=======
	blk = dev->block_info;
	for (b = dev->internal_start_block; b <= dev->internal_end_block; b++) {
		switch (blk->block_state) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		case YAFFS_BLOCK_STATE_EMPTY:
		case YAFFS_BLOCK_STATE_ALLOCATING:
		case YAFFS_BLOCK_STATE_COLLECTING:
		case YAFFS_BLOCK_STATE_FULL:
<<<<<<< HEAD
			nFree +=
			    (dev->param.nChunksPerBlock - blk->pagesInUse +
			     blk->softDeletions);
=======
			n_free +=
			    (dev->param.chunks_per_block - blk->pages_in_use +
			     blk->soft_del_pages);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
			break;
		default:
			break;
		}
		blk++;
	}
	return n_free;
}

int yaffs_get_n_free_chunks(struct yaffs_dev *dev)
{
	/* This is what we report to the outside world */
	int n_free;
	int n_dirty_caches;
	int blocks_for_checkpt;
	int i;

	n_free = dev->n_free_chunks;
	n_free += dev->n_deleted_files;

	/* Now count and subtract the number of dirty chunks in the cache. */

<<<<<<< HEAD
	for (nDirtyCacheChunks = 0, i = 0; i < dev->param.nShortOpCaches; i++) {
		if (dev->srCache[i].dirty)
			nDirtyCacheChunks++;
	}

	nFree -= nDirtyCacheChunks;

	nFree -= ((dev->param.nReservedBlocks + 1) * dev->param.nChunksPerBlock);

	/* Now we figure out how much to reserve for the checkpoint and report that... */
	blocksForCheckpoint = yaffs2_CalcCheckpointBlocksRequired(dev);

	nFree -= (blocksForCheckpoint * dev->param.nChunksPerBlock);
=======
	for (n_dirty_caches = 0, i = 0; i < dev->param.n_caches; i++) {
		if (dev->cache[i].dirty)
			n_dirty_caches++;
	}

	n_free -= n_dirty_caches;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	n_free -=
	    ((dev->param.n_reserved_blocks + 1) * dev->param.chunks_per_block);

	/* Now figure checkpoint space and report that... */
	blocks_for_checkpt = yaffs_calc_checkpt_blocks_required(dev);

<<<<<<< HEAD
}


/*---------------------------------------- YAFFS test code ----------------------*/

#define yaffs_CheckStruct(structure, syze, name) \
	do { \
		if (sizeof(structure) != syze) { \
			T(YAFFS_TRACE_ALWAYS, (TSTR("%s should be %d but is %d\n" TENDSTR),\
				name, syze, (int) sizeof(structure))); \
			return YAFFS_FAIL; \
		} \
	} while (0)

static int yaffs_CheckStructures(void)
{
/*      yaffs_CheckStruct(yaffs_Tags,8,"yaffs_Tags"); */
/*      yaffs_CheckStruct(yaffs_TagsUnion,8,"yaffs_TagsUnion"); */
/*      yaffs_CheckStruct(yaffs_Spare,16,"yaffs_Spare"); */
/*	yaffs_CheckStruct(yaffs_Tnode, 2 * YAFFS_NTNODES_LEVEL0, "yaffs_Tnode"); */

#ifndef CONFIG_YAFFS_WINCE
	yaffs_CheckStruct(yaffs_ObjectHeader, 512, "yaffs_ObjectHeader");
#endif
	return YAFFS_OK;
=======
	n_free -= (blocks_for_checkpt * dev->param.chunks_per_block);

	if (n_free < 0)
		n_free = 0;

	return n_free;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
}
