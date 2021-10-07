#include <iostream>
#include <fstream>
#include <cstring>
#include "sha256.h"
#include "reduction.hpp"
 
using std::string;
using std::cout;
using std::endl;
 
int main(int argc, char *argv[])
{
    SHA256 sha256;
    string last_red = "grape214";
    string output1 = sha256(last_red);
    unsigned password_length = 0;
    std::ifstream passwd_table("psswd.txt");
	std::ofstream RainbowTable("RT.csv");
    
    cout << "sha256('"<< last_red << "'):" << output1 << endl;
    std::cout << reduce_hash(output1, 0, output1.length() % 8);

    while (getline(passwd_table, last_red)){
		RainbowTable << last_red;
		RainbowTable << ';';
        password_length = strlen(last_red.c_str());

        for (unsigned i = 0; i < 50000; ++i) {
            output1 = sha256(last_red);
            last_red = reduce_hash(output1, i, password_length); //reduction
        }
        RainbowTable << last_red;
        RainbowTable << std::endl;
    }

    return 0;
}
