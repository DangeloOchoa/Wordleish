#include <Application.h>
#include <bobcat_ui/bobcat_ui.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

using namespace std;
using namespace bobcat;

Application::Application() {

    // Load words
    ifstream file("../assets/Words.csv");
    if (!file.is_open()) {
        cout << "Could not open Words.csv\n";
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty()) words.push_back(line);
    }

    srand(time(nullptr));

    // Proper shuffle (C++17-compliant)
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(words.begin(), words.end(), g);

    targetWord = words[rand() % words.size()];

    // Load previous games
    loadHistory();

    // ---------------- MAIN MENU ----------------
    window = new Window(100, 100, 400, 400, "Programming Project");

    textbox = new TextBox(145, 45, 350, 25, "Wordle-ish Game");
    helpButton = new Button(150, 300, 100, 25, "Instructions");
    playButton = new Button(150, 275, 100, 25, "Play");
    SCORE = new Button(150, 100, 100, 25, "Scoreboard");

    window->end();

    // ---------------- INSTRUCTIONS WINDOW ----------------
    instructionsWindow = new Window(100, 100, 500, 400, "Instructions");
    instructionsText = new TextBox(20, 20, 350, 25, "You have 6 guesses to guess a 5 letter word.");
    instructionsText1 = new TextBox(20, 45, 350, 25, "Use clues from previous guesses.");
    instructionsText2 = new TextBox(20, 70, 350, 25, "Points depend on how fast you guess.");
    instructionsText3 = new TextBox(20, 95, 350, 25, "Good luck!");

    pointsbreakdown = new TextBox(20, 140, 300, 25, "1st = 1000 points");
    pointsbreakdown1 = new TextBox(20, 165, 300, 25, "2nd = 800 points");
    pointsbreakdown2 = new TextBox(20, 190, 300, 25, "3rd = 600 points");
    pointsbreakdown3 = new TextBox(20, 215, 300, 25, "4th = 400 points");
    pointsbreakdown4 = new TextBox(20, 240, 300, 25, "5th = 200 points");
    pointsbreakdown5 = new TextBox(20, 265, 300, 25, "6th = 100 points");
    pointsbreakdown6 = new TextBox(20, 290, 300, 25, "0 if you fail");

    backbutton = new Button(350, 350, 100, 30, "Back");
    instructionsWindow->end();

    // ---------------- GAME WINDOW ----------------
    gamewindow = new Window(100, 100, 600, 650, "WORDLEish");

    // Clean updated layout with new error row
    wrongLettersRow    = new TextBox(50, 80, 600, 40, "Not in word:");
    correctLettersRow  = new TextBox(50, 120, 600, 40, "Correct letters:");
    misplacedRow       = new TextBox(50, 160, 600, 40, "Misplaced:");
    
    guessInputRow      = new Input(50, 210, 300, 30, "Enter guess:");

    // NEW: Dedicated error message row (no more overlapping UI)
    errorRow           = new TextBox(50, 250, 600, 40, "");

    // Guess rows shifted down
    for (int i = 0; i < 6; i++)
        guessRows[i] = new TextBox(50, 300 + i * 30, 300, 25, "-----");

    gamewindow->end();

    // ---------------- SCOREBOARD WINDOW ----------------
    scoreboard = new Window(100, 100, 450, 400, "Scoreboard");

    scoreSummary = new TextBox(20, 20, 350, 25, "Score:");
    historyDisplay = new TextBox(20, 60, 350, 250, "History:");
    scoreboardBackButton = new Button(20, 330, 100, 30, "Back");

    scoreboard->end();

    // ---------------- WIN WINDOW ----------------
    winWindow = new Window(150, 150, 300, 200, "Result");
    winMessage = new TextBox(50, 50, 200, 25, "YOU WIN!");
    winBackButton = new Button(100, 120, 100, 30, "Back");
    winWindow->end();

    // ---------------- LOSE WINDOW ----------------
    loseWindow = new Window(150, 150, 300, 200, "Result");
    loseMessage = new TextBox(50, 50, 200, 25, "YOU LOSE!");
    loseBackButton = new Button(100, 120, 100, 30, "Back");
    loseWindow->end();

    // ---------------- EVENT HOOKS ----------------
    ON_CLICK(helpButton, Application::handleInstructionsButtonClick);
    ON_CLICK(backbutton, Application::handleBackButtonClick);
    ON_CLICK(playButton, Application::handlePlayButtonClick);
    ON_CLICK(SCORE, Application::handleSCOREButtonClick);
    ON_CLICK(scoreboardBackButton, Application::handleScoreBackButtonClick);

    ON_CLICK(winBackButton, Application::handleWinBack);
    ON_CLICK(loseBackButton, Application::handleLoseBack);

    ON_ENTER(guessInputRow, Application::handleGuessSubmit);

    window->show();
}

// ----------------------------------------------------------
// Load history
void Application::loadHistory() {
    ifstream in(saveFile);
    if (!in.is_open()) return;

    GameRecord rec;
    while (in >> rec.word >> rec.guesses >> rec.points >> rec.win) {
        gameHistory.push_back(rec);
        totalScore += rec.points;
        if (rec.win) wins++;
        else losses++;
    }
}

