#include <iostream>
#include <fstream>
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
    std::ifstream passwd_table("psswd.txt");
	std::ofstream RainbowTable("RT.csv");
    
    cout << "sha256('"<< last_red << "'):" << output1 << endl;
    std::cout << reduce_hash(output1, 0, output1.length());

    while (getline(passwd_table, last_red)){
        std::cout << last_red << std::endl;
		RainbowTable << last_red;
		RainbowTable << ';';
        for (unsigned i = 0; i < 50000; ++i) {
            output1 = sha256(last_red);
            //std::cout << "output1: " << output1 << std::endl;
            last_red = reduce_hash(output1, i, (int)output1.length()); //reduction
            //std::cout << "last_red: " << last_red << std::endl;
        }
        RainbowTable << last_red;
        RainbowTable << std::endl;
    }

    return 0;
}
