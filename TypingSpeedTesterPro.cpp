/*
 * ============================================================
 *         TYPING SPEED TESTER PRO
 *         A Console-Based Typing Speed Application
 *         Language : C++
 *         Author   : [Your Name]
 *         Version  : 1.0
 * ============================================================
 *
 * PROJECT EXPLANATION:
 * --------------------
 * This project measures how fast and accurately a user can type
 * a given paragraph. The user selects a difficulty level, and a
 * random paragraph is shown. After typing, the program calculates
 * WPM (Words Per Minute) and Accuracy %, then saves the score to
 * a leaderboard file.
 *
 * HOW WPM IS CALCULATED:
 * ----------------------
 *   WPM = (Total Characters Typed / 5) / Time in Minutes
 *   The standard "word" in typing tests = 5 characters.
 *   Example: If you type 250 chars in 1 min => 250/5 = 50 WPM
 *
 * HOW ACCURACY IS CALCULATED:
 * ----------------------------
 *   Accuracy = (Correct Characters / Total Characters) * 100
 *   We compare each character in user input vs. the original text.
 *
 * FEATURES USED:
 * --------------
 *   - Vectors           : Store paragraphs and leaderboard entries
 *   - Strings           : Input handling and text comparison
 *   - File Handling     : Save/load leaderboard from "leaderboard.txt"
 *   - Functions         : Modular design for each feature
 *   - Chrono Library    : Precise timer for WPM calculation
 *   - ANSI Colors       : Colored console output (works on most terminals)
 *   - Random            : Randomly pick paragraphs
 *   - Sorting           : Leaderboard sorted by WPM (descending)
 * ============================================================
 */

// ─── Standard Library Includes ───────────────────────────────
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <thread>
#include <sstream>

using namespace std;
using namespace chrono;

// ─── ANSI Color Codes (for colored terminal output) ──────────
// These work in VS Code terminal, Linux, macOS, and modern Windows terminals.
// If colors don't show on your system, the text still appears — just without color.
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BG_BLUE "\033[44m"
#define BG_GREEN "\033[42m"
#define BG_RED "\033[41m"
#define BRIGHT_CYAN "\033[96m"
#define BRIGHT_YEL "\033[93m"
#define BRIGHT_GRN "\033[92m"
#define BRIGHT_MAG "\033[95m"

// ─── Leaderboard File Name ────────────────────────────────────
const string LEADERBOARD_FILE = "leaderboard.txt";

// ─── Leaderboard Entry Structure ─────────────────────────────
struct LeaderboardEntry
{
    string name;       // Player name
    double wpm;        // Words per minute
    double accuracy;   // Accuracy percentage
    string difficulty; // Easy / Medium / Hard
    string date;       // Date of attempt
};

// ============================================================
//  PARAGRAPH BANKS (organized by difficulty)
// ============================================================

// Easy paragraphs — short, simple words
vector<string> easyParagraphs = {
    "The sun is bright today. The birds are singing in the trees. The sky is clear and blue. It is a nice day to go outside and play.",
    "I like to read books every night before I sleep. Books take me to new worlds and teach me new things. Reading is a great habit.",
    "My dog loves to run in the park. He wags his tail when he sees other dogs. We go for walks every morning together.",
    "She drinks a cup of tea every morning. It helps her feel calm and ready for the day. Tea is her favorite drink.",
    "The small cat sat on the mat. It looked at the bird outside the window. The bird flew away and the cat went back to sleep."};

// Medium paragraphs — moderate length, general knowledge
vector<string> mediumParagraphs = {
    "Technology has transformed the way we communicate and work. Smartphones keep us connected to people around the world. The internet has made information accessible to everyone, changing education and business forever.",
    "Exercise is essential for a healthy lifestyle. Regular physical activity strengthens muscles, improves mood, and reduces the risk of chronic diseases. Even a thirty-minute walk each day can make a significant difference.",
    "Cooking at home is a valuable skill that saves money and promotes healthy eating. Learning to prepare simple meals helps people understand nutrition and develop creativity in the kitchen.",
    "Reading newspapers and books regularly expands vocabulary and improves concentration. It also keeps the mind sharp and helps people stay informed about events happening around the world.",
    "Climate change is one of the biggest challenges facing humanity today. Rising temperatures are affecting weather patterns, wildlife habitats, and sea levels across the entire planet."};

