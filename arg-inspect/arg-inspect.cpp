//
// arg-inspect
// (c) 2015 Andrzej Budzanowski
//

#include <iostream>

int main(int argc, const char * argv[])
{
	if (argc == 1)
	{
		std::cout << "arg-inspect\n"
					 "(c) 2015 Andrzej Budzanowski\n"
					 "--\n"
					 "No argument parameters to list" << std::endl;

		return 1;
	}

	for (int it = 1; it < argc; ++it)
		std::cout << it << ": " << argv[it] << std::endl;

	return 0;
}
