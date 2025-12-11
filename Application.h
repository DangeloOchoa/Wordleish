#ifndef APPLICATION_H
#define APPLICATION_H

#include <bobcat_ui/all.h>
#include <bobcat_ui/bobcat_ui.h>
#include <bobcat_ui/button.h>
#include <bobcat_ui/input.h>
#include <bobcat_ui/textbox.h>
#include <bobcat_ui/window.h>
#include <vector>
#include <string>

class Application : public bobcat::Application_ {

    // Word list
    std::vector<std::string> words;

    // Game state
    std::string targetWord;
    int guessCount = 0;
    std::vector<std::string> previousGuesses;

    std::string persistentCorrect = "_____";
    std::string persistentWrong = "";
    std::string persistentMisplaced = "";

    bobcat::TextBox *misplacedRow;
    

    // Score tracking
    int totalScore = 0;
    int wins = 0;
    int losses = 0;

    struct GameRecord {
        std::string word;
        int guesses;
        int points;
        bool win;
    };

    std::vector<GameRecord> gameHistory;
    std::string saveFile = "../assets/score_history.txt";

    // Windows and UI objects
    bobcat::Window *window;
    bobcat::TextBox *textbox;
    bobcat::Button *helpButton;
    bobcat::Button *playButton;
    bobcat::Button *SCORE;

    bobcat::Window *instructionsWindow;
    bobcat::TextBox *instructionsText;
    bobcat::TextBox *instructionsText1;
    bobcat::TextBox *instructionsText2;
    bobcat::TextBox *instructionsText3;

    bobcat::TextBox *pointsbreakdown;
    bobcat::TextBox *pointsbreakdown1;
    bobcat::TextBox *pointsbreakdown2;
    bobcat::TextBox *pointsbreakdown3;
    bobcat::TextBox *pointsbreakdown4;
    bobcat::TextBox *pointsbreakdown5;
    bobcat::TextBox *pointsbreakdown6;

    bobcat::Button *backbutton;

    bobcat::Window *gamewindow;
    bobcat::TextBox *wrongLettersRow;
    bobcat::TextBox *correctLettersRow;
    bobcat::Input *guessInputRow;
    bobcat::TextBox *guessRows[6];
    bobcat::TextBox *errorRow;

    bobcat::Window *scoreboard;
    bobcat::TextBox *scoreSummary;
    bobcat::TextBox *historyDisplay;
    bobcat::Button *scoreboardBackButton;

    // WIN window
    bobcat::Window *winWindow;
    bobcat::TextBox *winMessage;
    bobcat::Button *winBackButton;

    // LOSE window
    bobcat::Window *loseWindow;
    bobcat::TextBox *loseMessage;
    bobcat::Button *loseBackButton;

    

    // Helpers
    void loadHistory();
    void saveRecord(const GameRecord &rec);
    int calculatePoints(int guessNum);

    // Callbacks
    void handleInstructionsButtonClick(bobcat::Widget *sender);
    void handleBackButtonClick(bobcat::Widget *sender);
    void handlePlayButtonClick(bobcat::Widget *sender);
    void handleSCOREButtonClick(bobcat::Widget *sender);
    void handleScoreBackButtonClick(bobcat::Widget *sender);
    void handleGuessSubmit(bobcat::Widget *sender);

    void handleWinBack(bobcat::Widget *sender);
    void handleLoseBack(bobcat::Widget *sender);

public:
    Application();
};

#endif
