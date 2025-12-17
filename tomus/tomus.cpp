#include "tomus.h"

uint32_t computeScore(uint32_t guess)
{
    switch (guess)
    {
        case 1: return 250;
        case 2: return 200;
        case 3: return 150;
        case 4: return  75;
        case 5: return  50;
        default:
                return 0;
    }
}

Try::Try(std::string_view w) : 
    word(w), input(w.size(), '.'),
    states(w.size(), State::UNKNOWN),
    bestStates(w.size(), State::UNKNOWN)
{
    std::fill(letters.begin(), letters.end(), State::UNKNOWN);

    input[0] = w[0];
    states[0] = State::GOOD_POSITION;
    bestStates[0] = State::GOOD_POSITION;
}

void Tomus::NewWord()
{
    std::uniform_int_distribution<std::size_t> dist(0, config.words.size() - 1);
    
    unsigned int idx = dist(gen);
    if (currentTries.size() > 0)
    {
        history.push_back(currentTries);
        score += computeScore(currentTries.size() - 1);

        currentTries.clear();
    }
    currentTries.emplace_back(config.words[idx]);
}

InputResult Tomus::Input(const std::string& input)
{
    const auto& lastTry = currentTries.back();

    if (input.size() != lastTry.word.size())
        return InputResult::WRONG_LENGTH;
    if (input[0] != lastTry.word[0])
        return InputResult::WRONG_LETTER;
    
    if (!config.IsWordAdmissible(input)) 
        return InputResult::UNKNOWN_WORD;
    
    Try t = lastTry;

    // Copy input and reset letter states
    t.input = std::string{input};
    std::fill(t.states.begin() + 1, t.states.end(), State::UNKNOWN);

    for (unsigned int i = 1; i < t.word.size(); ++i)
    {
        const char current = input[i];
        const int idx = current - 'a';
        if (current == t.word[i])
        {
            t.states[i] = State::GOOD_POSITION;
            t.letters[idx] = State::GOOD_POSITION;
            t.bestStates[i] = State::GOOD_POSITION;
        }
    }
    // Update configuration now
    for (unsigned int i = 1; i < t.word.size(); ++i)
    {
        const char current = input[i];
        const int idx = current - 'a';
        if (current != t.word[i])
        {
            if (t.letters[idx] == State::NOT_IN_WORD)
                continue;

            int32_t countInWord  = 0;
            int32_t countGoodPos = 0;
            int32_t countLetter = 0;
            
            for (unsigned int j = 1; j < t.word.size(); ++j)
            {
                if (t.word[j] == current) countLetter++;

                if (input[j] == current)
                {
                    if (t.states[j] == State::GOOD_POSITION)
                        countGoodPos++;
                    if (t.states[j] == State::IN_WORD)
                        countInWord++;
                }
            }

            if (countLetter == 0)
            {
                t.letters[idx] = State::NOT_IN_WORD;
            }
            else if (countLetter - countInWord - countGoodPos > 0)
            {
                t.states[i] = State::IN_WORD;

                if (t.letters[idx] == State::UNKNOWN)
                    t.letters[idx] = State::IN_WORD;

                if (t.bestStates[i] == State::UNKNOWN)
                    t.bestStates[i] = State::IN_WORD;
            }
        }
    }
    
    currentTries.push_back(t);
    bool win = true;
    for (unsigned int i = 1; i < t.states.size(); ++i)
    {
        if (t.states[i] != State::GOOD_POSITION) 
            win = false;
    }

    if (win)
        return InputResult::WIN;

    if (currentTries.size() > config.maxTries)
        return InputResult::LOSE;
    return InputResult::VALID;
}

const std::vector<Try>& Tomus::Tries() const
{
    return currentTries;
}

const std::vector<std::vector<Try>>& Tomus::History() const
{
    return history;
}

unsigned int Tomus::Score() const
{
    return score;
}
