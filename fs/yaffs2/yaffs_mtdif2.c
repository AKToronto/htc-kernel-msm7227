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

/* mtd interface for YAFFS2 */

#include "yportenv.h"
#include "yaffs_trace.h"

#include "yaffs_mtdif2.h"

#include "linux/mtd/mtd.h"
#include "linux/types.h"
#include "linux/time.h"

#include "yaffs_packedtags2.h"

#include "yaffs_linux.h"

/* NB For use with inband tags....
 * We assume that the data buffer is of size total_bytes_per_chunk so
 * that we can also use it to load the tags.
 */
int nandmtd2_write_chunk_tags(struct yaffs_dev *dev, int nand_chunk,
			      const u8 *data,
			      const struct yaffs_ext_tags *tags)
{
<<<<<<< HEAD
	struct mtd_info *mtd = yaffs_DeviceToMtd(dev);
=======
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
#if (MTD_VERSION_CODE > MTD_VERSION(2, 6, 17))
	struct mtd_oob_ops ops;
#else
	size_t dummy;
#endif
	int retval = 0;

	loff_t addr;

	struct yaffs_packed_tags2 pt;

<<<<<<< HEAD
	int packed_tags_size = dev->param.noTagsECC ? sizeof(pt.t) : sizeof(pt);
	void * packed_tags_ptr = dev->param.noTagsECC ? (void *) &pt.t : (void *)&pt;

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_WriteChunkWithTagsToNAND chunk %d data %p tags %p"
	    TENDSTR), chunkInNAND, data, tags));


	addr  = ((loff_t) chunkInNAND) * dev->param.totalBytesPerChunk;
=======
	int packed_tags_size =
	    dev->param.no_tags_ecc ? sizeof(pt.t) : sizeof(pt);
	void *packed_tags_ptr =
	    dev->param.no_tags_ecc ? (void *)&pt.t : (void *)&pt;

	yaffs_trace(YAFFS_TRACE_MTD,
		"nandmtd2_write_chunk_tags chunk %d data %p tags %p",
		nand_chunk, data, tags);

	addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	/* For yaffs2 writing there must be both data and tags.
	 * If we're using inband tags, then the tags are stuffed into
	 * the end of the data buffer.
	 */
	if (!data || !tags)
		BUG();
<<<<<<< HEAD
	else if (dev->param.inbandTags) {
		yaffs_PackedTags2TagsPart *pt2tp;
		pt2tp = (yaffs_PackedTags2TagsPart *)(data + dev->nDataBytesPerChunk);
		yaffs_PackTags2TagsPart(pt2tp, tags);
	} else
		yaffs_PackTags2(&pt, tags, !dev->param.noTagsECC);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
	ops.mode = MTD_OOB_AUTO;
	ops.ooblen = (dev->param.inbandTags) ? 0 : packed_tags_size;
	ops.len = dev->param.totalBytesPerChunk;
	ops.ooboffs = 0;
	ops.datbuf = (__u8 *)data;
	ops.oobbuf = (dev->param.inbandTags) ? NULL : packed_tags_ptr;
	retval = mtd->write_oob(mtd, addr, &ops);

