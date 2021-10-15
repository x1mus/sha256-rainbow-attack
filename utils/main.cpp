#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include "sha256.h"
#include "reduction.hpp"
#include "argparse.hpp"
#include "passwd-utils.hpp"
#include <thread>
#include <mutex>

#ifdef DEBUG
# define D(x) x
#else
# define D(x)
#endif

bool is_number(const std::string&);
void display_help(std::string&);
void attack(std::string, std::string, unsigned, unsigned, unsigned, std::ofstream&);
void find_head(std::string&, std::string, std::ifstream&, unsigned, unsigned, unsigned);
std::string find_corresponding_password(std::string&, std::string&, unsigned);

std::mutex write_result_mutex;

int main(int argc, char *argv[]) {
	/*
	*	=====================
	*	PARSING ALL ARGUMENTS	
	*	=====================
	*/
	argparse::ArgumentParser program("main");

	// Defining modes of working
	program.add_argument("--gen").help("Generate the rainbow table").default_value(false).implicit_value(true);
	program.add_argument("--atk").help("Launch an attack on a specified hash").default_value(false).implicit_value(true);

	// "GEN" mode arguments
	program.add_argument("-n", "--nb_chains").help("Specify the number of chains to generate (int)").scan<'i', unsigned>();
	
	// "ATK" mode arguments
	program.add_argument("-s", "--sha256").help("Specify the hash to crack (str)");
	program.add_argument("-S", "--sha256_file").help("Specify a file containing hashs to crack");

	// BOTH mode arguments
	program.add_argument("-l", "--length_chains").help("Specify the length of the chains to generate (int)").scan<'i', unsigned>();
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

	std::string program_help = program.help().str();

	if (program["--gen"] == true && program["--atk"] == true) {
		display_help(program_help);
	

	/*
	*	===============
	*	GENERATION MODE
	*	===============
	*/
	} else if (program["--gen"] == true) {
		if (program.is_used("--nb_chains") == true && 
			program.is_used("--length_chains") == true &&
			program.is_used("--rainbow_table") == true &&
			program.is_used("--password_length") == true) {
			
			// Params parsing
			unsigned nb_chains = program.get<unsigned>("--nb_chains");
			unsigned length_chains = program.get<unsigned>("--length_chains");
			unsigned password_length = program.get<unsigned>("--password_length");
			auto rb_file = program.get<std::string>("--rainbow_table");
			
			// Default variables
			SHA256 sha256;
			std::string password;
			std::string reduc;
			std::string hash;
			
			// Input and output stream + generations of passwords
			rainbow::mass_generate(nb_chains, password_length, password_length, "password.txt");
			std::ifstream passwd_table("password.txt");
			std::ofstream RainbowTable(rb_file);
			
			// For each password we do all the required operations to generate
			while (getline(passwd_table, password)){
#ifdef DEBUG
				std::cout << "Password : " << password;
#endif
				password_length = strlen(password.c_str());
				RainbowTable << password.c_str();
				RainbowTable << ";";
				reduc = password;
				for(unsigned nbr_hash_red = 0; nbr_hash_red < length_chains; ++nbr_hash_red){
					hash = sha256(reduc);
					reduc = reduce_hash(hash, nbr_hash_red, password_length);
#ifdef DEBUG
					std::cout << " -> " << hash.substr(0, 15) << " -> " << reduc;
#endif

				}
#ifdef DEBUG
				std::cout << std::endl;
#endif
				RainbowTable << reduc << std::endl;
			}

			// We close every stream and delete the temp psswd file
			passwd_table.close();
			std::remove("password.txt");
			RainbowTable.close();

		} else {
			std::cout << "Please specify all the required arguments : See README" << std::endl;
			std::cout << program;
		}


	/*
	*	===========
	*	ATTACK MODE
	*	===========
	*/
	} else if (program["--atk"] == true &&
		program.is_used("--rainbow_table") == true &&
		program.is_used("--length_chains") == true &&
		program.is_used("--password_length") == true) {

		// Params parsing
		auto rb_file = program.get<std::string>("--rainbow_table");
		unsigned length_chains = program.get<unsigned>("--length_chains");
		unsigned password_length = program.get<unsigned>("--password_length");
		
		// Attack variable
		unsigned nb_thread;
		std::string input;
		std::ofstream result_file;
		result_file.open("result.txt");


		/*
		*	===========
		*	SINGLE HASH
		*	===========
		*/
		if (program.is_used("--sha256") == true) {
			auto hash = program.get<std::string>("--sha256");

			// Asking for the number of threads for each hashs
			do {
				std::cout << "How many threads do you want to start per hash [Default: 5] ? ";
				std::getline(std::cin, input);
				
				if (input.empty()) {
					input = "5";
				}
			} while (!is_number(input) || input == "0");
			nb_thread = std::stoul(input);

			// Little verification and then we start the attack
			if (hash.size() == 64) {
				attack(hash, rb_file, length_chains, password_length, nb_thread, result_file);
			} else {
				std::cout << "Malformed hash" << std::endl;
			}


		/*
		*	==========
		*	HASHS FILE
		*	==========
		*/
		} else if (program.is_used("--sha256_file") == true) {
			auto sha256_file = program.get<std::string>("--sha256_file");
			std::ifstream to_crack(sha256_file);
			std::string hash;
			unsigned i = 0;
			
			// Asking if 1 thread/hash is active
			std::string activate_thread;
			do {
				std::cout << "Do you want to use 1 thread per hash [Y/n] ? ";
				std::getline(std::cin, activate_thread);
			} while (activate_thread != "y" && activate_thread != "" && activate_thread != "n");

			// Asking for the number of threads for each hashs
			do {
				std::cout << "How many threads do you want to start per hash [Default: 5] ? ";
				std::getline(std::cin, input);
				
				if (input.empty()) {
					input = "5";
				}
			} while (!is_number(input) || input == "0");
			nb_thread = std::stoul(input);

			// We start the attack
			if (activate_thread == "n") {
				while(getline(to_crack, hash)) {
					if (hash.size() == 64) {
						attack(hash, rb_file, length_chains, password_length, nb_thread, std::ref(result_file));
					} else {
						std::cout << "Malformed hash" << std::endl;
					}
					i++;
				}
			} else {
				std::thread attack_threads[100];
				while(getline(to_crack, hash)) {
					if (hash.size() == 64) {
						attack_threads[i] = std::thread(attack, hash, rb_file, length_chains, password_length, nb_thread, std::ref(result_file));
					} else {
						std::cout << "Malformed hash" << std::endl;
					}
					i++;
				}


				while(i > 0) {
					attack_threads[i-1].join();
					i--;
				}
			}

			to_crack.close();
		
		} else {
			display_help(program_help);
		}

	} else {
		display_help(program_help);
	}
	
	return 0;
}

