#include <iostream>
#include <string>
#include <cmath>
#include "reduction.hpp"

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
	unsigned padding = (2 * passwd_length) - new_password.size(); // number of 0 to add to our password
	new_password.insert(0, padding_chars, 0, padding);

}

inline std::string reduced_hash_to_new_password(std::string& new_password_as_digits){
	std::string new_password;
	int pair_of_digits;
	const char allowed_chars[] = "abcdefghijklmnopqrstuvwxyz"
								 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								 "0123456789"; // size 62

	for(int pos = 0; pos < new_password_as_digits.size(); pos+=2) {
		pair_of_digits = std::stoi(new_password_as_digits.substr(pos, 2));
		pair_of_digits %= 62;
		new_password.push_back(allowed_chars[pair_of_digits]);
	}

	return new_password;
}