<h1 align="center"> SSD - Rainbow Table </h1>

In the context of our Secure Software Design and Web Security course, we were asked to develop a rainbow table attack on a file containing passwords hashed with the cryptographic function sha256.

## Built with

To complete this task, we needed to choose one of the fastest programming language and one that has access to libraries provided by our teacher (for simplicity reasons). Therefore, all of us agreed that the best solution for this assignment was to use C++ due to high performance it can provide.

### External libraries

During development, we used few libraries to make our work easier:

- [argparse](http://github.com/p-ranav/argparse): This library allows a better handling of our arguments
- sha256, password-utils: Teacher's provided libraries to perform the hashing and generation of passwords

## Installation

To install this project on your device, please follow theses commands :

```bash
$ git clone https://gitlab.com/Herrgrim0/ssd-rainbow-table.git
$ cd ssd-rainbow-table/utils
$ make
$ ./main -h
```

## Usage

This program can be run in two different modes depending on the task you want to perform:

### Generation
To generate a rainbow table, you need to use this syntax
```bash
$ ./main --gen -r <file.csv>  -l <x> -p <x> -n <x>
```
Example :
```bash
$ ./main --gen -r rt.csv -l 10 -p 8 -n 5
```
This command will generate a rainbow table with the following specifications:
```
- Number of chains (n) = 5
- Length of chains (l) = 10
- Length of password (p) = 8
- Rainbow table file (r) = rt.csv
```

### Attack
To attack a hash, you need to use the following syntax :

```bash
$ ./main --atk -r <file.csv> -l <x> -p <x> -s <sha256>
$ ./main --atk -r <file.csv> -l <x> -p <x> -S <sha256.txt>
```

Examples :
```bash
$ ./main --atk -r rt.csv -l 10 -p 8 -s f2d81a260dea8a100dd517984e53c56a7523d96942a834b9cdc249bd4e8c7aa9
$ ./main --atk -r rt.csv -l 10 -p 8 -S hashes_to_crack.txt
```
Those commands will attack a hash or a file containing hashs where :
```
- Hash to crack (s) = f2d81a260dea8a100dd517984e53c56a7523d96942a834b9cdc249bd4e8c7aa9
- Set of hashs to crack (S) = hashes_to_crack.txt
- Length of chains (l) = 10
- Length of password (p) = 8
- Rainbow table file (r) = rt.csv
```
### Combine both commands
In order to generate the rainbow table and attack a single hash after. You simply need to combine both commands with the operator `&&`
```bash
$ ./main --gen -r rt.csv -l 5 -p 8 -n 10 && ./main --atk -r rt.csv -l 5 -p 8 -s f2d81a260dea8a100dd517984e53c56a7523d96942a834b9cdc249bd4e8c7aa9
```

## Authors
* **Romain Grimaru**
* **Maximilien Laenen**
* **Mehdi Laurent**
* **Nathan Pembe Lemlin**
* **Arkadiusz Snarski**

## License
The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