// Hard paragraphs — longer sentences, complex vocabulary
vector<string> hardParagraphs = {
    "The proliferation of artificial intelligence technologies across various industries has fundamentally disrupted traditional business models, necessitating organizations to recalibrate their operational strategies and workforce competencies to remain competitive in an increasingly automated economic landscape.",
    "Quantum computing represents a paradigmatic shift in computational capability, leveraging the principles of superposition and entanglement to perform complex calculations exponentially faster than conventional binary systems, potentially revolutionizing cryptography, pharmaceutical research, and financial modeling.",
    "The philosophical examination of consciousness remains one of the most perplexing inquiries in both neuroscience and metaphysics, as scholars continue to debate whether subjective experience can ultimately be reduced to neurochemical processes or whether it constitutes an irreducible phenomenon transcending physicalist explanations.",
    "Blockchain technology, originally conceived as the underlying infrastructure for decentralized cryptocurrency transactions, has since found extensive applications in supply chain management, digital identity verification, smart contract execution, and transparent governance frameworks across public and private sectors.",
    "The intricate interplay between genetic predisposition and environmental epigenetic factors continues to challenge researchers attempting to delineate the precise mechanisms through which hereditary information influences phenotypic expression and susceptibility to multifactorial disorders in heterogeneous human populations."};

// ============================================================
//  UTILITY FUNCTIONS
// ============================================================

// Clear the console screen (cross-platform)
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Get current date as a string (e.g., "2025-07-15")
string getCurrentDate()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    ostringstream oss;
    oss << (1900 + ltm->tm_year) << "-"
        << setw(2) << setfill('0') << (1 + ltm->tm_mon) << "-"
        << setw(2) << setfill('0') << ltm->tm_mday;
    return oss.str();
}

// Print a horizontal divider line
void printDivider(char ch = '=', int width = 60)
{
    cout << CYAN;
    for (int i = 0; i < width; i++)
        cout << ch;
    cout << RESET << "\n";
}

// Center-align text within a given width
void printCentered(const string &text, int width = 60)
{
    int padding = (width - (int)text.length()) / 2;
    if (padding > 0)
        cout << string(padding, ' ');
    cout << text << "\n";
}

// Loading animation — gives the app a professional feel
void loadingAnimation(const string &message, int steps = 15)
{
    cout << YELLOW << message;
    cout.flush();
    for (int i = 0; i < steps; i++)
    {
        cout << ".";
        cout.flush();
        // Cross-platform sleep: 80 milliseconds
        for (int delay = 0; delay < 100000000; delay++)
        {
        }
    }
    cout << RESET << "\n";
}

// Pause and wait for the user to press Enter
void pressEnterToContinue()
{
    cout << YELLOW << "\n  Press ENTER to continue..." << RESET;
    cin.ignore();
    cin.get();
}

