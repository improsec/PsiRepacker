#include "repacker_generic.hpp"

#include <Windows.h>

#include <iostream>
#include <regex>

namespace std {
namespace detail {

int compare_char_strings(char const* left, char const* right)
{
	size_t const left_size = strlen(left);
	size_t const right_size = strlen(right);

	int const result = memcmp(left, right, min<size_t>(left_size, right_size));

	if (result != 0)
		return result;
	else if (left_size < right_size)
		return (-1);
	else if (left_size > right_size)
		return 1;
	else
		return 0;
}

} // namespace detail

bool operator<(unique_ptr<char const[]> const& left, unique_ptr<char const[]> const& right)
{
	return (detail::compare_char_strings(left.get(), right.get()) < 0);
}

} // namespace std

namespace psi {

bool repacker_generic::load(std::string const& filename, std::string const& pattern)
{
	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		throw std::runtime_error("failed to open file");
	else
	{
		std::string buffer;
		std::pair<std::array<char, cache_size>, DWORD> temp;

		while (ReadFile(hFile, &temp.first[0], static_cast<DWORD>(temp.first.size()), &temp.second, NULL) && temp.second != 0)
		{
			buffer.insert(buffer.end(), temp.first.begin(), temp.first.begin() + temp.second);

			for (std::size_t offset = 0; (offset = buffer.find("\r\n")) != std::string::npos; buffer.erase(0, offset + sizeof('\r') + sizeof('\n')))
			{
				std::regex const entry_regex(pattern);
				std::smatch entry_match;

				if (std::regex_match(buffer.cbegin(), buffer.cbegin() + offset, entry_match, entry_regex) && entry_match.size() == 2)
				{
					/*
						entry_match[0] = whole match string
						entry_match[1] = highlighted match for hash/password (or other type of key value)
					*/

					std::string const& key = entry_match[1];

					auto entry = std::make_unique<char const[]>(key.size() + sizeof('\0'));
					memcpy(const_cast<char*>(entry.get()), key.data(), key.size() + sizeof('\0'));
				}
				else
				{
					std::cout << "Regex pattern match failed for entry " << buffer.substr(0, offset) << std::endl;
				}
			}
		}

		CloseHandle(hFile);
	}
}

bool repacker_generic::save(std::string const& filename)
{
	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		throw std::runtime_error("failed to create file");
	else
	{
		std::pair<std::string, DWORD> temp = std::make_pair(std::to_string(entries_.size()), 0);

		if (WriteFile(hFile, temp.first.c_str(), static_cast<DWORD>(temp.first.size()), &temp.second, NULL) &&
			WriteFile(hFile, "\r\n", sizeof('\r') + sizeof('\n'), &temp.second, NULL))
		{
			for (auto const& entry : entries_)
			{
				if (!WriteFile(hFile, entry.get(), static_cast<DWORD>(strlen(entry.get())), &temp.second, NULL) || temp.second == 0)
					break;

				if (!WriteFile(hFile, "\r\n", sizeof('\r') + sizeof('\n'), &temp.second, NULL) || temp.second == 0)
					break;
			}
		}

		CloseHandle(hFile);
	}
}

} // namespace psi