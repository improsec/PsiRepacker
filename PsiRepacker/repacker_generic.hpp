#pragma once

#include "repacker_base.hpp"

#include <array>

namespace psi {

class repacker_generic : public repacker_base<std::unique_ptr<char const[]>>
{
public:
	bool load(std::string const& filename, std::string const& pattern) override;
	bool save(std::string const& filename) override;
};

} // namespace psi