// ============================================================
//  WELCOME SCREEN
// ============================================================
void showWelcomeScreen()
{
    clearScreen();
    cout << "\n";
    printDivider('*');
    cout << BOLD << BRIGHT_CYAN;
    printCentered("  ████████╗██╗   ██╗██████╗ ██╗███╗   ██╗ ██████╗  ");
    printCentered("     ██╔══╝╚██╗ ██╔╝██╔══██╗██║████╗  ██║██╔════╝  ");
    printCentered("     ██║    ╚████╔╝ ██████╔╝██║██╔██╗ ██║██║  ███╗ ");
    printCentered("     ██║     ╚██╔╝  ██╔═══╝ ██║██║╚██╗██║██║   ██║ ");
    printCentered("     ██║      ██║   ██║      ██║██║ ╚████║╚██████╔╝ ");
    printCentered("     ╚═╝      ╚═╝   ╚═╝      ╚═╝╚═╝  ╚═══╝ ╚═════╝  ");
    cout << RESET;
    cout << "\n";
    cout << BOLD << BRIGHT_YEL;
    printCentered("  ███████╗██████╗ ███████╗███████╗██████╗ ");
    printCentered("  ██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗");
    printCentered("  ███████╗██████╔╝█████╗  █████╗  ██║  ██║");
    printCentered("  ╚════██║██╔═══╝ ██╔══╝  ██╔══╝  ██║  ██║");
    printCentered("  ███████║██║     ███████╗███████╗██████╔╝");
    printCentered("  ╚══════╝╚═╝     ╚══════╝╚══════╝╚═════╝ ");
    cout << RESET;
    cout << "\n";
    cout << BOLD << BRIGHT_MAG;
    printCentered("  ████████╗███████╗███████╗████████╗███████╗██████╗ ");
    printCentered("     ██╔══╝██╔════╝██╔════╝╚══██╔══╝██╔════╝██╔══██╗");
    printCentered("     ██║   █████╗  ███████╗   ██║   █████╗  ██████╔╝");
    printCentered("     ██║   ██╔══╝  ╚════██║   ██║   ██╔══╝  ██╔══██╗");
    printCentered("     ██║   ███████╗███████║   ██║   ███████╗██║  ██║");
    printCentered("     ╚═╝   ╚══════╝╚══════╝   ╚═╝   ╚══════╝╚═╝  ╚═╝");
    cout << RESET << "\n";
    printDivider('*');
    cout << "\n";
    cout << BRIGHT_GRN;
    printCentered("[ P R O ]   v1.0  —  Console Typing Speed Analyzer");
    cout << RESET;
    printCentered("  Measure • Improve • Dominate Your Typing Skills  ");
    cout << "\n";
    printDivider('-');
    cout << GREEN << "  Built with C++ | File Handling | Vectors | Chrono\n"
         << RESET;
    printDivider('-');
    cout << "\n";

    loadingAnimation("  Initializing application", 12);
    cout << BRIGHT_GRN << "  ✔  System Ready!\n"
         << RESET;
    pressEnterToContinue();
}

// ============================================================
//  MAIN MENU
// ============================================================
int showMainMenu()
{
    clearScreen();
    cout << "\n";
    printDivider();
    cout << BOLD << BRIGHT_CYAN;
    printCentered("     TYPING SPEED TESTER PRO  —  MAIN MENU     ");
    cout << RESET;
    printDivider();
    cout << "\n";
    cout << BRIGHT_YEL << "   ┌─────────────────────────────────────────┐\n";
    cout << "   │                                         │\n";
    cout << "   │   " << BRIGHT_GRN << "[1]  ▶  Start Typing Test            " << BRIGHT_YEL << "│\n";
    cout << "   │                                         │\n";
    cout << "   │   " << CYAN << "[2]  🏆  View Leaderboard             " << BRIGHT_YEL << "│\n";
    cout << "   │                                         │\n";
    cout << "   │   " << RED << "[3]  ✖  Exit Application              " << BRIGHT_YEL << "│\n";
    cout << "   │                                         │\n";
    cout << "   └─────────────────────────────────────────┘\n";
    cout << RESET << "\n";
    printDivider('-');
    cout << BRIGHT_YEL << "  Enter your choice [1-3]: " << RESET;

    int choice;
    // Input validation loop
    while (!(cin >> choice) || choice < 1 || choice > 3)
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << RED << "  ✘ Invalid input! Please enter 1, 2, or 3: " << RESET;
    }
    cin.ignore(); // flush leftover newline
    return choice;
}

// ============================================================
//  DIFFICULTY SELECTION MENU
// ============================================================
int selectDifficulty()
{
    clearScreen();
    cout << "\n";
    printDivider();
    cout << BOLD << BRIGHT_CYAN;
    printCentered("       SELECT DIFFICULTY LEVEL        ");
    cout << RESET;
    printDivider();
    cout << "\n";
    cout << "   " << BRIGHT_GRN << "[1]  🟢  EASY    — Short, simple sentences\n";
    cout << "                   Ideal for beginners\n\n";
    cout << "   " << BRIGHT_YEL << "[2]  🟡  MEDIUM  — Moderate length paragraphs\n";
    cout << "                   Good for daily practice\n\n";
    cout << "   " << RED << "[3]  🔴  HARD    — Complex vocabulary & long text\n";
    cout << "                   Challenge yourself!\n\n";
    cout << RESET;
    printDivider('-');
    cout << BRIGHT_YEL << "  Enter difficulty [1-3]: " << RESET;

    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 3)
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << RED << "  ✘ Invalid! Enter 1, 2, or 3: " << RESET;
    }
    cin.ignore();
    return choice;
}

