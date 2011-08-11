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

#include "yaffs_guts.h"
#include "yaffs_tagscompat.h"
#include "yaffs_ecc.h"
#include "yaffs_getblockinfo.h"
#include "yaffs_trace.h"
<<<<<<< HEAD
=======

static void yaffs_handle_rd_data_error(struct yaffs_dev *dev, int nand_chunk);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000


/********** Tags ECC calculations  *********/

void yaffs_calc_ecc(const u8 *data, struct yaffs_spare *spare)
{
	yaffs_ecc_cacl(data, spare->ecc1);
	yaffs_ecc_cacl(&data[256], spare->ecc2);
}

void yaffs_calc_tags_ecc(struct yaffs_tags *tags)
{
	/* Calculate an ecc */
	unsigned char *b = ((union yaffs_tags_union *)tags)->as_bytes;
	unsigned i, j;
	unsigned ecc = 0;
	unsigned bit = 0;

	tags->ecc = 0;

	for (i = 0; i < 8; i++) {
		for (j = 1; j & 0xff; j <<= 1) {
			bit++;
			if (b[i] & j)
				ecc ^= bit;
		}
	}
	tags->ecc = ecc;
}

int yaffs_check_tags_ecc(struct yaffs_tags *tags)
{
	unsigned ecc = tags->ecc;

	yaffs_calc_tags_ecc(tags);

	ecc ^= tags->ecc;

	if (ecc && ecc <= 64) {
		/* TODO: Handle the failure better. Retire? */
		unsigned char *b = ((union yaffs_tags_union *)tags)->as_bytes;

		ecc--;

		b[ecc / 8] ^= (1 << (ecc & 7));

		/* Now recvalc the ecc */
		yaffs_calc_tags_ecc(tags);

		return 1;	/* recovered error */
	} else if (ecc) {
		/* Wierd ecc failure value */
		/* TODO Need to do somethiong here */
		return -1;	/* unrecovered error */
	}
	return 0;
}

/********** Tags **********/

static void yaffs_load_tags_to_spare(struct yaffs_spare *spare_ptr,
				     struct yaffs_tags *tags_ptr)
{
	union yaffs_tags_union *tu = (union yaffs_tags_union *)tags_ptr;

	yaffs_calc_tags_ecc(tags_ptr);

	spare_ptr->tb0 = tu->as_bytes[0];
	spare_ptr->tb1 = tu->as_bytes[1];
	spare_ptr->tb2 = tu->as_bytes[2];
	spare_ptr->tb3 = tu->as_bytes[3];
	spare_ptr->tb4 = tu->as_bytes[4];
	spare_ptr->tb5 = tu->as_bytes[5];
	spare_ptr->tb6 = tu->as_bytes[6];
	spare_ptr->tb7 = tu->as_bytes[7];
}

static void yaffs_get_tags_from_spare(struct yaffs_dev *dev,
				      struct yaffs_spare *spare_ptr,
				      struct yaffs_tags *tags_ptr)
{
	union yaffs_tags_union *tu = (union yaffs_tags_union *)tags_ptr;
	int result;

	tu->as_bytes[0] = spare_ptr->tb0;
	tu->as_bytes[1] = spare_ptr->tb1;
	tu->as_bytes[2] = spare_ptr->tb2;
	tu->as_bytes[3] = spare_ptr->tb3;
	tu->as_bytes[4] = spare_ptr->tb4;
	tu->as_bytes[5] = spare_ptr->tb5;
	tu->as_bytes[6] = spare_ptr->tb6;
	tu->as_bytes[7] = spare_ptr->tb7;

	result = yaffs_check_tags_ecc(tags_ptr);
	if (result > 0)
		dev->n_tags_ecc_fixed++;
	else if (result < 0)
		dev->n_tags_ecc_unfixed++;
}

static void yaffs_spare_init(struct yaffs_spare *spare)
{
	memset(spare, 0xff, sizeof(struct yaffs_spare));
}

static int yaffs_wr_nand(struct yaffs_dev *dev,
			 int nand_chunk, const u8 *data,
			 struct yaffs_spare *spare)
{
<<<<<<< HEAD
	if (chunkInNAND < dev->param.startBlock * dev->param.nChunksPerBlock) {
		T(YAFFS_TRACE_ERROR,
		  (TSTR("**>> yaffs chunk %d is not valid" TENDSTR),
		   chunkInNAND));
		return YAFFS_FAIL;
	}

