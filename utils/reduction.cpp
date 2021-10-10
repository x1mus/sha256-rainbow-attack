#include <iostream>
#include <string>
#include <cmath>
#include "reduction.hpp"

std::string reduce_hash(std::string hash, int time_reduced, unsigned passwd_length){
	std::string new_password = "";
	unsigned long long int reduction;
	unsigned long long number_of_passwd = std::lround(pow(62.0, static_cast<double>(passwd_length)));

	hash.resize(16); // keep 16 first char of the string

	// Conversion from hex to int
	reduction = std::strtoull(hash.c_str(), nullptr, 16);
	
	// Doing formula
	reduction += time_reduced;
	reduction = reduction % number_of_passwd;
	
	// From int to ascii
	new_password.append(std::to_string(reduction));

	if (new_password.length() > 2 * passwd_length) {
		new_password.substr(0, 2 * passwd_length);
	} else {
		padding_reduction(new_password, passwd_length);
	}

	new_password = reduced_hash_to_new_password(new_password);
	
	return new_password;
}

inline void padding_reduction(std::string& new_password, unsigned passwd_length) {
	// Transform reduction into a string with a size of twice de password
	std::string padding_chars = "";
	while (new_password.size() < passwd_length*2) {
    	new_password += new_password;
	}

	padding_chars = new_password.substr(new_password.size() - passwd_length*2);
	new_password = padding_chars;
}

inline std::string reduced_hash_to_new_password(std::string& new_password_as_digits){
	std::string new_password;
	int pair_of_digits;
	const char allowed_chars[] = "abcdefghijklmnopqrstuvwxyz"
								 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								 "0123456789"; // size 62

	for(unsigned pos = 0; pos < new_password_as_digits.size(); pos+=2) {
		pair_of_digits = std::stoi(new_password_as_digits.substr(pos, 2));
		pair_of_digits %= 62;
		new_password.push_back(allowed_chars[pair_of_digits]);
	}

	return new_password;
}