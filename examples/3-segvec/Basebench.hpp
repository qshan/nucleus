#ifndef BASEBENCH_H
#define BASEBENCH_H

#include <algorithm> 
#include <iostream> 
#include <fstream> 
#include <sstream>
#include <vector> 
#include <string> 
#include <chrono> 
#include <random> 

// Basebench comparison: Read/search word using std lib functions. 

namespace nucleus::examples::subclass {

const std::string filename = "/home/hughty/nucleus/bin/wlist_match1.txt";

class Basebench {
public:
    Basebench();
    void Shuffle(); 
    void Sort(); 
    long long BinSearch(std::string query = "0");
    long long SeqSearch(std::string query = "0");
private: 
    std::vector<std::string> wordlist; 
}; 

}

#endif 
