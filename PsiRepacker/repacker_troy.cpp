#include "repacker_troy.hpp"
#include "utility.hpp"

#include <Windows.h>
#include <iostream>

namespace psi {

repacker_troy::repacker_troy()
{
	//entries_.reserve(517238892);	// Troy Hunt v3
	entries_.reserve(551509767);	// Troy Hunt v4
}

bool repacker_troy::load(std::string const& filename, std::string const& pattern)
{
	static constexpr uint32_t hash_size = 32;

	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to open file " << filename << std::endl;
		return false;
	}
	else
	{
		std::vector<char> buffer;
		std::vector<char> clrf = { '\r', '\n' };

		std::pair<std::array<char, cache_size>, DWORD> temp;

		while (ReadFile(hFile, &temp.first[0], static_cast<DWORD>(temp.first.size()), &temp.second, NULL))
		{
			if (temp.second != 0)
			{
				buffer.insert(buffer.end(), temp.first.begin(), temp.first.begin() + temp.second);

				for (decltype(buffer)::const_iterator iterator; (iterator = std::search(buffer.begin(), buffer.end(), clrf.begin(), clrf.end())) != buffer.end();
					buffer.erase(buffer.begin(), std::next(iterator, clrf.size())))
				{
					entries_.push_back(std::array<uint8_t, 16>());
					utility::hex_to_array(entries_.back(), buffer.begin(), buffer.begin() + hash_size);
				}
			}
			else
			{
				entries_.push_back(std::array<uint8_t, 16>());
				utility::hex_to_array(entries_.back(), buffer.begin(), buffer.begin() + hash_size);

				break;
			}
		}

		CloseHandle(hFile);
		return true;
	}
}

bool repacker_troy::save(std::string const& filename)
{
	static constexpr uint64_t cache_size = 16 * 1024;

	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to create file " << filename << std::endl;
		return false;
	}
	else if (!entries_.empty())
	{
		DWORD written = 0;
		uint64_t elements = entries_.size();

		if (WriteFile(hFile, &elements, sizeof(uint64_t), &written, NULL) && written == sizeof(uint64_t))
		{
			vector_t::size_type offset = 0;
			vector_t::size_type length = static_cast<vector_t::size_type>(elements) * std::tuple_size<vector_t::value_type>::value;

			while (WriteFile(hFile, reinterpret_cast<uint8_t*>(entries_.data()) + offset, (length - offset) >= cache_size ? 
				static_cast<DWORD>(cache_size) : static_cast<DWORD>(length - offset), &written, NULL))
			{
				if (written != 0)
					offset += written;
				else
					break;
			}
		}
	}

	CloseHandle(hFile);
	return true;
}

} // namespace psi