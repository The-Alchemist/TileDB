/**
 * @file   helpers.h
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017-2019 TileDB, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * This file declares some test suite helper functions.
 */

#ifndef TILEDB_TEST_HELPERS_H
#define TILEDB_TEST_HELPERS_H

#include "tiledb.h"
#include "tiledb/sm/array/array.h"
#include "tiledb/sm/enums/layout.h"
#include "tiledb/sm/subarray/subarray.h"

#include <sstream>
#include <string>
#include <thread>

// For easy reference
typedef std::pair<tiledb_filter_type_t, int> Compressor;
template <class T>
using SubarrayRanges = std::vector<std::vector<T>>;

/** Helper struct for the buffers of an attribute (fixed- or var-sized). */
struct AttrBuffer {
  /**
   * For fixed-sized attributes, it contains the fixed-sized values.
   * For var-sized attributes, it contains the offsets.
   * var buffer is nullptr.
   */
  void* fixed_;
  /** Size of fixed buffer. */
  uint64_t fixed_size_;
  /**
   * For fixed-sized attributes, it is `nullptr`.
   * For var-sized attributes, it contains the var-sized values.
   */
  void* var_;
  /** Size of var buffer. */
  uint64_t var_size_;
};
/** Map attribute_name -> AttrBuffer */
typedef std::map<std::string, AttrBuffer> AttrBuffers;

/**
 * Checks that the input partitioner produces the input partitions
 * (i.e., subarrays).
 *
 * @tparam T The datatype of the subarray of the partitioner.
 * @param partitioner The partitioner.
 * @param ranges The ranges to be checked.
 * @param last_unsplittable Whether the last partition is unsplittable.
 */
template <class T>
void check_partitions(
    tiledb::sm::SubarrayPartitioner& partitioner,
    const std::vector<SubarrayRanges<T>>& partitions,
    bool last_unsplittable);

/**
 * Checks if the input subarray has the input subarray ranges.
 *
 * @tparam T The subarray domain datatype
 * @param subarray The subarray to be checked.
 * @param ranges The ranges to be checked (a vector of ranges per dimension).
 */
template <class T>
void check_subarray(
    tiledb::sm::Subarray& subarray, const SubarrayRanges<T>& ranges);

/**
 * Closes an array.
 *
 * @param ctx The TileDB context.
 * @param array The array to be closed.
 */
void close_array(tiledb_ctx_t* ctx, tiledb_array_t* array);

/**
 * Helper method to create an array.
 *
 * @param ctx TileDB context.
 * @param array_name The array name.
 * @param array_type The array type (dense or sparse).
 * @param dim_names The names of dimensions.
 * @param dim_types The types of dimensions.
 * @param dim_domains The domains of dimensions.
 * @param tile_extents The tile extents of dimensions.
 * @param attr_names The names of attributes.
 * @param attr_types The types of attributes.
 * @param cell_val_num The number of values per cell of attributes.
 * @param compressors The compressors of attributes.
 * @param tile_order The tile order.
 * @param cell_order The cell order.
 * @param capacity The tile capacity.
 */

void create_array(
    tiledb_ctx_t* ctx,
    const std::string& array_name,
    tiledb_array_type_t array_type,
    const std::vector<std::string>& dim_names,
    const std::vector<tiledb_datatype_t>& dim_types,
    const std::vector<void*>& dim_domains,
    const std::vector<void*>& tile_extents,
    const std::vector<std::string>& attr_names,
    const std::vector<tiledb_datatype_t>& attr_types,
    const std::vector<uint32_t>& cell_val_num,
    const std::vector<std::pair<tiledb_filter_type_t, int>>& compressors,
    tiledb_layout_t tile_order,
    tiledb_layout_t cell_order,
    uint64_t capacity);

/**
 * Helper method that creates a directory.
 *
 * @param path The name of the directory to be created.
 * @param ctx The TileDB context.
 * @param vfs The VFS object that will create the directory.
 */
void create_dir(const std::string& path, tiledb_ctx_t* ctx, tiledb_vfs_t* vfs);

