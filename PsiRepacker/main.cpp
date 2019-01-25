#include "repacker_troy.hpp"

#include <chrono>
#include <functional>
#include <iostream>

template <typename Functor, typename... Args>
void timed_event(std::string const& description, Functor functor, Args&&... args)
{
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "Started " << description << std::endl;

	functor(std::forward<Args>(args)...);

	auto end = std::chrono::high_resolution_clock::now();
	auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

	std::cout << "Finished " << description << " in " << time_span.count() << " seconds" << std::endl;
	std::cout << std::endl;
}

int main(int argc, char const* argv[])
{
	if (argc != 3)
	{
		std::cout << "Invalid parameter(s). Usage is: PsiRepacker <input path> <output path>" << std::endl;
		return 1;
	}
	else
	{
		psi::repacker_troy repack;

		std::string filename = argv[1];
		std::string repacked = argv[2];

		timed_event("loading " + filename, std::bind(&psi::repacker_troy::load, &repack, std::placeholders::_1, std::placeholders::_2), filename, "([A-Za-z0-9]+):[0-9]+");
		timed_event("sorting " + filename, std::bind(&psi::repacker_troy::sort, &repack));
		timed_event("saveing " + repacked, std::bind(&psi::repacker_troy::save, &repack, std::placeholders::_1), repacked);

		return 0;
	}
}
