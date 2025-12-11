#include <format>
#include <nlohmann/json.hpp>
#include "raylib.h"
#include "tomus/tomus.h"

float GetSpacing(float fontSize) 
{
    if (fontSize > 10) return fontSize / 10;
    return 1;
}

struct DrawBoardConfig
{
    Vector2 topLeft;

    Font  font = GetFontDefault();
    Color fontColor = WHITE;
    float fontTargetSize = 50;
    Vector2 fontSpacing = {0, 0};

    Color gridColor = WHITE;
    float gridSize = 50;
    float gridThickness = 1.5;

    Color cursorColor = {0, 125, 135, 255};
    char emptyLetter = '.';
    float spacing = 0.f;

    Color goodColor   = {239, 73, 73, 255};
    Color inwordColor = {247, 185, 89, 255};

    DrawBoardConfig()
    { }

    float GetFontSize(char letter) const
    {
        GlyphInfo g = GetGlyphInfo(font, letter);
        float w = g.advanceX;
        float h = font.baseSize + g.offsetY;

        return font.baseSize * std::min(fontTargetSize / w, fontTargetSize / h);
    }
};


struct DrawLetterConfig
{
    Vector2 topLeft = {10, 10};

    Font font = GetFontDefault();
    Vector2 fontSpacing = {0, 0};
    Vector2 size = {15, 25};
    Vector2 spacing = {5, 5};

    float rounding = 0.5f;
    float thickness = 2;
    int segment = 100;
    
    Color defaultColor = WHITE;
    Color notinwordColor = {138, 173, 182, 255};
    Color goodColor   = {239, 73, 73, 255};
    Color inwordColor = {247, 185, 89, 255};

    DrawLetterConfig() 
    {}

    float GetFontSize(char letter) const
    {
        GlyphInfo g = GetGlyphInfo(font, letter);
        float w = g.advanceX;
        float h = font.baseSize + g.offsetY;

        return font.baseSize * std::min(size.x / w, size.y / h);
    }
};

struct DrawInfoConfig
{
    Font font = GetFontDefault();
    Color fontColor = WHITE;
    float letterSize = 0;
    float contentLength = 0;

    Vector2 wordPos;
    Vector2 errorPos;

    float GetFontSize(char letter) const
    {
        GlyphInfo g = GetGlyphInfo(font, letter);
        float w = g.advanceX;
        float h = font.baseSize + g.offsetY;

        return font.baseSize * std::min(letterSize / w, letterSize / h);
    }
};

struct DrawTomusConfig
{
    // As % of screen width or height
    float bigPadding = 0.05f;
    float smallPadding = 0.025f;

    float boardPositionX = 1.f / 4.f;
    float boardPositionY = 9.f / 40.f;
    float boardSize = 1.f / 15.f;
    float boardLetterSpacing = 1.f / 80.f;

    float historyScale = 2.f / 3.f;
    
    float letterSizeX = 1.f / 30.f;
    float letterSizeY = 1.f / 18.f;
    float letterSpacing = 1.f / 80.f;
    float letterLetterSpacing = 1.f / 80.f;

    float infoSize = 1.f / 25.f;

    Color backgroundColor = Color{.r = 15, .g = 81, .b = 101};
    DrawBoardConfig board;
    DrawBoardConfig history;
    DrawInfoConfig info;
    DrawLetterConfig letters;

    void Update(int width, int height, const Tomus& tomus)
    {
        const unsigned int maxWord = tomus.config.maxLength;
        const unsigned int wordSize = tomus.Tries()[0].word.size();
        const unsigned int tries = tomus.config.maxTries;
        const unsigned int letterRowCount = 10;

        int dim = std::min(width, height);
        Vector2 boardContentPos = {
            width  * boardPositionX,
            height * boardPositionY
        };

        board.fontTargetSize = dim * boardSize;
        board.gridSize       = dim * boardSize;
        board.fontSpacing.x  = dim * boardLetterSpacing;

        history.fontTargetSize = board.fontTargetSize * historyScale;
        history.gridSize       = board.gridSize       * historyScale;
        history.fontSpacing.x  = board.fontSpacing.x  * historyScale;

        float estimatedWidth = wordSize * (board.gridSize + board.gridThickness);
        board.topLeft.x = boardContentPos.x + (width - boardContentPos.x) / 2 - estimatedWidth / 2;
        board.topLeft.y = boardContentPos.y;

        history.topLeft.x = bigPadding * width;
        history.topLeft.y = board.topLeft.y - bigPadding * height;
        history.spacing = height * smallPadding;

        letters.size.x = width * letterSizeX;
        letters.size.y = height * letterSizeY;
        letters.spacing.x = dim * letterSpacing;
        letters.spacing.y = dim * letterSpacing;
        letters.fontSpacing.x = dim * letterLetterSpacing;

        // 10 is hardcoded here...
        float letterEstimatedWidth = 10 * (letters.size.x + letters.spacing.x + letters.thickness);
        letters.topLeft.x = boardContentPos.x + (width - boardContentPos.x) / 2 - letterEstimatedWidth / 2;
        letters.topLeft.y = boardContentPos.y + tries * (board.gridSize + board.gridThickness) + bigPadding * height;
        
        info.letterSize = dim * infoSize;
        info.contentLength = maxWord * (board.gridSize + board.gridThickness);
        info.wordPos.x = boardContentPos.x + (width - boardContentPos.x) / 2;
        info.wordPos.y = boardContentPos.y - bigPadding * height;

        info.errorPos.x = boardContentPos.x + (width - boardContentPos.x) / 2;
        info.errorPos.y = letters.topLeft.y + 3 * (letters.size.y + letters.spacing.x) + smallPadding * height;
    }
};

