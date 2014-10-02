//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_CONTENT_BINARYREADER_80B251B8_159B_4CE5_97BC_ED14AE71F9B0_HPP
#define POMDOG_SRC_CONTENT_BINARYREADER_80B251B8_159B_4CE5_97BC_ED14AE71F9B0_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Utility/Assert.hpp>
#include <cstddef>
#include <type_traits>
#include <array>
#include <vector>
#include <utility>

namespace Pomdog {
namespace Details {

namespace BinaryReader {

template <class Stream>
std::size_t GetBinarySize(Stream & stream)
{
	stream.seekg(0, stream.end);
	auto const length = stream.tellg();
	stream.seekg(0, stream.beg);
	return length;
}

template <typename T, class Stream>
std::vector<T> ReadArray(Stream & stream, std::size_t elementCount)
{
	static_assert(std::is_pod<T>::value, "You can only use POD types.");
	
	std::vector<T> result(elementCount);
	stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
	return std::move(result);
}

template <typename T, std::size_t ElementCount, class Stream>
std::array<T, ElementCount> ReadArray(Stream & stream)
{
	static_assert(std::is_pod<T>::value, "You can only use POD types.");
	
	std::array<T, ElementCount> result;
	stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
	return std::move(result);
}

template <typename T, class Stream>
T Read(Stream & stream)
{
	static_assert(std::is_pod<T>::value, "You can only use POD types.");
	return std::move(ReadArray<T, 1>(stream)[0]);
}

template <typename T>
bool CanRead(std::size_t byteLength)
{
	return sizeof(T) <= byteLength;
}

template <typename T>
T Read(std::uint8_t const* data)
{
	POMDOG_ASSERT(data);
	static_assert(std::is_pod<T>::value, "You can only use POD types.");
	T value;
	std::memcpy(&value, data, sizeof(value));
	return std::move(value);
}

}// namespace BinaryReader

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_CONTENT_BINARYREADER_80B251B8_159B_4CE5_97BC_ED14AE71F9B0_HPP)
