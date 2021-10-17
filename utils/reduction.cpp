#include <iostream>
#include <string>
#include <cmath>
#include "reduction.hpp"

const char ALLOWED_CHARS[] = "abcdefghijklmnopqrstuvwxyz"
							"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							"0123456789"; // size 62

std::string reduce_hash(std::string hash, int time_reduced, unsigned passwd_length){

	std::string new_password (passwd_length, ' ');

	unsigned long long int reduction;
	unsigned long long number_of_passwd = std::lround(pow(62.0, static_cast<double>(passwd_length))); // |P|

	hash.resize(16); // keep 16 first char of the string

	// Conversion from hex to int
	unsigned long long hash_to_int = std::strtoull(hash.c_str(), nullptr, 16);
	reduction = hash_to_int; 
	
	// Applying formula
	reduction = (reduction + time_reduced) % number_of_passwd; // (x+i) mod |P|
	
	// From int to string
	std::string output_as_str = convert_to_passwd(reduction); // map result to a string

	int str_size_diff = passwd_length - output_as_str.length();
	for (unsigned i=0; i < output_as_str.length(); i++ ) {
        new_password[i+str_size_diff] = output_as_str[i];
    }

	// padding using unmodified hash
	for (int i=0; i < str_size_diff; i++) {
        new_password[i] = ALLOWED_CHARS[hash_to_int % 62];
        hash_to_int /= 62;
    }
	
	return new_password;
}

inline std::string convert_to_passwd(unsigned long long digits_to_convert) {
	std::string result;

	do {
		result.insert(0, std::string(1, ALLOWED_CHARS[digits_to_convert % 62]));
		digits_to_convert /= 62;
	} while (digits_to_convert > 0);

	return result;
}