	return dev->param.writeChunkToNAND(dev, chunkInNAND, data, spare);
=======
	if (nand_chunk < dev->param.start_block * dev->param.chunks_per_block) {
		yaffs_trace(YAFFS_TRACE_ERROR,
			"**>> yaffs chunk %d is not valid",
			nand_chunk);
		return YAFFS_FAIL;
	}

	return dev->param.write_chunk_fn(dev, nand_chunk, data, spare);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
}

static int yaffs_rd_chunk_nand(struct yaffs_dev *dev,
			       int nand_chunk,
			       u8 *data,
			       struct yaffs_spare *spare,
			       enum yaffs_ecc_result *ecc_result,
			       int correct_errors)
{
	int ret_val;
	struct yaffs_spare local_spare;

	if (!spare) {
		/* If we don't have a real spare, then we use a local one. */
		/* Need this for the calculation of the ecc */
		spare = &local_spare;
	}

<<<<<<< HEAD
	if (!dev->param.useNANDECC) {
		retVal = dev->param.readChunkFromNAND(dev, chunkInNAND, data, spare);
		if (data && doErrorCorrection) {
=======
	if (!dev->param.use_nand_ecc) {
		ret_val =
		    dev->param.read_chunk_fn(dev, nand_chunk, data, spare);
		if (data && correct_errors) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
			/* Do ECC correction */
			/* Todo handle any errors */
			int ecc_result1, ecc_result2;
			u8 calc_ecc[3];

			yaffs_ecc_cacl(data, calc_ecc);
			ecc_result1 =
			    yaffs_ecc_correct(data, spare->ecc1, calc_ecc);
			yaffs_ecc_cacl(&data[256], calc_ecc);
			ecc_result2 =
			    yaffs_ecc_correct(&data[256], spare->ecc2,
					      calc_ecc);

			if (ecc_result1 > 0) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"**>>yaffs ecc error fix performed on chunk %d:0",
					nand_chunk);
				dev->n_ecc_fixed++;
			} else if (ecc_result1 < 0) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"**>>yaffs ecc error unfixed on chunk %d:0",
					nand_chunk);
				dev->n_ecc_unfixed++;
			}

			if (ecc_result2 > 0) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"**>>yaffs ecc error fix performed on chunk %d:1",
					nand_chunk);
				dev->n_ecc_fixed++;
			} else if (ecc_result2 < 0) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"**>>yaffs ecc error unfixed on chunk %d:1",
					nand_chunk);
				dev->n_ecc_unfixed++;
			}

			if (ecc_result1 || ecc_result2) {
				/* We had a data problem on this page */
				yaffs_handle_rd_data_error(dev, nand_chunk);
			}

			if (ecc_result1 < 0 || ecc_result2 < 0)
				*ecc_result = YAFFS_ECC_RESULT_UNFIXED;
			else if (ecc_result1 > 0 || ecc_result2 > 0)
				*ecc_result = YAFFS_ECC_RESULT_FIXED;
			else
				*ecc_result = YAFFS_ECC_RESULT_NO_ERROR;
		}
	} else {
		/* Must allocate enough memory for spare+2*sizeof(int) */
		/* for ecc results from device. */
		struct yaffs_nand_spare nspare;

		memset(&nspare, 0, sizeof(nspare));

<<<<<<< HEAD
		retVal = dev->param.readChunkFromNAND(dev, chunkInNAND, data,
					(yaffs_Spare *) &nspare);
		memcpy(spare, &nspare, sizeof(yaffs_Spare));
		if (data && doErrorCorrection) {
=======
		ret_val = dev->param.read_chunk_fn(dev, nand_chunk, data,
						   (struct yaffs_spare *)
						   &nspare);
		memcpy(spare, &nspare, sizeof(struct yaffs_spare));
		if (data && correct_errors) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
			if (nspare.eccres1 > 0) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"**>>mtd ecc error fix performed on chunk %d:0",
					nand_chunk);
			} else if (nspare.eccres1 < 0) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"**>>mtd ecc error unfixed on chunk %d:0",
					nand_chunk);
			}

			if (nspare.eccres2 > 0) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"**>>mtd ecc error fix performed on chunk %d:1",
					nand_chunk);
			} else if (nspare.eccres2 < 0) {
				yaffs_trace(YAFFS_TRACE_ERROR,
					"**>>mtd ecc error unfixed on chunk %d:1",
					nand_chunk);
			}

			if (nspare.eccres1 || nspare.eccres2) {
				/* We had a data problem on this page */
				yaffs_handle_rd_data_error(dev, nand_chunk);
			}

			if (nspare.eccres1 < 0 || nspare.eccres2 < 0)
				*ecc_result = YAFFS_ECC_RESULT_UNFIXED;
			else if (nspare.eccres1 > 0 || nspare.eccres2 > 0)
				*ecc_result = YAFFS_ECC_RESULT_FIXED;
			else
				*ecc_result = YAFFS_ECC_RESULT_NO_ERROR;

		}
	}