/**
 * Helper method that creates an S3 bucket (if it does not already exist).
 *
 * @param bucket_name The name of the bucket to be created.
 * @param s3_supported The bucket will be created only if this is `true`.
 * @param ctx The TileDB context.
 * @param vfs The VFS object that will create the bucket.
 */
void create_s3_bucket(
    const std::string& bucket_name,
    bool s3_supported,
    tiledb_ctx_t* ctx,
    tiledb_vfs_t* vfs);

/**
 * Creates a subarray for the input array.
 *
 * @tparam T The datatype of the subarray domain.
 * @param array The input array.
 * @param ranges The ranges of the subarray to be created.
 * @param layout The layout of the subarray.
 * @param subarray The subarray to be set.
 */
template <class T>
void create_subarray(
    tiledb::sm::Array* array,
    const SubarrayRanges<T>& ranges,
    tiledb::sm::Layout layout,
    tiledb::sm::Subarray* subarray);

/**
 * Helper method that creates a TileDB context and a VFS object.
 *
 * @param s3_supported Indicates whether S3 is supported or not.
 * @param ctx The TileDB context to be created.
 * @param vfs The VFS object to be created.
 */
void create_ctx_and_vfs(
    bool s3_supported, tiledb_ctx_t** ctx, tiledb_vfs_t** vfs);

/**
 * Helper function to get the supported filesystems.
 *
 * @param s3_supported Set to `true` if S3 is supported.
 * @param hdfs_supported Set to `false` if HDFS is supported.
 */
void get_supported_fs(bool* s3_supported, bool* hdfs_supported);

/**
 * Opens an array.
 *
 * @param ctx The TileDB context.
 * @param array The array to be opened.
 * @param query_type The query type.
 */
void open_array(tiledb_ctx_t* ctx, tiledb_array_t* array, tiledb_query_type_t);

/**
 * Returns a random bucket name, with `prefix` as prefix and using
 * the thread id as a "random" suffix.
 *
 * @param prefix The prefix of the bucket name.
 * @return A random bucket name.
 */
std::string random_bucket_name(const std::string& prefix);

/**
 * Helper method that removes a directory.
 *
 * @param path The name of the directory to be removed.
 * @param ctx The TileDB context.
 * @param vfs The VFS object that will remove the directory.
 */
void remove_dir(const std::string& path, tiledb_ctx_t* ctx, tiledb_vfs_t* vfs);

/**
 * Helper method that removes an S3 bucket.
 *
 * @param bucket_name The name of the bucket to be removed.
 * @param s3_supported The bucket is removed only when this is `true`.
 * @param ctx The TileDB context.
 * @param vfs The VFS object that will remove the bucket.
 */
void remove_s3_bucket(
    const std::string& bucket_name,
    bool s3_supported,
    tiledb_ctx_t* ctx,
    tiledb_vfs_t* vfs);

/**
 * Helper method to configure a single-stage filter list with the given
 * compressor and add it to the given attribute.
 *
 * @param ctx TileDB context
 * @param attr Attribute to set filter list on
 * @param compressor Compressor type to use
 * @param level Compression level to use
 */
int set_attribute_compression_filter(
    tiledb_ctx_t* ctx,
    tiledb_attribute_t* attr,
    tiledb_filter_type_t compressor,
    int32_t level);

/**
 * Performs a single write to an array.
 *
 * @param ctx The TileDB context.
 * @param array_name The array name.
 * @param layout The layout to write into.
 * @param attr_buffers The attribute buffers to be written.
 */
void write_array(
    tiledb_ctx_t* ctx,
    const std::string& array_name,
    tiledb_layout_t layout,
    const AttrBuffers& attr_buffers);

/**
 * Performs a single read to an array.
 *
 * @tparam T The array domain type.
 * @param ctx The TileDB context.
 * @param array The input array.
 * @param The subarray ranges.
 * @param layout The query layout.
 * @param attr_buffers The attribute buffers to be read.
 */
template <class T>
void read_array(
    tiledb_ctx_t* ctx,
    tiledb_array_t* array,
    const SubarrayRanges<T>& ranges,
    tiledb_layout_t layout,
    const AttrBuffers& attr_buffers);

#endif