// ============================================================
//  RANDOM PARAGRAPH PICKER
// ============================================================
// Returns a random paragraph string based on difficulty choice.
// Difficulty: 1 = Easy, 2 = Medium, 3 = Hard
string getRandomParagraph(int difficulty)
{
    // Seed random with current time for true randomness
    srand((unsigned int)time(0));

    if (difficulty == 1)
    {
        int idx = rand() % easyParagraphs.size();
        return easyParagraphs[idx];
    }
    else if (difficulty == 2)
    {
        int idx = rand() % mediumParagraphs.size();
        return mediumParagraphs[idx];
    }
    else
    {
        int idx = rand() % hardParagraphs.size();
        return hardParagraphs[idx];
    }
}

// ============================================================
//  DISPLAY PARAGRAPH TO USER
// ============================================================
void displayParagraph(const string &para, const string &difficultyName)
{
    clearScreen();
    cout << "\n";
    printDivider();
    cout << BOLD << BRIGHT_CYAN;
    printCentered("         TYPING TEST — " + difficultyName + " MODE          ");
    cout << RESET;
    printDivider();
    cout << "\n";
    cout << YELLOW << "  Instructions:\n"
         << RESET;
    cout << "  1. Read the paragraph below carefully.\n";
    cout << "  2. Press ENTER when you're ready to start.\n";
    cout << "  3. Type the paragraph as fast and accurately as possible.\n";
    cout << "  4. Press ENTER when done.\n\n";
    printDivider('-');
    cout << "\n";
    cout << BRIGHT_GRN << "  PARAGRAPH TO TYPE:\n\n"
         << RESET;
    // Word-wrap the paragraph at ~58 chars for readability
    int lineLen = 0;
    cout << "  ";
    for (char c : para)
    {
        cout << BOLD << WHITE << c << RESET;
        lineLen++;
        if (lineLen >= 58 && c == ' ')
        {
            cout << "\n  ";
            lineLen = 0;
        }
    }
    cout << "\n\n";
    printDivider('-');
    cout << BRIGHT_YEL << "\n  Press ENTER when you are READY to start typing...\n"
         << RESET;
    cin.get(); // Wait for user to press Enter
}

// ============================================================
//  COMPARE INPUT — Character-by-character analysis
// ============================================================
// Returns: number of correct characters
int compareStrings(const string &original, const string &typed,
                   int &correctChars, int &wrongChars)
{
    correctChars = 0;
    wrongChars = 0;
    int minLen = min(original.length(), typed.length());

    for (int i = 0; i < minLen; i++)
    {
        if (original[i] == typed[i])
        {
            correctChars++;
        }
        else
        {
            wrongChars++;
        }
    }
    // Characters not typed (if user typed less than original)
    if (typed.length() < original.length())
    {
        wrongChars += (int)(original.length() - typed.length());
    }
    return correctChars;
}

// ============================================================
//  CALCULATE WPM
// ============================================================
// Standard formula: (characters typed / 5) / minutes elapsed
double calculateWPM(int totalCharsTyped, double seconds)
{
    if (seconds <= 0)
        return 0.0;
    double minutes = seconds / 60.0;
    double words = totalCharsTyped / 5.0;
    return words / minutes;
}

// ============================================================
//  CALCULATE ACCURACY
// ============================================================
double calculateAccuracy(int correctChars, int totalOriginalChars)
{
    if (totalOriginalChars <= 0)
        return 0.0;
    return ((double)correctChars / totalOriginalChars) * 100.0;
}

