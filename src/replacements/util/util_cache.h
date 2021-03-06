/*
 * Copyright 2011, Blender Foundation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __UTIL_CACHE_H__
#define __UTIL_CACHE_H__

/* Disk Cache based on Hashing
 *
 * To be used to cache expensive computations. The hash key is created from an
 * arbitrary number of bytes, by hashing the bytes using MD5, which then gives
 * the file name containing the data. This data then is read from the file
 * again into the appropriate data structures.
 *
 * This way we do not need to accurately track changes, compare dates and
 * invalidate cache entries, at the cost of exta computation. If everything
 * is stored in a global cache, computations can perhaps even be shared between
 * different scenes where it may be hard to detect duplicate work.
 */

#include "util_set.h"
#include "util_string.h"
#include "util_vector.h"

#include <stdio.h>

CCL_NAMESPACE_BEGIN


class CacheBuffer {
public:
	const void *data;
	size_t size;

	CacheBuffer(const void *data_, size_t size_)
	{ data = data_; size = size_; }
};

class CacheData {
public:
	vector<CacheBuffer> buffers;
	string name;
	string filename;
	bool have_filename;
	FILE *f;

	CacheData(const string& name = "") {}
	~CacheData() {}

	const string& get_filename() { printf("uhoh, cache\n"); return name; }

	template<typename T> void add(const vector<T>& data)
	{
		CacheBuffer buffer(data.size()? &data[0]: NULL, data.size()*sizeof(T));
		buffers.push_back(buffer);
	}

	template<typename T> void add(const array<T>& data)
	{
		CacheBuffer buffer(data.size()? &data[0]: NULL, data.size()*sizeof(T));
		buffers.push_back(buffer);
	}

	void add(const void *data, size_t size)
	{
		if(size) {
			CacheBuffer buffer(data, size);
			buffers.push_back(buffer);
		}
	}

	void add(const int& data)
	{
		CacheBuffer buffer(&data, sizeof(int));
		buffers.push_back(buffer);
	}

	void add(const float& data)
	{
		CacheBuffer buffer(&data, sizeof(float));
		buffers.push_back(buffer);
	}

	void add(size_t& data)
	{
		CacheBuffer buffer(&data, sizeof(size_t));
		buffers.push_back(buffer);
	}

	template<typename T> void read(array<T>& data)
	{
		size_t size;

		if(!fread(&size, sizeof(size), 1, f)) {
			fprintf(stderr, "Failed to read vector size from cache.\n");
			return;
		}

		if(!size)
			return;

		data.resize(size/sizeof(T));

		if(!fread(&data[0], size, 1, f)) {
			fprintf(stderr, "Failed to read vector data from cache (%lu).\n", (unsigned long)size);
			return;
		}
	}

	void read(int& data)
	{
		size_t size;

		if(!fread(&size, sizeof(size), 1, f))
			fprintf(stderr, "Failed to read int size from cache.\n");
		if(!fread(&data, sizeof(data), 1, f))
			fprintf(stderr, "Failed to read int from cache.\n");
	}

	void read(float& data)
	{
		size_t size;

		if(!fread(&size, sizeof(size), 1, f))
			fprintf(stderr, "Failed to read float size from cache.\n");
		if(!fread(&data, sizeof(data), 1, f))
			fprintf(stderr, "Failed to read float from cache.\n");
	}

	void read(size_t& data)
	{
		size_t size;

		if(!fread(&size, sizeof(size), 1, f))
			fprintf(stderr, "Failed to read size_t size from cache.\n");
		if(!fread(&data, sizeof(data), 1, f))
			fprintf(stderr, "Failed to read size_t from cache.\n");
	}
};

class Cache {
public:
  static Cache global;

	void insert(CacheData& key, CacheData& value) {}
	bool lookup(CacheData& key, CacheData& value) { return false; }

	void clear_except(const string& name, const set<string>& except) {}

protected:
	string data_filename(CacheData& key) { return ""; }
};

CCL_NAMESPACE_END

#endif /* __UTIL_CACHE_H__ */

