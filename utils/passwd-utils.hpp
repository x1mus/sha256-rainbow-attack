#include <string>
#include <fstream>

#include "random.hpp"
#include "sha256.h"

namespace rainbow {

std::string generate_passwd(int length)
{
	static const std::string char_policy = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN1234567890";
	static const int c_len = char_policy.length();

	char str[length + 1];
	for(int i = 0; i < length; i++)
		str[i] = char_policy[rainbow::random(0, c_len - 1)];
	str[length] = '\0';

	return std::string(str);
}

void mass_generate(int n, int mc, int MC, const std::string& of_pwd)
{
	SHA256 sha256;
	std::ofstream passwd_file;
	passwd_file.open(of_pwd);


	if(passwd_file.is_open())
	{		
		for(int i = 0; i < n; i++)
		{
			std::string pass = generate_passwd(rainbow::random(mc, MC));
			passwd_file << pass << std::endl;

		}

		passwd_file.close();
	}
	else
		throw std::runtime_error("Output files could not be opened");
}

inline bool check_pwd(const std::string& pwd, const std::string& hash)
{
	SHA256 sha256;
	return sha256(pwd) == hash;
}

double mass_check(const std::string& if_pwd, const std::string& if_hash)
{
	std::ifstream passwd_file;
	passwd_file.open(if_pwd);

	std::ifstream hash_file;
	hash_file.open(if_hash);

	if(passwd_file.is_open() && hash_file.is_open())
	{		
		std::string pass;
		std::string hash;

		int count = 0;
		int success = 0;
		while(std::getline(passwd_file, pass) && std::getline(hash_file, hash))
		{
			count++;
			if(check_pwd(pass, hash))
				success++;
		}

		passwd_file.close();
		hash_file.close();

		return (static_cast<double>(success) / count) * 100;
	}
	else
		throw std::runtime_error("Input files could not be opened");
}

}//rainbow namespace