// ============================================================
//  PERFORMANCE GRADE (bonus feature)
// ============================================================
string getGrade(double wpm, double accuracy)
{
    if (accuracy >= 98 && wpm >= 80)
        return "S  [LEGENDARY TYPIST]";
    if (accuracy >= 95 && wpm >= 60)
        return "A  [EXPERT]";
    if (accuracy >= 90 && wpm >= 40)
        return "B  [PROFICIENT]";
    if (accuracy >= 80 && wpm >= 25)
        return "C  [AVERAGE]";
    return "D  [NEEDS PRACTICE]";
}

// ============================================================
//  DISPLAY FINAL REPORT
// ============================================================
void displayReport(const string &playerName, double wpm, double accuracy,
                   int correctChars, int wrongChars, double timeTaken,
                   const string &difficultyName)
{
    clearScreen();
    cout << "\n";
    printDivider('=');
    cout << BOLD << BRIGHT_YEL;
    printCentered("       PERFORMANCE REPORT        ");
    cout << RESET;
    printDivider('=');
    cout << "\n";

    // Player info
    cout << CYAN << "  Player    : " << RESET << BOLD << playerName << "\n"
         << RESET;
    cout << CYAN << "  Difficulty: " << RESET << difficultyName << "\n";
    cout << CYAN << "  Date      : " << RESET << getCurrentDate() << "\n\n";

    printDivider('-');

    // Timing
    cout << YELLOW << "  ⏱  Time Taken   : " << RESET
         << fixed << setprecision(2) << timeTaken << " seconds\n\n";

    // Speed
    cout << BRIGHT_GRN << "  ⚡ Typing Speed : " << RESET
         << BOLD << fixed << setprecision(1) << wpm << " WPM" << RESET
         << "  (Words Per Minute)\n\n";

    // Accuracy
    if (accuracy >= 90)
        cout << BRIGHT_GRN;
    else if (accuracy >= 70)
        cout << YELLOW;
    else
        cout << RED;
    cout << "  🎯 Accuracy     : " << RESET
         << BOLD << fixed << setprecision(1) << accuracy << "%" << RESET << "\n\n";

    // Character breakdown
    cout << BRIGHT_GRN << "  ✔  Correct Chars: " << RESET
         << BOLD << GREEN << correctChars << RESET << "\n";
    cout << RED << "  ✘  Wrong Chars  : " << RESET
         << BOLD << RED << wrongChars << RESET << "\n\n";

    printDivider('-');

    // Grade
    string grade = getGrade(wpm, accuracy);
    cout << BOLD << BRIGHT_MAG << "  🏅 Grade : " << grade << "\n"
         << RESET;

    printDivider('=');
    cout << "\n";
}

// ============================================================
//  LEADERBOARD — SAVE SCORE TO FILE
// ============================================================
void saveToLeaderboard(const LeaderboardEntry &entry)
{
    // Open file in append mode so existing scores are preserved
    ofstream file(LEADERBOARD_FILE, ios::app);
    if (file.is_open())
    {
        // Format: name|wpm|accuracy|difficulty|date
        file << entry.name << "|"
             << fixed << setprecision(2) << entry.wpm << "|"
             << fixed << setprecision(2) << entry.accuracy << "|"
             << entry.difficulty << "|"
             << entry.date << "\n";
        file.close();
    }
    else
    {
        cout << RED << "  ✘ Could not save to leaderboard file.\n"
             << RESET;
    }
}

// ============================================================
//  LEADERBOARD — LOAD SCORES FROM FILE
// ============================================================
vector<LeaderboardEntry> loadLeaderboard()
{
    vector<LeaderboardEntry> entries;
    ifstream file(LEADERBOARD_FILE);

    if (!file.is_open())
    {
        // No leaderboard file yet — that's okay
        return entries;
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        // Parse the pipe-delimited line
        LeaderboardEntry e;
        stringstream ss(line);
        string token;

        getline(ss, e.name, '|');
        getline(ss, token, '|');
        e.wpm = stod(token);
        getline(ss, token, '|');
        e.accuracy = stod(token);
        getline(ss, e.difficulty, '|');
        getline(ss, e.date, '|');

        entries.push_back(e);
    }
    file.close();

    // Sort by WPM descending (best scores first)
    sort(entries.begin(), entries.end(),
         [](const LeaderboardEntry &a, const LeaderboardEntry &b)
         {
             return a.wpm > b.wpm;
         });

    return entries;
}