// Save a record
void Application::saveRecord(const GameRecord &rec) {
    ofstream out(saveFile, ios::app);
    if (!out.is_open()) return;

    out << rec.word << " " << rec.guesses << " "
        << rec.points << " " << rec.win << "\n";
}

// Scoring system
int Application::calculatePoints(int guessNum) {
    switch (guessNum) {
        case 1: return 1000;
        case 2: return 800;
        case 3: return 600;
        case 4: return 400;
        case 5: return 200;
        case 6: return 100;
        default: return 0;
    }
}

// ----------------------------------------------------------
// CALLBACKS

void Application::handleInstructionsButtonClick(Widget *sender) {
    window->hide();
    instructionsWindow->show();
}

void Application::handleBackButtonClick(Widget *sender) {
    instructionsWindow->hide();
    window->show();
}

void Application::handlePlayButtonClick(Widget *sender) {
    guessCount = 0;
    previousGuesses.clear();

    persistentCorrect = "_____";
    persistentWrong = "";

    wrongLettersRow->label("Not in word:");
    correctLettersRow->label("Correct letters:");
    misplacedRow->label("Misplaced:");
    errorRow->label("");

    for (int i = 0; i < 6; i++)
        guessRows[i]->label("-----");

    targetWord = words[rand() % words.size()];
    guessInputRow->value("");
    guessInputRow->take_focus();

    window->hide();
    gamewindow->show();
}

void Application::handleSCOREButtonClick(Widget *sender) {

    scoreSummary->label(
        "Score: " + to_string(totalScore) +
        " | Wins: " + to_string(wins) +
        " | Losses: " + to_string(losses)
    );

    string hist = "Previous games:\n";
    for (auto &r : gameHistory) {
        hist += r.word + " | Guesses: " + to_string(r.guesses)
              + " | Points: " + to_string(r.points)
              + (r.win ? " | Win\n" : " | Loss\n");
    }

    historyDisplay->label(hist);

    window->hide();
    scoreboard->show();
}

void Application::handleScoreBackButtonClick(Widget *sender) {
    scoreboard->hide();
    window->show();
}

void Application::handleWinBack(Widget *sender) {
    winWindow->hide();
    guessInputRow->value("");
    window->show();
}

void Application::handleLoseBack(Widget *sender) {
    loseWindow->hide();
    guessInputRow->value("");
    window->show();
}

// ----------------------------------------------------------
// GUESS HANDLING
void Application::handleGuessSubmit(Widget *sender) {

    string guess = guessInputRow->value();

    // lowercase input
    for (char &c : guess) c = tolower(c);

    // only letters allowed
    for (char c : guess) {
        if (!isalpha(c)) {
            errorRow->label("Use letters only.");
            return;
        }
    }

    if (guess.length() != 5) {
        errorRow->label("Guess must be 5 letters.");
        return;
    }

    for (auto &prev : previousGuesses) {
        if (prev == guess) {
            errorRow->label("Already guessed.");
            return;
        }
    }

    // CLEAR ERROR ROW (valid guess)
    errorRow->label("");

    previousGuesses.push_back(guess);
    guessCount++;

    string thisMisplaced = "";

    // --- WRONG / MISPLACED / CORRECT logic ---
    for (int i = 0; i < 5; i++) {
        char g = guess[i];
        char t = targetWord[i];

        // correct position
        if (g == t) {
            persistentCorrect[i] = g;
            continue;
        }

        // not in word at all
        if (targetWord.find(g) == string::npos) {
            if (persistentWrong.find(g) == string::npos)
                persistentWrong += g;
        }
        // in word but wrong position
        else {
            if (thisMisplaced.find(g) == string::npos)
                thisMisplaced += g;
        }
    }

    // Update UI
    wrongLettersRow->label("Not in word: " + persistentWrong);
    correctLettersRow->label("Correct letters: " + persistentCorrect);
    misplacedRow->label("Misplaced: " + thisMisplaced);

    guessRows[guessCount - 1]->label("Guess " + to_string(guessCount) + ": " + guess);

    guessInputRow->value("");
    guessInputRow->take_focus();

    // WIN CHECK
    if (guess == targetWord) {

        int points = calculatePoints(guessCount);
        totalScore += points;
        wins++;

        GameRecord rec { targetWord, guessCount, points, true };
        gameHistory.push_back(rec);
        saveRecord(rec);

        winMessage->label("YOU WIN! +" + to_string(points) + " points");

        gamewindow->hide();
        winWindow->show();
        return;
    }

    // LOSS CHECK
    if (guessCount >= 6) {

        losses++;

        GameRecord rec { targetWord, guessCount, 0, false };
        gameHistory.push_back(rec);
        saveRecord(rec);

        loseMessage->label("YOU LOSE! Word was: " + targetWord);

        gamewindow->hide();
        loseWindow->show();
        return;
    }
}
