#pragma once

#include "repacker_base.hpp"

#include <array>

namespace psi {

class repacker_troy : public repacker_base<std::array<uint8_t, 16>>
{
public:
	repacker_troy();

	bool load(std::string const& filename, std::string const& pattern) override;
	bool save(std::string const& filename) override;
};

} // namespace psi