// ============================================================
//  DISPLAY LEADERBOARD
// ============================================================
void showLeaderboard()
{
    clearScreen();
    vector<LeaderboardEntry> entries = loadLeaderboard();

    cout << "\n";
    printDivider('=');
    cout << BOLD << BRIGHT_YEL;
    printCentered("     🏆  TOP SCORES LEADERBOARD  🏆     ");
    cout << RESET;
    printDivider('=');
    cout << "\n";

    if (entries.empty())
    {
        cout << YELLOW;
        printCentered("No scores recorded yet.");
        printCentered("Complete a typing test to appear here!");
        cout << RESET << "\n";
    }
    else
    {
        // Table header
        cout << BOLD << CYAN
             << "  #   Name              WPM     Accuracy  Level    Date\n"
             << RESET;
        printDivider('-');

        // Show top 10 entries
        int limit = min((int)entries.size(), 10);
        for (int i = 0; i < limit; i++)
        {
            // Medal emoji for top 3
            string medal = "   ";
            if (i == 0)
                medal = BRIGHT_YEL + string(" 🥇") + RESET;
            else if (i == 1)
                medal = WHITE + string(" 🥈") + RESET;
            else if (i == 2)
                medal = YELLOW + string(" 🥉") + RESET;

            cout << medal << " ";
            cout << BOLD << setw(2) << (i + 1) << RESET << "  ";
            cout << left << setw(18) << entries[i].name;
            cout << BRIGHT_GRN << right << setw(6)
                 << fixed << setprecision(1) << entries[i].wpm << " WPM  " << RESET;
            cout << CYAN << setw(7)
                 << fixed << setprecision(1) << entries[i].accuracy << "%  " << RESET;
            cout << left << setw(9) << entries[i].difficulty;
            cout << entries[i].date << "\n";
        }

        printDivider('-');
        cout << YELLOW << "  Total entries: " << entries.size() << "\n"
             << RESET;
    }

    cout << "\n";
    printDivider('=');
    pressEnterToContinue();
}

// ============================================================
//  GET PLAYER NAME (with validation)
// ============================================================
string getPlayerName()
{
    string name;
    cout << "\n"
         << BRIGHT_YEL << "  Enter your name (max 15 chars): " << RESET;
    getline(cin, name);

    // Trim whitespace
    if (name.empty())
        name = "Player";
    if (name.length() > 15)
        name = name.substr(0, 15);

    return name;
}

// ============================================================
//  CORE TYPING TEST FLOW
// ============================================================
void startTypingTest()
{
    // Step 1: Get player name
    clearScreen();
    cout << "\n";
    printDivider();
    cout << BOLD << BRIGHT_CYAN;
    printCentered("        NEW TYPING TEST         ");
    cout << RESET;
    printDivider();
    string playerName = getPlayerName();

    // Step 2: Select difficulty
    int diffChoice = selectDifficulty();
    string difficultyName;
    if (diffChoice == 1)
        difficultyName = "EASY";
    else if (diffChoice == 2)
        difficultyName = "MEDIUM";
    else
        difficultyName = "HARD";

    // Step 3: Get a random paragraph
    string paragraph = getRandomParagraph(diffChoice);

    // Step 4: Show the paragraph and wait for ready signal
    displayParagraph(paragraph, difficultyName);

    // Step 5: Start the timer and capture input
    clearScreen();
    cout << "\n";
    printDivider();
    cout << BOLD << BRIGHT_GRN;
    printCentered("  ⌨  START TYPING NOW!  ⌨  ");
    cout << RESET;
    printDivider();
    cout << "\n  " << YELLOW << paragraph << RESET << "\n\n";
    printDivider('-');
    cout << BRIGHT_GRN << "\n  Your Input:\n  " << RESET;

    // Record start time
    auto startTime = high_resolution_clock::now();

    // Get full-line input from user
    string userInput;
    getline(cin, userInput);

    // Record end time
    auto endTime = high_resolution_clock::now();

    // Calculate time taken in seconds
    double timeTaken = duration<double>(endTime - startTime).count();

    // Step 6: Analyze results
    int correctChars = 0, wrongChars = 0;
    compareStrings(paragraph, userInput, correctChars, wrongChars);

    double wpm = calculateWPM((int)userInput.length(), timeTaken);
    double accuracy = calculateAccuracy(correctChars, (int)paragraph.length());

    // Step 7: Show loading animation before report
    cout << "\n";
    loadingAnimation("  Analyzing your performance", 10);

    // Step 8: Display the performance report
    displayReport(playerName, wpm, accuracy, correctChars, wrongChars,
                  timeTaken, difficultyName);

    // Step 9: Save score to leaderboard
    LeaderboardEntry entry;
    entry.name = playerName;
    entry.wpm = wpm;
    entry.accuracy = accuracy;
    entry.difficulty = difficultyName;
    entry.date = getCurrentDate();

    saveToLeaderboard(entry);
    cout << BRIGHT_GRN << "  ✔  Score saved to leaderboard!\n"
         << RESET;

    pressEnterToContinue();
}

