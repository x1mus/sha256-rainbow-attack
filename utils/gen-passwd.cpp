#include <iostream>
#include <string>
#include <cmath>
#include "passwd-utils.hpp"

void reduce_hash(std::string, int, int);
inline void padding_reduction(std::string&, int);
inline std::string reduced_hash_to_new_password(std::string&);

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
	std::string new_password = "";
	unsigned long long int reduction;
	double number_of_passwd = pow(56.0, static_cast<double>(passwd_length));

	hash.resize(16); // keep 16 first char of the string

	reduction = std::strtoull(hash.c_str(), nullptr, 16);
	reduction += time_reduced;
	reduction = reduction % static_cast<unsigned long long int>(number_of_passwd);
	new_password.append(std::to_string(reduction));

	padding_reduction(new_password, passwd_length);
	std::cout << new_password << std::endl;
	new_password = reduced_hash_to_new_password(new_password);
	std::cout << new_password << std::endl;
	

	//return reduction;
}

inline void padding_reduction(std::string& new_password, int passwd_length) {
	/*transform reduction into a string with a size of twice de password*/
	std::string padding_chars= "0000000000000000";
	int padding = std::strtoull(new_password.c_str(), nullptr, 10) % (2 * passwd_length); // number of 0 to add to our password
	std::cout << "padding: " << padding << std::endl;
	new_password.insert(0, padding_chars, 0, padding);

}

inline std::string reduced_hash_to_new_password(std::string& new_password_as_digits){
	std::cout << "in reduc_hash_to_n_passwd" << std::endl;
	std::string new_password;
	int pair_of_digits;
	const char allowed_chars[] = "abcdefghijklmnopqrstuvwxyz"
								 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								 "0123456789"; // size 62
	
	std::cout << "on est là fraté" << std::endl;
	for(int pos = 0; pos < new_password_as_digits.size(); pos+=2) {
		std::cout << "for ce en nous" << std::endl;
		pair_of_digits = std::stoi(new_password_as_digits.substr(pos, 2));
		pair_of_digits %= 62;
		new_password.push_back(allowed_chars[pair_of_digits]);
		std::cout << "fonctionne" << std::endl;
	}

	std::cout << "in reduc_hash_to_n_passwd" << std::endl;

	return new_password;
}