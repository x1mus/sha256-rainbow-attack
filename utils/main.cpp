// Little disclaimer on how to compile
// g++ -std=c++17 main.cpp -o main

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include "sha256.h"
#include "reduction.hpp"
#include "argparse.hpp"
#include "passwd-utils.hpp"

void find_corresponding_password(std::string&, std::string&, unsigned, unsigned);
std::string find_head(std::string&, std::ifstream&, unsigned, unsigned, unsigned);

int main(int argc, char *argv[]) {
	argparse::ArgumentParser program("rainbow_table");

	// Defining modes of working
	program.add_argument("--gen").help("Generate the rainbow table").default_value(false).implicit_value(true);
	program.add_argument("--atk").help("Launch an attack on a specified hash").default_value(false).implicit_value(true);

	// "GEN" mode arguments
	program.add_argument("-l", "--length_chains").help("Specify the length of the chains to generate (int)").scan<'i', unsigned>();
	
	// "ATK" mode arguments
	program.add_argument("-s", "--sha256").help("Specify the hash to crack (str)");
	program.add_argument("-S", "--sha256_file").help("Specify a file containing hashs to crack");

	// BOTH mode arguments
	program.add_argument("-n", "--nb_chains").help("Specify the number of chains to generate (int)").scan<'i', unsigned>();
	program.add_argument("-r", "--rainbow_table").help("Specify the filename where you want the rainbow table to be generated (CSV)");
	program.add_argument("-p", "--password_length").help("Specify password size").scan<'i', unsigned>();

	// We try to parse all arguments, if an error occurs, we just display the help
	try {
		program.parse_args(argc, argv);
	} catch (const std::runtime_error& err) {
		std::cout << err.what() << std::endl;
		std::cout << program;
		exit(0);
	}

	// Figuring out which mode the user is using
	if (program["--gen"] == true && program["--atk"] == true) {
		std::cout << "--gen and --atk can't be present at the same time" << std::endl;
		std::cout << program;
	} else if (program["--gen"] == true) {
		
		// MODE == GEN --> We test the presence of all arguments
		if (program.is_used("--nb_chains") == true && 
			program.is_used("--length_chains") == true &&
			program.is_used("--rainbow_table") == true &&
			program.is_used("--password_length") == true) {
			
			unsigned nb_chains = program.get<unsigned>("--nb_chains");
			unsigned length_chains = program.get<unsigned>("--length_chains");
			unsigned password_length = program.get<unsigned>("--password_length");
			auto rb_file = program.get<std::string>("--rainbow_table");
			SHA256 sha256;
			std::string password;
			std::string reduc;
			std::string hash;
			rainbow::mass_generate(nb_chains, password_length, password_length, "password.txt");
			std::ifstream passwd_table("password.txt");
			std::ofstream RainbowTable(rb_file);
			while (getline(passwd_table, password)){
				std::cout << "Password : " << password;
				password_length = strlen(password.c_str());
				RainbowTable << password.c_str();
				RainbowTable << ";";
				reduc = password;
				for(unsigned nbr_hash_red = 0; nbr_hash_red < length_chains; ++nbr_hash_red){
					hash = sha256(reduc);
					reduc = reduce_hash(hash, nbr_hash_red, password_length);
					std::cout << " -> " << hash.substr(0, 15) << " -> " << reduc;

				}
				std::cout << std::endl;
				RainbowTable << reduc << std::endl;
			}

			passwd_table.close();
			RainbowTable.close();

		} else {
			std::cout << "Please specify all the required arguments : See README" << std::endl;
			std::cout << program;
		}

	} else if (program["--atk"] == true) {
		
		// MODE == ATK
		if (program.is_used("--sha256") == true && 
			program.is_used("--rainbow_table") == true &&
			program.is_used("--length_chains") == true &&
			program.is_used("--password_length") == true) {
			
			auto hash = program.get<std::string>("--sha256");
			auto rb_file = program.get<std::string>("--rainbow_table");
			unsigned length_chains = program.get<unsigned>("--length_chains");
			unsigned password_length = program.get<unsigned>("--password_length");

			std::ifstream RainbowTable(rb_file);

			if (hash.length() == 64) {
				int i = length_chains;
				std::string head = "";
				std::cout << "++++++++++++++++++++++++" << std::endl;
				std::cout << "Trying to find head ... " << std::endl;
				std::cout << "++++++++++++++++++++++++" << std::endl;
				while (i>=0 && head.empty()){

					std::cout << "===============" << std::endl;
					std::cout << "NEW WHILE ENTRY" << std::endl;
					std::cout << "===============" << std::endl;
					std::cout << "MAIN i : " << i << std::endl;
					std::cout << "---------" << std::endl;
					head = find_head(hash, RainbowTable, length_chains, i, password_length);
					--i;
					std::cout << "------------" << std::endl;
					std::cout << "MAIN HEAD : " << head << std::endl;
					std::cout << "MAIN HASH : " << hash << std::endl;
					std::cout << std::endl;
				}

				std::cout << std::endl;
				std::cout << "++++++++++++++++++++++++++++++++++++++++++" << std::endl;
				std::cout << "Trying to find corresponding password ... " << std::endl;
				std::cout << "++++++++++++++++++++++++++++++++++++++++++" << std::endl;
				if (head.empty()){
					std::cout << "password not found :/" << std::endl;
				} else {
					find_corresponding_password(head, hash,0, password_length);
				}
				
				
				

			} else {
				std::cout << "Hash value is incorrect. Only SHA256 is available" << std::endl;
				std::cout << program;
			}

		} else if (program.is_used("--sha256_file") == true && program.is_used("--rainbow_table") == true) {
			auto sha256_file = program.get<std::string>("--sha256_file");
			std::ifstream to_crack(sha256_file);

			// Foreach hash in sha256_file
			//if (hash.length() == 64) {
				//Do attack

			//} else {
			//	std::cout << "Hash value is incorrect. Only SHA256 is available" << std::endl;
			//	std::cout << program;
			//}

			to_crack.close();
		
		} else {
			std::cout << "Please specify all the required arguments : See README" << std::endl;
			std::cout << program;
		}

	} else {
		std::cout << "Please specify a mode of working [--gen/--atk]" << std::endl;
		std::cout << program;
	}
	
	return 0;
}