void DrawBoard(
    const DrawBoardConfig& config, 
    const std::vector<Try> tries, 
    const unsigned int maxTries, 
    const std::string& currentInput, 
    bool withGrid, bool withInput
)
{
    const char majShift = 'A' - 'a';
    const auto computePosX = [&](int idx, bool spacing = false) {
        return config.topLeft.x + idx * config.gridSize + spacing * config.fontSpacing.x;
    };
    const auto computePosY = [&](int idx, bool spacing = false) {
        return config.topLeft.y + idx * config.gridSize - spacing * config.fontSpacing.y;
    };
    
    for (unsigned int i = 1; i < tries.size(); ++i)
    {
        const float y = computePosY(i - 1, true);

        for (unsigned int j = 0; j < tries[i].input.size(); ++j)
        {
            const float x = computePosX(j, true);
            const char buff[2] = {(char)(majShift + tries[i].input[j]), '\0'};

            Rectangle rect = {
                .x = computePosX(j) + config.gridThickness / 2, 
                .y = computePosY(i - 1) + config.gridThickness / 2,
                .width  = config.gridSize,
                .height = config.gridSize
            };
            if (tries[i].states[j] == State::GOOD_POSITION)
            {
                DrawRectangleRec(rect, config.goodColor);
            }
            else if (tries[i].states[j] == State::IN_WORD)
            {
                DrawRectangleRec(rect, config.inwordColor);
            }
            
            auto fsize = config.GetFontSize(buff[0]);
            auto spacing = GetSpacing(fsize);
            DrawTextEx(config.font, buff, Vector2{x, y}, fsize, spacing, config.fontColor);
        }
    }

    if (withInput && tries.size() <= maxTries)
    {
        const auto& lastTry = tries.back();
        const float y = computePosY(tries.size() - 1, true);
        const char buffEmpty[2] = {config.emptyLetter, '\0'};

        // Draw cursor 
        {
            unsigned int pos = currentInput.size();
            if (currentInput.size() > 0) pos --;

            const float x = computePosX(pos);

            Rectangle rect = {
                .x = computePosX(pos) + config.gridThickness / 2, 
                .y = computePosY(tries.size() - 1) + config.gridThickness / 2,
                .width  = config.gridSize,
                .height = config.gridSize
            };
            DrawRectangleRec(rect, config.cursorColor);
        }


        for (unsigned int j = 0; j < lastTry.word.size(); ++j)
        {
            const float x = computePosX(j, true);
            
            if (j < currentInput.size() && j != 0)
            {
                const char buff[2] = {(char)(currentInput[j] + majShift), '\0'};
                
                auto fsize = config.GetFontSize(buff[0]);
                auto spacing = GetSpacing(fsize);
                DrawTextEx(config.font, buff, Vector2{x, y}, fsize, spacing, config.fontColor);
            }
            else
            {
                if (lastTry.bestStates[j] == State::GOOD_POSITION)
                {
                    const char buff[2] = {(char)(lastTry.word[j] + majShift), '\0'};
                    auto fsize = config.GetFontSize(buff[0]);
                    auto spacing = GetSpacing(fsize);
                    DrawTextEx(config.font, buff, Vector2{x, y}, fsize, spacing, config.fontColor);
                }
                else
                {
                    auto fsize = config.GetFontSize(buffEmpty[0]);
                    auto spacing = GetSpacing(fsize);
                    DrawTextEx(config.font, buffEmpty, Vector2{x, y}, fsize, spacing, config.fontColor);
                }
            }
        }
    }

    if (withGrid)
    {
        // TODO: Thickness is not correct on edges
        for (unsigned int i = 0; i < maxTries; ++i)
        {
            const float y = computePosY(i);
            for (unsigned int j = 0; j < tries[0].word.size(); ++j)
            {
                const float x = computePosX(j);

                Rectangle rect = {
                    .x = x, .y = y,
                    .width = config.gridSize, 
                    .height = config.gridSize
                };
                DrawRectangleLinesEx(rect, config.gridThickness, config.gridColor);
            }
        }
    }
}

