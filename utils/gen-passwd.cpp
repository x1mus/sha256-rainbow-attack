#include <iostream>
#include <string>
#include <cmath>
#include "passwd-utils.hpp"
#include "reduction.hpp"

int main(int argc, char *argv[])
{
	std::string hash ("0f78fcc486f5315418fbf095e71c0675ee07d318e5ac4d150050cd8e57966496");
	hash.resize(16);
	std::cout << hash << std::endl;
	reduce_hash(hash, 1, 8);

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

	rainbow::mass_generate(c, mc, MC, argv[4]);
}