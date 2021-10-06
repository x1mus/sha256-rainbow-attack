#ifndef REDUCTION_HPP
#define REDUCTION_HPP

std::string reduce_hash(std::string, int, int);
inline void padding_reduction(std::string&, int);
inline std::string reduced_hash_to_new_password(std::string&);

#endif