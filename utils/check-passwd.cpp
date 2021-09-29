#include <iostream>
#include <iomanip>
#include "passwd-utils.hpp"

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		std::cerr << "Usage : \"gen-passwd if_pwd if_hash\", where" << std::endl
			<< "- if_pwd is the name of the input file where the passwords are stored" << std::endl
			<< "- if_hash is the name of the input file where the sha-256 hashes of the passwords are stored" << std::endl;
		return 1;
	}		

	double success = rainbow::mass_check(argv[1], argv[2]);
	std::cout << std::setprecision(4) << success << "% success" << std::endl;
}
