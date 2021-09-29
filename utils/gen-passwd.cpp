#include <iostream>
#include "passwd-utils.hpp"

int main(int argc, char *argv[])
{
	if(argc != 6)
	{
		std::cerr << "Usage : \"gen-passwd c mc MC of_pwd of_hash\", where" << std::endl
			<< "- c is the number of passwords to generate," << std::endl
			<< "- mc is the minimum number of chars allowed in an alphanumeric password," << std::endl
			<< "- MC is the maximum number of chars allowed in an alphanumeric password," << std::endl
			<< "- of_pwd is the name of the output file where the passwords will be stored" << std::endl
			<< "- of_hash is the name of the output file where the sha-256 hashes of the passwords will be stored" << std::endl;
		return 1;
	}
	
	int c = std::stoi(argv[1]);
	int mc = std::stoi(argv[2]);
	int MC = std::stoi(argv[3]);

	rainbow::mass_generate(c, mc, MC, argv[4], argv[5]);
}
