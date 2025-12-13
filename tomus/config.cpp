#include "config.h"

void Config::SetWords(const std::vector<std::string>& wds)
{
    for (const auto& w : wds)
    {
        words.push_back(w);
        admissibleWords.insert(w); // Make sure every solution is admissible
    }
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
