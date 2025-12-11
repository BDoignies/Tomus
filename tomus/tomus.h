#include "config.h"

enum class State 
{
    GOOD_POSITION = 0,
    IN_WORD = 1,
    NOT_IN_WORD = 2,
    UNKNOWN = 3
};

enum class InputResult
{
    WRONG_LENGTH = 0, 
    WRONG_LETTER = 1,
    UNKNOWN_WORD = 2,
    VALID = 3, 
    WIN = 4,
    LOSE = 5
};

struct Try
{
    Try(std::string_view word);

    std::string word;
    std::string input;

    std::vector<State> states;
    std::vector<State> bestStates;
    std::array<State, 26> letters;
};

struct Tomus
{
public:
    Tomus(const Config& c) : config(c), gen(std::random_device{}())
    { }

    void NewWord();

    InputResult Input(const std::string& input);

    const std::vector<Try>& Tries() const;
    const std::vector<std::vector<Try>>& History() const;
    unsigned int Score() const;
    const Config& config;
private:
    unsigned int score = 0;
    std::vector<Try> currentTries;   
    std::vector<std::vector<Try>> history;

    std::mt19937 gen;
};