std::string find_head(std::string& hash, std::ifstream& rainbow_table, unsigned length_chains, unsigned i, unsigned password_length) {
	std::cout << "ARGUMENTS" << std::endl;
	std::cout << "........." << std::endl;
	std::cout << "hash : " << hash << std::endl;
	std::cout << "i : " << i << std::endl;
	std::cout << "password length : " << password_length << std::endl;
	std::cout << "length_chains : " << length_chains << std::endl;
	SHA256 sha256;
	std::string reduction = reduce_hash(hash, i, password_length);
	std::cout << "reduction : " << reduction << std::endl;
	std::string rainbow_table_line;
	std::string tail_of_rainbow_table;
	const char delimiter = ';';

	if (length_chains == i){
		std::cout << "length = 0" << std::endl;
		return "";
	}

	i++;
	while(getline(rainbow_table, rainbow_table_line)){
		tail_of_rainbow_table = rainbow_table_line.substr(rainbow_table_line.find(delimiter)+1);
		std::cout << "tail : " << tail_of_rainbow_table << std::endl;
		
		if(reduction.compare(tail_of_rainbow_table) == 0){
			std::cout << "success" << std::endl;
			return rainbow_table_line.substr(0, rainbow_table_line.find(delimiter));
		}
	}
	rainbow_table.clear();
	rainbow_table.seekg(0);
	
	reduction = sha256(reduction);

	std::cout << std::endl;
	return find_head(reduction, rainbow_table, length_chains, i, password_length);
}

void find_corresponding_password(std::string& reduction, std::string& hash, unsigned i, unsigned password_length) {
	SHA256 sha256;
	std::string hashed = sha256(reduction);
	if(hashed == hash) {
		std::cout << "The corresponding password for " << hash << "is: " << reduction << std::endl;
	} else {
		hashed = reduce_hash(hashed, i, password_length);
		find_corresponding_password(hashed, hash, ++i, password_length);
	}
}