// ============================================================
//  EXIT SCREEN
// ============================================================
void showExitScreen()
{
    clearScreen();
    cout << "\n\n";
    printDivider('*');
    cout << BOLD << BRIGHT_CYAN;
    printCentered("   Thank you for using Typing Speed Tester Pro!  ");
    cout << RESET;
    cout << "\n";
    printCentered("  Keep practicing — speed and accuracy improve");
    printCentered("  with consistent daily effort. Happy typing!  ");
    cout << "\n";
    cout << BRIGHT_YEL;
    printCentered("  Pro Tip: The average typist types 40 WPM.   ");
    printCentered("  Professionals reach 70-100+ WPM. Aim high!  ");
    cout << RESET << "\n";
    printDivider('*');
    cout << "\n";

    loadingAnimation("  Closing application", 8);
    cout << BRIGHT_GRN << "  ✔  Goodbye!\n\n"
         << RESET;
}

// ============================================================
//  MAIN FUNCTION — Entry point and main loop
// ============================================================
int main()
{
    // Show the welcome splash screen once at startup
    showWelcomeScreen();

    // Main menu loop
    bool running = true;
    while (running)
    {
        int choice = showMainMenu();

        switch (choice)
        {
        case 1:
            startTypingTest();
            break;
        case 2:
            showLeaderboard();
            break;
        case 3:
            running = false;
            break;
        }
    }

    // Show goodbye screen
    showExitScreen();
    return 0;
}

/*
 * ============================================================
 *  END OF FILE — TypingSpeedTesterPro.cpp
 * ============================================================
 *
 *  HOW TO COMPILE & RUN:
 *  ─────────────────────
 *  Option A — Command Line (g++):
 *    g++ -std=c++11 -o TypingSpeedTesterPro TypingSpeedTesterPro.cpp
 *    ./TypingSpeedTesterPro         (Linux/macOS)
 *    TypingSpeedTesterPro.exe       (Windows)
 *
 *  Option B — VS Code:
 *    Open folder → Select file → Press Ctrl+Shift+B to build
 *    Then run in integrated terminal.
 *
 *  Option C — Dev-C++ / CodeBlocks:
 *    File → New → Source File → Paste code → Compile & Run (F9 / F11)
 *
 *  NOTE ON COLORS:
 *  ───────────────
 *  Colors use ANSI escape codes and work in:
 *    ✔ VS Code Terminal
 *    ✔ Linux / macOS Terminal
 *    ✔ Windows Terminal (Win 10+)
 *  They may not show in older Windows CMD. All text still visible.
 *
 *  LEADERBOARD FILE:
 *  ─────────────────
 *  Scores are saved to "leaderboard.txt" in the same folder
 *  as the executable. The file is created automatically.
 *
 * ============================================================
 */
