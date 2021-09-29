// //////////////////////////////////////////////////////////
// sha256.h
// Copyright (c) 2014,2015 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//

#pragma once

#include <array>
#include <cstdint>
#include <string>

class SHA256
{
public:
	/// split into 64 byte blocks (=> 512 bits), hash is 32 bytes long
	static constexpr uint32_t BlockSize = 512 / 8;
	static constexpr uint32_t HashBytes = 32;

	/// same as reset()
	SHA256();

	/// compute SHA256 of a memory block
	std::string operator()(const void* data, size_t numBytes);
	
	/// compute SHA256 of a string, excluding final zero
	std::string operator()(const std::string& text);

	template <size_t N>
	inline void add(const std::array<uint8_t, N>& arr) {
		add(arr.data(), arr.size());
	}
	
	/// add arbitrary number of bytes
	void add(const void* data, size_t numBytes);

	/// return latest hash as 64 hex characters
	std::string getHashString();

	/// return latest hash as bytes
	std::array<uint8_t, HashBytes> getHashBytes();

	/// restart
	void reset();

private:
	static constexpr uint32_t HashValues = HashBytes / 4;
	
	/// process 64 bytes
	void processBlock(const void* data);

	/// process everything left in the internal buffer
	void processBuffer();
	
	/// bytes not processed yet
	std::array<uint8_t, BlockSize> m_buffer;

	/// valid bytes in m_buffer
	size_t   m_bufferSize = 0;

	/// size of processed data in bytes
	uint64_t m_numBytes = 0;

	/// hash, stored as integers
	std::array<uint32_t, HashValues> m_hash;
};

std::string sha256(const std::string& s);