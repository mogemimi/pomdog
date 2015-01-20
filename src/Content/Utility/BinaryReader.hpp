//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BINARYREADER_80B251B8_159B_4CE5_97BC_ED14AE71F9B0_HPP
#define POMDOG_BINARYREADER_80B251B8_159B_4CE5_97BC_ED14AE71F9B0_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <type_traits>
#include <array>
#include <vector>
#include <utility>

namespace Pomdog {
namespace Details {

struct POMDOG_EXPORT BinaryReader {
	template <class Stream>
	static std::size_t GetBinarySize(Stream & stream)
	{
		///@todo This code is NOT secure.
		stream.seekg(0, stream.end);
		auto const byteLength = static_cast<std::size_t>(stream.tellg());
		stream.seekg(0, stream.beg);
		return byteLength;
	}

	template <typename T, class Stream>
	static std::vector<T> ReadArray(Stream & stream, std::size_t elementCount)
	{
		static_assert(std::is_pod<T>::value, "You can only use POD types.");

		POMDOG_ASSERT(elementCount > 0);

		std::vector<T> result(elementCount);
		stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
		return std::move(result);
	}

	template <typename T, std::size_t ElementCount, class Stream>
	static std::array<T, ElementCount> ReadArray(Stream & stream)
	{
		static_assert(std::is_pod<T>::value, "You can only use POD types.");

		std::array<T, ElementCount> result;
		stream.read(reinterpret_cast<char*>(result.data()), sizeof(T) * result.size());
		return std::move(result);
	}

	template <typename T, class Stream>
	static T Read(Stream & stream)
	{
		static_assert(std::is_pod<T>::value, "You can only use POD types.");
		return std::move(ReadArray<T, 1>(stream)[0]);
	}

	template <typename T>
	static bool CanRead(std::size_t byteLength)
	{
		return sizeof(T) <= byteLength;
	}

	template <typename T>
	static T Read(std::uint8_t const* data)
	{
		POMDOG_ASSERT(data);
		static_assert(std::is_pod<T>::value, "You can only use POD types.");
		T value;
		std::memcpy(&value, data, sizeof(value));
		return std::move(value);
	}
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_BINARYREADER_80B251B8_159B_4CE5_97BC_ED14AE71F9B0_HPP)