<<<<<<< HEAD
	return retVal;
}

#ifdef NOTYET
static int yaffs_CheckChunkErased(struct yaffs_DeviceStruct *dev,
				  int chunkInNAND)
{
	static int init;
	static __u8 cmpbuf[YAFFS_BYTES_PER_CHUNK];
	static __u8 data[YAFFS_BYTES_PER_CHUNK];
	/* Might as well always allocate the larger size for */
	/* dev->param.useNANDECC == true; */
	static __u8 spare[sizeof(struct yaffs_NANDSpare)];

	dev->param.readChunkFromNAND(dev, chunkInNAND, data, (yaffs_Spare *) spare);

	if (!init) {
		memset(cmpbuf, 0xff, YAFFS_BYTES_PER_CHUNK);
		init = 1;
	}

	if (memcmp(cmpbuf, data, YAFFS_BYTES_PER_CHUNK))
		return YAFFS_FAIL;
	if (memcmp(cmpbuf, spare, 16))
		return YAFFS_FAIL;

	return YAFFS_OK;

=======
	return ret_val;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
}

/*
 * Functions for robustisizing
 */

static void yaffs_handle_rd_data_error(struct yaffs_dev *dev, int nand_chunk)
{
<<<<<<< HEAD
	int blockInNAND = chunkInNAND / dev->param.nChunksPerBlock;
=======
	int flash_block = nand_chunk / dev->param.chunks_per_block;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	/* Mark the block for retirement */
	yaffs_get_block_info(dev, flash_block + dev->block_offset)->
		needs_retiring = 1;
	yaffs_trace(YAFFS_TRACE_ERROR | YAFFS_TRACE_BAD_BLOCKS,
		"**>>Block %d marked for retirement",
		flash_block);

	/* TODO:
	 * Just do a garbage collection on the affected block
	 * then retire the block
	 * NB recursion
	 */
}

<<<<<<< HEAD
#ifdef NOTYET
static void yaffs_CheckWrittenBlock(yaffs_Device *dev, int chunkInNAND)
{
}

static void yaffs_HandleWriteChunkOk(yaffs_Device *dev, int chunkInNAND,
				     const __u8 *data,
				     const yaffs_Spare *spare)
{
}

static void yaffs_HandleUpdateChunk(yaffs_Device *dev, int chunkInNAND,
				    const yaffs_Spare *spare)
{
}

static void yaffs_HandleWriteChunkError(yaffs_Device *dev, int chunkInNAND)
{
	int blockInNAND = chunkInNAND / dev->param.nChunksPerBlock;

	/* Mark the block for retirement */
	yaffs_GetBlockInfo(dev, blockInNAND)->needsRetiring = 1;
	/* Delete the chunk */
	yaffs_DeleteChunk(dev, chunkInNAND, 1, __LINE__);
}

static int yaffs_VerifyCompare(const __u8 *d0, const __u8 *d1,
			       const yaffs_Spare *s0, const yaffs_Spare *s1)
=======
int yaffs_tags_compat_wr(struct yaffs_dev *dev,
			 int nand_chunk,
			 const u8 *data, const struct yaffs_ext_tags *ext_tags)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
{
	struct yaffs_spare spare;
	struct yaffs_tags tags;

	yaffs_spare_init(&spare);

	if (ext_tags->is_deleted)
		spare.page_status = 0;
	else {
		tags.obj_id = ext_tags->obj_id;
		tags.chunk_id = ext_tags->chunk_id;

		tags.n_bytes_lsb = ext_tags->n_bytes & (1024 - 1);

		if (dev->data_bytes_per_chunk >= 1024)
			tags.n_bytes_msb = (ext_tags->n_bytes >> 10) & 3;
		else
			tags.n_bytes_msb = 3;

		tags.serial_number = ext_tags->serial_number;

<<<<<<< HEAD
		tags.serialNumber = eTags->serialNumber;

		if (!dev->param.useNANDECC && data)
			yaffs_CalcECC(data, &spare);

		yaffs_LoadTagsIntoSpare(&spare, &tags);
=======
		if (!dev->param.use_nand_ecc && data)
			yaffs_calc_ecc(data, &spare);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

		yaffs_load_tags_to_spare(&spare, &tags);
	}
	return yaffs_wr_nand(dev, nand_chunk, data, &spare);
}

