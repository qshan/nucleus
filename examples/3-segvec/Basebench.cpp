#include "Basebench.hpp" 

namespace nucleus::examples::subclass {

// https://stackoverflow.com/questions/2808398/easily-measure-elapsed-time 
Basebench::Basebench() {
    std::cout << "READ WORD LIST INTO VECTOR:\n"; 
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(); 

    std::string line; 
    std::ifstream infile(filename); 
    while (std::getline(infile, line))  {
        wordlist.push_back(line); 
    }

    std::chrono::steady_clock::time_point stop  = std::chrono::steady_clock::now(); 
    std::cout << "TIME TAKEN = " << std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count() << " ms.\n"; 
}

// https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector
void 
Basebench::Shuffle() { 
    std::cout << "SHUFFLE WORD LIST:\n"; 
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(); 

    auto rng = std::default_random_engine {}; 
    std::shuffle(wordlist.begin(), wordlist.end(), rng); 

    std::chrono::steady_clock::time_point stop  = std::chrono::steady_clock::now(); 
    std::cout << "TIME TAKEN = " << std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count() << " ms.\n"; 
}

void 
Basebench::Sort() {
    std::cout << "SORT WORD LIST:\n"; 
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(); 

    std::sort(wordlist.begin(), wordlist.end()); 

    std::chrono::steady_clock::time_point stop  = std::chrono::steady_clock::now(); 
    std::cout << "TIME TAKEN = " << std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count() << " ms.\n"; 
}

// https://stackoverflow.com/questions/27431029/binary-search-with-returned-index-in-stl 
long long 
Basebench::BinSearch(std::string query) {
    std::cout << "SEARCH WORD LIST:\n"; 
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now(); 

    long long Res = -1; 
    auto it = std::lower_bound(wordlist.begin(), wordlist.end(), query); 
    if (it == wordlist.end() || *it != query) { 
        Res = -1; 
    } else { 
        Res = std::distance(wordlist.begin(), it); 
    } 

    std::chrono::steady_clock::time_point stop  = std::chrono::steady_clock::now(); 
    std::cout << "TIME TAKEN = " << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count() << " us.\n";
    return Res; 
}

long long
Basebench::SeqSearch(std::string query) {
    std::cout << "SEARCH WORD LIST:\n";
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    long long Res = -1;
    for (auto it = wordlist.begin(); it != wordlist.end(); ++it) {
        if (*it == query) {
            Res = std::distance(wordlist.begin(), it);
            break;
        }
    }

    std::chrono::steady_clock::time_point stop  = std::chrono::steady_clock::now();
    std::cout << "TIME TAKEN = " << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count() << " us.\n";
    return Res;
}

}