void DrawHistory(
        const DrawBoardConfig& conf, 
        const std::vector<std::vector<Try>>& history,
        int maxH = 3
)
{
    if (history.size() == 0) return;
    
    unsigned int start = std::max((int)history.size() - (int)maxH, 0);
    unsigned int y = conf.topLeft.y;

    DrawBoardConfig copy = conf;
    for (unsigned int i = start; i < history.size(); ++i)
    {
        DrawBoard(copy, history[i], 0, "", false, false);
        copy.topLeft.y += conf.spacing + history[i].size() * (conf.gridSize + conf.gridThickness);
    }
}

void DrawLetter(const DrawLetterConfig& conf, char letter, Vector2 pos, State s)
{
    const char majShift = 'A' - 'a';
    Color fontColor = conf.defaultColor;
    if (s == State::NOT_IN_WORD) fontColor = conf.notinwordColor;
    
    Rectangle rec {
        .x = pos.x, .y = pos.y, 
        .width = conf.size.x, .height = conf.size.y
    };
    DrawRectangleRoundedLinesEx(rec, conf.rounding, conf.segment, conf.thickness, fontColor);
    
    if (s == State::GOOD_POSITION)
        DrawRectangleRounded(rec, conf.rounding, conf.segment, conf.goodColor);
    else if (s == State::IN_WORD)
        DrawRectangleRounded(rec, conf.rounding, conf.segment, conf.inwordColor);
    
    const char l = (char)(majShift + letter);
    const char buffer[2] = {l, '\0'};

    auto fsize = conf.GetFontSize(buffer[0]);
    auto spacing = GetSpacing(fsize);
    Vector2 tpos{pos.x + conf.fontSpacing.x, pos.y + conf.fontSpacing.y};
    DrawTextEx(conf.font, buffer, tpos, fsize, spacing, fontColor);
}


void DrawLetters(
    const DrawLetterConfig& conf,
    const std::array<State, 26>& states)
{
    static const std::array<std::string, 3> layout = {
        "azertyuiop", 
        "qsdfghjklm", 
        "  wxcvbn  "
    };
    
    for (unsigned int i = 0; i < layout.size(); ++i)
    {
        for (unsigned int j = 0; j < layout[i].size(); ++j)
        {
            if (layout[i][j] < 'a' || layout[i][j] > 'z') continue;
            const float x = conf.topLeft.x + j * (conf.size.x + conf.spacing.x);
            const float y = conf.topLeft.y + i * (conf.size.y + conf.spacing.y);
            DrawLetter(conf, layout[i][j], {x, y}, states[layout[i][j] - 'a']);
        }
    }
}

void DrawInfo(const DrawInfoConfig& conf, 
    int wCount, int score, int time,
    const std::string& err
)
{
    const int m = time / 60;
    const int s = time % 60;
    const std::string wMsg = std::format("Mot: {}", wCount);
    const std::string sMsg = std::format("Score: {}", score);
    const std::string tMsg = std::format("Timer: {:02}m{:02}", time / 60, time % 60);
    const std::string eMsg = err;

    int fSize = conf.GetFontSize('M'); // Biggest letter I guess
    int spacing = (fSize > 10) ? fSize / 10 : 1;
    Vector2 eS = MeasureTextEx(conf.font, eMsg.c_str(), fSize, spacing);
    Vector2 wS = MeasureTextEx(conf.font, wMsg.c_str(), fSize, spacing);
    Vector2 sS = MeasureTextEx(conf.font, sMsg.c_str(), fSize, spacing);
    Vector2 tS = MeasureTextEx(conf.font, tMsg.c_str(), fSize, spacing);

    float newY = conf.wordPos.y - wS.y / 2;
    float wX = conf.wordPos.x;
    float sX = conf.wordPos.x + conf.contentLength / 2 - sS.x / 2;
    float tX = conf.wordPos.y + conf.contentLength - tS.y;

    DrawText(wMsg.c_str(), wX, newY, fSize, conf.fontColor);
    DrawText(sMsg.c_str(), sX, newY, fSize, conf.fontColor);
    DrawText(tMsg.c_str(), tX, newY, fSize, conf.fontColor);

    float eX = conf.errorPos.x - eS.x / 2;
    float eY = conf.errorPos.y;
    DrawText(eMsg.c_str(), eX, eY, fSize, conf.fontColor);
}