int yaffs_tags_compat_rd(struct yaffs_dev *dev,
			 int nand_chunk,
			 u8 *data, struct yaffs_ext_tags *ext_tags)
{
	struct yaffs_spare spare;
	struct yaffs_tags tags;
	enum yaffs_ecc_result ecc_result = YAFFS_ECC_RESULT_UNKNOWN;
	static struct yaffs_spare spare_ff;
	static int init;
	int deleted;

	if (!init) {
		memset(&spare_ff, 0xff, sizeof(spare_ff));
		init = 1;
	}

	if (!yaffs_rd_chunk_nand(dev, nand_chunk,
					data, &spare, &ecc_result, 1))
		return YAFFS_FAIL;

	/* ext_tags may be NULL */
	if (!ext_tags)
		return YAFFS_OK;

	deleted = (hweight8(spare.page_status) < 7) ? 1 : 0;

	ext_tags->is_deleted = deleted;
	ext_tags->ecc_result = ecc_result;
	ext_tags->block_bad = 0;	/* We're reading it */
	/* therefore it is not a bad block */
	ext_tags->chunk_used =
		memcmp(&spare_ff, &spare, sizeof(spare_ff)) ? 1 : 0;

	if (ext_tags->chunk_used) {
		yaffs_get_tags_from_spare(dev, &spare, &tags);
		ext_tags->obj_id = tags.obj_id;
		ext_tags->chunk_id = tags.chunk_id;
		ext_tags->n_bytes = tags.n_bytes_lsb;

		if (dev->data_bytes_per_chunk >= 1024)
			ext_tags->n_bytes |=
				(((unsigned)tags.n_bytes_msb) << 10);

		ext_tags->serial_number = tags.serial_number;
	}

	return YAFFS_OK;
}

int yaffs_tags_compat_mark_bad(struct yaffs_dev *dev, int flash_block)
{
	struct yaffs_spare spare;

	memset(&spare, 0xff, sizeof(struct yaffs_spare));

	spare.block_status = 'Y';

<<<<<<< HEAD
	spare.blockStatus = 'Y';

	yaffs_WriteChunkToNAND(dev, blockInNAND * dev->param.nChunksPerBlock, NULL,
			       &spare);
	yaffs_WriteChunkToNAND(dev, blockInNAND * dev->param.nChunksPerBlock + 1,
			       NULL, &spare);
=======
	yaffs_wr_nand(dev, flash_block * dev->param.chunks_per_block, NULL,
		      &spare);
	yaffs_wr_nand(dev, flash_block * dev->param.chunks_per_block + 1,
		      NULL, &spare);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	return YAFFS_OK;
}

int yaffs_tags_compat_query_block(struct yaffs_dev *dev,
				  int block_no,
				  enum yaffs_block_state *state,
				  u32 *seq_number)
{
	struct yaffs_spare spare0, spare1;
	static struct yaffs_spare spare_ff;
	static int init;
	enum yaffs_ecc_result dummy;

	if (!init) {
		memset(&spare_ff, 0xff, sizeof(spare_ff));
		init = 1;
	}

	*seq_number = 0;

<<<<<<< HEAD
	yaffs_ReadChunkFromNAND(dev, blockNo * dev->param.nChunksPerBlock, NULL,
				&spare0, &dummy, 1);
	yaffs_ReadChunkFromNAND(dev, blockNo * dev->param.nChunksPerBlock + 1, NULL,
				&spare1, &dummy, 1);
=======
	yaffs_rd_chunk_nand(dev, block_no * dev->param.chunks_per_block, NULL,
			    &spare0, &dummy, 1);
	yaffs_rd_chunk_nand(dev, block_no * dev->param.chunks_per_block + 1,
			    NULL, &spare1, &dummy, 1);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	if (hweight8(spare0.block_status & spare1.block_status) < 7)
		*state = YAFFS_BLOCK_STATE_DEAD;
	else if (memcmp(&spare_ff, &spare0, sizeof(spare_ff)) == 0)
		*state = YAFFS_BLOCK_STATE_EMPTY;
	else
		*state = YAFFS_BLOCK_STATE_NEEDS_SCAN;

	return YAFFS_OK;
}
