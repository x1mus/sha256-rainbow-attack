#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include "sha256.h"
#include "reduction.hpp"
#include "argparse.hpp"
#include "passwd-utils.hpp"
#include <thread>

#ifdef DEBUG
# define D(x) x
#else
# define D(x)
#endif

void display_help(std::string&);
void attack(std::string, std::string, unsigned, unsigned);
void find_corresponding_password(std::string&, std::string&, unsigned, unsigned);
void find_head(std::string&, std::string, std::ifstream&, unsigned, unsigned, unsigned);

int main(int argc, char *argv[]) {
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

	// Figuring out which mode the user is using
	if (program["--gen"] == true && program["--atk"] == true) {
		display_help(program_help);
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

	} else if (program["--atk"] == true &&
		program.is_used("--rainbow_table") == true &&
		program.is_used("--length_chains") == true &&
		program.is_used("--password_length") == true) {

		auto rb_file = program.get<std::string>("--rainbow_table");
		unsigned length_chains = program.get<unsigned>("--length_chains");
		unsigned password_length = program.get<unsigned>("--password_length");
		
		// MODE == ATK
		if (program.is_used("--sha256") == true) {
			auto hash = program.get<std::string>("--sha256");

			if (hash.size() == 64) {
				attack(hash, rb_file, length_chains, password_length);
			} else {
				std::cout << "Malformed hash" << std::endl;
			}

		} else if (program.is_used("--sha256_file") == true) {
			auto sha256_file = program.get<std::string>("--sha256_file");
			std::ifstream to_crack(sha256_file);
			std::string hash;

			std::thread attack_threads[100];

			unsigned i = 0;
			while(getline(to_crack, hash)) {
				if (hash.size() == 64) {
					attack_threads[i] = std::thread(attack, hash, rb_file, length_chains, password_length);
				} else {
					std::cout << "Malformed hash" << std::endl;
				}
				i++;
			}


			while(i > 0) {
       			attack_threads[i-1].join();
       			i--;
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

void display_help(std::string& program) {
	std::cout << "The correct way to use this program is :" << std::endl
		<< "	./main --gen -r <rt.csv> -l <x> -p <x> -n <x>" << std::endl
		<< "	./main --atk -r <rt.csv> -l <x> -p <x> -s <sha256>" << std::endl
		<< "	./main --atk -r <rt.csv> -l <x> -p <x> -S <sha256_file>" << std::endl << std::endl;
	std::cout << program;
}

void attack(std::string hash, std::string rb_file, unsigned length_chains, unsigned password_length) {

	std::ifstream RainbowTable(rb_file);

	int i = length_chains;
	std::string head = "";
	std::thread find_threads[5];

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

		//find_head(head, hash, RainbowTable, length_chains, i, password_length);
		for (unsigned n = 0; n < 5; n++) {
			find_threads[n] = std::thread(find_head, std::ref(head), hash, std::ref(RainbowTable), length_chains, i, password_length);
			i--;
		}
		for (unsigned n = 0; n < 5; n++) {
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

	if (head.empty()){
		std::cout << hash << ":/" << std::endl;
	} else {
		find_corresponding_password(head, hash,0, password_length);
	}
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

void find_corresponding_password(std::string& reduction, std::string& hash, unsigned i, unsigned password_length) {
	SHA256 sha256;
	std::string hashed = sha256(reduction);
	if(hashed == hash) {
		std::cout << hash << ":" << reduction << std::endl;
	} else {
		hashed = reduce_hash(hashed, i, password_length);
		find_corresponding_password(hashed, hash, ++i, password_length);
	}
}