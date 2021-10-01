#include <iostream>
#include <string>
#include <cmath>
#include "passwd-utils.hpp"

void reduce_hash(std::string, int, int);

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

	rainbow::mass_generate(c, mc, MC, argv[4], argv[5]);
}


void reduce_hash(std::string hash, int time_reduced, int passwd_length){
	std::string new_password;
	unsigned long long int reduction;
	double number_of_passwd = pow(56.0, static_cast<double>(passwd_length));

	hash.resize(16); // keep 16 first char of the string

	reduction = std::strtoull(hash.c_str(), nullptr, 16);
	reduction += time_reduced;
	reduction = reduction % static_cast<unsigned long long int>(number_of_passwd);

	new_password = padding_reduction(reduction, passwd_length);


	//return reduction;
}

inline std::string padding_reduction(unsigned long long int reduction, int passwd_length) {
	/*transform reduction into a string with a size of twice de password*/
	std::string new_password = std::to_string(reduction);
	std::string padding_chars= "0000000000000000";
	int padding = reduction % (2 * passwd_length); // number of 0 to add to our password
	new_password.insert(0, padding_chars, 0, padding);

	return new_password;
}