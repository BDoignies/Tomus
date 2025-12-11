#include "config.h"

void Config::SetWords(const std::vector<std::string>& wds)
{
    for (const auto& w : wds)
        words.push_back(w);
}

void Config::SetAdmissible(const std::vector<std::string>& wds)
{
    for (const auto& w : wds)
        admissibleWords.insert(w);
}

bool Config::IsWordAdmissible(const std::string& sv) const
{
    auto it = admissibleWords.find(sv);
    return it != admissibleWords.end();
}
