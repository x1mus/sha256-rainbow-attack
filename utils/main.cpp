// Little disclaimer on how to compile
// g++ -std=c++17 main.cpp -o main

#include <iostream>
#include <string>
#include "argparse.hpp"

int main(int argc, char *argv[]) {
	argparse::ArgumentParser program("rainbow_table");

	// Defining modes of working
	program.add_argument("--gen").help("Generate the rainbow table").default_value(false).implicit_value(true);
	program.add_argument("--atk").help("Launch an attack on a specified hash").default_value(false).implicit_value(true);

	// "GEN" mode arguments
	program.add_argument("-n", "--nb_chains").help("Specify the number of chains to generate (int)").scan<'i', int>();
	program.add_argument("-l", "--length_chains").help("Specify the length of the chains to generate (int)").scan<'i', int>();
	program.add_argument("-p", "--psswd_length").help("Specify the length of the passwords to generate (int)").scan<'i', int>();

	// "ATK" mode arguments
	program.add_argument("-s", "--sha256").help("Specify the hash to crack (str)");

	// We try to parse all arguments, if an error occurs, we just display the help
	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
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
		if (program.is_used("--nb_chains") == true && program.is_used("--length_chains") == true && program.is_used("--psswd_length") == true) {
			int nb_chains = program.get<int>("--nb_chains");
			int length_chains = program.get<int>("--length_chains");
			int psswd_length = program.get<int>("--psswd_length");

			// CONNECT TO THE DB
			// GENERATE RAINBOW TABLE
			// STORE RAINBOW TABLE

		} else {
			std::cout << "Please specify all the required arguments : See README" << std::endl;
			std::cout << program;
		}

	} else if (program["--atk"] == true) {
		
		// MODE == ATK
		if (program.is_used("--sha256") == true) {
			auto hash = program.get<std::string>("--sha256");

			if (hash.length() == 64) {
				std::cout << "hash : " << hash << std::endl;
			} else {
				std::cout << "Hash value is incorrect. Only SHA256 is available" << std::endl;
				std::cout << program;
			}

		} else {
			std::cout << "Please specify all the required arguments : See README" << std::endl;
			std::cout << program;
		}

	} else {
		std::cout << program;
	}
	
	return 0;
}