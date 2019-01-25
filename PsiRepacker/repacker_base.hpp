#pragma once

#include <string>
#include <vector>

namespace psi {

template <typename T>
class repacker_base
{
protected:
	using vector_t = std::vector<T>;

	static constexpr uint32_t cache_size = 16 * 1024;

public:
	virtual ~repacker_base() = default;

	virtual bool load(std::string const& filename, std::string const& pattern) = 0;
	virtual bool save(std::string const& filename) = 0;

	void sort()
	{
		std::sort(entries_.begin(), entries_.end());
	}

protected:
	template <class Iterator, class Container>
	inline decltype(auto) insert(Iterator begin, Iterator end, Container const& suffix, T const& value);

protected:
	vector_t entries_;
};

template <typename T>
template <class Iterator, class Container>
decltype(auto) repacker_base<T>::insert(Iterator begin, Iterator end, Container const& suffix, T const& value)
{
	auto entry = entries_.insert(entries_.end(), value);

	std::copy(begin, end, entry->begin());
	std::copy(suffix.begin(), suffix.end(), entry->begin() + std::distance(begin, end));

	return entry;
}

} // namespace psi