#else
	if (!dev->param.inbandTags) {
		retval =
		    mtd->write_ecc(mtd, addr, dev->nDataBytesPerChunk,
				   &dummy, data, (__u8 *) packed_tags_ptr, NULL);
	} else {
		retval =
		    mtd->write(mtd, addr, dev->param.totalBytesPerChunk, &dummy,
			       data);
=======
	else if (dev->param.inband_tags) {
		struct yaffs_packed_tags2_tags_only *pt2tp;
		pt2tp =
		    (struct yaffs_packed_tags2_tags_only *)(data +
							dev->
							data_bytes_per_chunk);
		yaffs_pack_tags2_tags_only(pt2tp, tags);
	} else {
		yaffs_pack_tags2(&pt, tags, !dev->param.no_tags_ecc);
	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
	ops.mode = MTD_OOB_AUTO;
	ops.ooblen = (dev->param.inband_tags) ? 0 : packed_tags_size;
	ops.len = dev->param.total_bytes_per_chunk;
	ops.ooboffs = 0;
	ops.datbuf = (u8 *) data;
	ops.oobbuf = (dev->param.inband_tags) ? NULL : packed_tags_ptr;
	retval = mtd->write_oob(mtd, addr, &ops);

#else
	if (!dev->param.inband_tags) {
		retval =
		    mtd->write_ecc(mtd, addr, dev->data_bytes_per_chunk,
				   &dummy, data, (u8 *) packed_tags_ptr, NULL);
	} else {
		retval =
		    mtd->write(mtd, addr, dev->param.total_bytes_per_chunk,
			       &dummy, data);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	}
#endif

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_read_chunk_tags(struct yaffs_dev *dev, int nand_chunk,
			     u8 *data, struct yaffs_ext_tags *tags)
{
<<<<<<< HEAD
	struct mtd_info *mtd = yaffs_DeviceToMtd(dev);
=======
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
#if (MTD_VERSION_CODE > MTD_VERSION(2, 6, 17))
	struct mtd_oob_ops ops;
#endif
	size_t dummy;
	int retval = 0;
	int local_data = 0;

<<<<<<< HEAD
	loff_t addr = ((loff_t) chunkInNAND) * dev->param.totalBytesPerChunk;
=======
	loff_t addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	struct yaffs_packed_tags2 pt;

<<<<<<< HEAD
	int packed_tags_size = dev->param.noTagsECC ? sizeof(pt.t) : sizeof(pt);
	void * packed_tags_ptr = dev->param.noTagsECC ? (void *) &pt.t: (void *)&pt;

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_ReadChunkWithTagsFromNAND chunk %d data %p tags %p"
	    TENDSTR), chunkInNAND, data, tags));

	if (dev->param.inbandTags) {
=======
	int packed_tags_size =
	    dev->param.no_tags_ecc ? sizeof(pt.t) : sizeof(pt);
	void *packed_tags_ptr =
	    dev->param.no_tags_ecc ? (void *)&pt.t : (void *)&pt;

	yaffs_trace(YAFFS_TRACE_MTD,
		"nandmtd2_read_chunk_tags chunk %d data %p tags %p",
		nand_chunk, data, tags);

	if (dev->param.inband_tags) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

		if (!data) {
			local_data = 1;
			data = yaffs_get_temp_buffer(dev);
		}

	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
<<<<<<< HEAD
	if (dev->param.inbandTags || (data && !tags))
		retval = mtd->read(mtd, addr, dev->param.totalBytesPerChunk,
				&dummy, data);
	else if (tags) {
		ops.mode = MTD_OOB_AUTO;
		ops.ooblen = packed_tags_size;
		ops.len = data ? dev->nDataBytesPerChunk : packed_tags_size;
		ops.ooboffs = 0;
		ops.datbuf = data;
		ops.oobbuf = yaffs_DeviceToLC(dev)->spareBuffer;
		retval = mtd->read_oob(mtd, addr, &ops);
	}
#else
	if (!dev->param.inbandTags && data && tags) {
=======
	if (dev->param.inband_tags || (data && !tags))
		retval = mtd->read(mtd, addr, dev->param.total_bytes_per_chunk,
				   &dummy, data);
	else if (tags) {
		ops.mode = MTD_OOB_AUTO;
		ops.ooblen = packed_tags_size;
		ops.len = data ? dev->data_bytes_per_chunk : packed_tags_size;
		ops.ooboffs = 0;
		ops.datbuf = data;
		ops.oobbuf = yaffs_dev_to_lc(dev)->spare_buffer;
		retval = mtd->read_oob(mtd, addr, &ops);
	}
#else
	if (!dev->param.inband_tags && data && tags) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

		retval = mtd->read_ecc(mtd, addr, dev->data_bytes_per_chunk,
				       &dummy, data, dev->spare_buffer, NULL);
	} else {
		if (data)
			retval =
<<<<<<< HEAD
			    mtd->read(mtd, addr, dev->nDataBytesPerChunk, &dummy,
				      data);
		if (!dev->param.inbandTags && tags)
=======
			    mtd->read(mtd, addr, dev->data_bytes_per_chunk,
				      &dummy, data);
		if (!dev->param.inband_tags && tags)
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
			retval =
			    mtd->read_oob(mtd, addr, mtd->oobsize, &dummy,
					  dev->spare_buffer);
	}
#endif

<<<<<<< HEAD

	if (dev->param.inbandTags) {
=======
	if (dev->param.inband_tags) {
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		if (tags) {
			struct yaffs_packed_tags2_tags_only *pt2tp;
			pt2tp =
				(struct yaffs_packed_tags2_tags_only *)
				&data[dev->data_bytes_per_chunk];
			yaffs_unpack_tags2_tags_only(tags, pt2tp);
		}
	} else {
		if (tags) {
<<<<<<< HEAD
			memcpy(packed_tags_ptr, yaffs_DeviceToLC(dev)->spareBuffer, packed_tags_size);
			yaffs_UnpackTags2(tags, &pt, !dev->param.noTagsECC);
=======
			memcpy(packed_tags_ptr,
			       yaffs_dev_to_lc(dev)->spare_buffer,
			       packed_tags_size);
			yaffs_unpack_tags2(tags, &pt, !dev->param.no_tags_ecc);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		}
	}

	if (local_data)
		yaffs_release_temp_buffer(dev, data);

	if (tags && retval == -EBADMSG
	    && tags->ecc_result == YAFFS_ECC_RESULT_NO_ERROR) {
		tags->ecc_result = YAFFS_ECC_RESULT_UNFIXED;
		dev->n_ecc_unfixed++;
	}
<<<<<<< HEAD
	if(tags && retval == -EUCLEAN && tags->eccResult == YAFFS_ECC_RESULT_NO_ERROR) {
		tags->eccResult = YAFFS_ECC_RESULT_FIXED;
		dev->eccFixed++;
=======
	if (tags && retval == -EUCLEAN
	    && tags->ecc_result == YAFFS_ECC_RESULT_NO_ERROR) {
		tags->ecc_result = YAFFS_ECC_RESULT_FIXED;
		dev->n_ecc_fixed++;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	}
	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_mark_block_bad(struct yaffs_dev *dev, int block_no)
{
<<<<<<< HEAD
	struct mtd_info *mtd = yaffs_DeviceToMtd(dev);
=======
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	int retval;
	yaffs_trace(YAFFS_TRACE_MTD,
		"nandmtd2_mark_block_bad %d",
		block_no);

	retval =
	    mtd->block_markbad(mtd,
<<<<<<< HEAD
			       blockNo * dev->param.nChunksPerBlock *
			       dev->param.totalBytesPerChunk);
=======
			       block_no * dev->param.chunks_per_block *
			       dev->param.total_bytes_per_chunk);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;

}

int nandmtd2_query_block(struct yaffs_dev *dev, int block_no,
			 enum yaffs_block_state *state, u32 *seq_number)
{
<<<<<<< HEAD
	struct mtd_info *mtd = yaffs_DeviceToMtd(dev);
=======
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
	int retval;

	yaffs_trace(YAFFS_TRACE_MTD, "nandmtd2_query_block %d", block_no);
	retval =
	    mtd->block_isbad(mtd,
<<<<<<< HEAD
			     blockNo * dev->param.nChunksPerBlock *
			     dev->param.totalBytesPerChunk);
=======
			     block_no * dev->param.chunks_per_block *
			     dev->param.total_bytes_per_chunk);
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000

	if (retval) {
		yaffs_trace(YAFFS_TRACE_MTD, "block is bad");

		*state = YAFFS_BLOCK_STATE_DEAD;
		*seq_number = 0;
	} else {
<<<<<<< HEAD
		yaffs_ExtendedTags t;
		nandmtd2_ReadChunkWithTagsFromNAND(dev,
						   blockNo *
						   dev->param.nChunksPerBlock, NULL,
						   &t);

		if (t.chunkUsed) {
			*sequenceNumber = t.sequenceNumber;
			*state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;
=======
		struct yaffs_ext_tags t;
		nandmtd2_read_chunk_tags(dev, block_no *
					 dev->param.chunks_per_block, NULL, &t);

		if (t.chunk_used) {
			*seq_number = t.seq_number;
			*state = YAFFS_BLOCK_STATE_NEEDS_SCAN;
>>>>>>> 1e6c135... yaffs2: Import YAFFS2 as of Mon, 7 Mar 2011 03:25:28 +0000
		} else {
			*seq_number = 0;
			*state = YAFFS_BLOCK_STATE_EMPTY;
		}
	}
	yaffs_trace(YAFFS_TRACE_MTD,
		"block is bad seq %d state %d",
		*seq_number, *state);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

