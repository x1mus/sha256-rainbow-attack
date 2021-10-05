#include <iostream>
#include "sha256.h"
 
using std::string;
using std::cout;
using std::endl;
 
int main(int argc, char *argv[])
{
    SHA256 sha256;
    const string input = "grape";
    string output1 = sha256(input);
 
    cout << "sha256('"<< input << "'):" << output1 << endl;
    return 0;
}
