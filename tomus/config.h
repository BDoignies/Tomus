#include <string_view>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <array>
#include <set>

struct Config
{
    Config()
    {}
    
    void SetWords(const std::vector<std::string>& words);
    bool IsWordAdmissible(const std::string& str) const;

    std::vector<std::string> words;
    std::set<std::string> admissibleWords;

    uint32_t minLength = 5;
    uint32_t maxLength = 8;
    uint32_t maxTries  = 6;
    uint32_t maxTime   = 30 * 60;
private:
};