Config LoadConfig(const std::string& path)
{
    using json = nlohmann::json;

    Config conf;
    std::ifstream file(path);
    try
    {
        if (file)
        {
            json data = json::parse(file);

            conf.minLength = data["minLength"].get<unsigned int>();
            conf.maxLength = data["maxLength"].get<unsigned int>();
            conf.maxTries  = data["maxTries"].get<unsigned int>();
            conf.maxTime   = data["maxTime"].get<unsigned int>();
            
            const std::string dicPath = data["mots"].get<std::string>();
            const std::string admPath = data["admissibles"].get<std::string>();

            std::ifstream fDic(dicPath);
            std::ifstream aDic(admPath);
            if (fDic && aDic)
            {
                std::string buffer;
                std::vector<std::string> words;
                while (std::getline(fDic, buffer))
                {
                    if (buffer.size() > conf.minLength && buffer.size() < conf.maxLength)
                        words.push_back(buffer);
                }
                conf.SetWords(words);
                std::cout << "Loaded: " << words.size() << std::endl;

                words.resize(0);
                while (std::getline(aDic, buffer))
                {
                    if (buffer.size() >= conf.minLength && buffer.size() <= conf.maxLength)
                        words.push_back(buffer);
                }
                conf.SetAdmissible(words);
                std::cout << "Loaded: " << words.size() << std::endl;

                return conf;
            }
            else
            {
                std::cerr << "Errro, can not load dictionnary: " << dicPath << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cerr << "Error, can not load [no file]: " << path << std::endl;
            exit(1);
        }
    }
    catch(std::exception& ex)
    {
        std::cerr << "Error, can not load: " << path << "; error = " << ex.what() << std::endl;
        exit(1);
    }
    return conf;
}

int main(int argc, char** argv)
{
    Config conf;
    if (argc > 1) conf = LoadConfig(argv[1]);
    else          conf = LoadConfig("config.json");

    Tomus tomus(conf);
    tomus.NewWord();

    const int screenWidth = 800;
    const int screenHeight = 450;
    const int currentFps = 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "TOMUS");
    SetTargetFPS(currentFps);

    DrawTomusConfig drawConf;
    std::vector<char> buffer(conf.maxLength + 1, '\0');
    unsigned int buffSize = 0;

    std::string currentInput = "";
    std::string errorString = "";
    double startTime = -1.0;
    double showErrTime = GetTime();
    double freezeTime = 0.;

    bool playing = true;
    while (!WindowShouldClose()) 
    {
        const auto& tries = tomus.Tries();
        const auto& word = tries[0].word;

        ClearBackground(drawConf.backgroundColor);
        
        if (playing)
        {
            if (IsKeyPressed(KEY_ENTER) && buffSize == tomus.Tries()[0].word.size())
            {
                // Enforce first letter
                buffer[0] = word[0];

                auto rslt = tomus.Input(&buffer[0]);
                buffer[0] = '\0';
                buffSize = 0;
                errorString = "";

                if (rslt == InputResult::WIN)
                {
                    tomus.NewWord();
                }
                else if (rslt == InputResult::UNKNOWN_WORD)
                {
                    errorString = "Ce mot n'est pas dans la liste";
                    showErrTime = GetTime();
                }
                else if (rslt == InputResult::LOSE)
                {
                    errorString = std::format("Le mot etait: {}", word);
                    playing = false;
                }

                freezeTime = GetTime() - startTime; 
            }

            int key = GetCharPressed();
            while (key > 0)
            {
                if ((key >= 'a' && key <= 'z'))
                {
                    if (startTime < 0.0)
                        startTime = GetTime();

                    if (buffSize == 0)
                    {
                        if (key != word[0]) 
                        {
                            buffer[buffSize] = word[0]; 
                            buffer[buffSize + 1] = '\0';
                            buffSize ++;
                        }
                    }

                    if (buffSize != 0)
                    {
                        if (buffSize < word.size())
                        {
                            buffer[buffSize] = key;
                            buffer[buffSize + 1] = '\0';
                            buffSize ++;
                        }
                    }
                }

                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                if (buffSize > 0)
                {
                    buffSize --;
                    buffer[buffSize] = '\0';
                }
            }

            if (playing && GetTime() - showErrTime > 5)
                errorString = "";
        }

        int time = (!playing) ? freezeTime : (startTime > 0) ? GetTime() - startTime : 0;
        drawConf.Update(GetScreenWidth(), GetScreenHeight(), tomus);
        
        BeginDrawing();
            DrawBoard(drawConf.board, tries, tomus.config.maxTries, &buffer[0], true, true);
            DrawLetters(drawConf.letters, tries.back().letters);
            DrawInfo(drawConf.info, tomus.History().size() + 1, tomus.Score(), time, errorString);
            DrawHistory(drawConf.history, tomus.History());
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