bool is_number(const std::string& s) {
	return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

void display_help(std::string& program) {
	std::cout << "The correct way to use this program is :" << std::endl
		<< "	./main --gen -r <rt.csv> -l <x> -p <x> -n <x>" << std::endl
		<< "	./main --atk -r <rt.csv> -l <x> -p <x> -s <sha256>" << std::endl
		<< "	./main --atk -r <rt.csv> -l <x> -p <x> -S <sha256_file>" << std::endl << std::endl;
	std::cout << program;
}

void attack(std::string hash, std::string rb_file, unsigned length_chains, unsigned password_length, unsigned nb_thread, std::ofstream& result_file) {

	std::ifstream RainbowTable(rb_file);

	int i = length_chains;
	std::string head = "";
	std::thread find_threads[nb_thread];

#ifdef DEBUG
	std::cout << "++++++++++++++++++++++++" << std::endl
		<< "Trying to find head ... " << std::endl
		<< "++++++++++++++++++++++++" << std::endl;
#endif
	while (i>=0 && head.empty()){

#ifdef DEBUG
		std::cout << "===============" << std::endl
			<< "NEW WHILE ENTRY" << std::endl
			<< "===============" << std::endl
			<< "MAIN i : " << i << std::endl
			<< "---------" << std::endl;
#endif

		for (unsigned n = 0; n < nb_thread; n++) {
			find_threads[n] = std::thread(find_head, std::ref(head), hash, std::ref(RainbowTable), length_chains, i, password_length);
			i--;
		}
		for (unsigned n = 0; n < nb_thread; n++) {
			find_threads[n].join();
		}

#ifdef DEBUG
		std::cout << "------------" << std::endl
			<< "MAIN HEAD : " << head << std::endl
			<< "MAIN HASH : " << hash << std::endl
			<< std::endl;
#endif
	}

#ifdef DEBUG
	std::cout << std::endl
		<< "++++++++++++++++++++++++++++++++++++++++++" << std::endl
		<< "Trying to find corresponding password ... " << std::endl
		<< "++++++++++++++++++++++++++++++++++++++++++" << std::endl;
#endif

	std::string result = hash + ":";
	if (head.empty()){
		result += "/";
	} else {
		result += find_corresponding_password(head, hash, password_length);
	}
	write_result_mutex.lock();
	result_file << result << std::endl;
	write_result_mutex.unlock();
	std::cout << result << std::endl;
}

void find_head(std::string& head, std::string hash, std::ifstream& rainbow_table, unsigned length_chains, unsigned i, unsigned password_length) {
#ifdef DEBUG
    std::cout << "ARGUMENTS" << std::endl
              << "###########" << std::endl
              << "hash : " << hash << std::endl
              << "i : " << i << std::endl
              << "password length : " << password_length << std::endl
              << "length_chains : " << length_chains << std::endl;
#endif
    SHA256 sha256;
    std::string reduction;
    std::string rainbow_table_line;
    std::string tail_of_rainbow_table;
    const char delimiter = ';';

    reduction = reduce_hash(hash, i, password_length);
    while(i < length_chains) {
        while(getline(rainbow_table, rainbow_table_line)){
            tail_of_rainbow_table = rainbow_table_line.substr(rainbow_table_line.find(delimiter)+1);
        
            if(reduction.compare(tail_of_rainbow_table) == 0){
                head = rainbow_table_line.substr(0, rainbow_table_line.find(delimiter));
            }
        }
        rainbow_table.clear();
        rainbow_table.seekg(0);

        i++;
        hash = sha256(reduction);
        reduction = reduce_hash(hash, i, password_length);
    }
}

std::string find_corresponding_password(std::string& reduction, std::string& hash, unsigned password_length) {
	SHA256 sha256;
	unsigned i = 0;
	std::string hashed = sha256(reduction);

	while (hashed != hash) {
		reduction = reduce_hash(hashed, i, password_length);
		hashed = sha256(reduction);
		i++;
	}

	return reduction;
}