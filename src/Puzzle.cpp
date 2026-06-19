#include "Puzzle.h"

Puzzle::Puzzle(PuzzleType type,
               const std::string& description,
               const std::string& correctAnswer,
               int damagePenalty,
               const std::vector<PuzzleOption>& options)
    : type_(type), description_(description),
      correctAnswer_(correctAnswer), damagePenalty_(damagePenalty),
      options_(options) {}

PuzzleType Puzzle::getType() const { return type_; }
const std::string& Puzzle::getDescription() const { return description_; }
const std::string& Puzzle::getCorrectAnswer() const { return correctAnswer_; }
int Puzzle::getDamagePenalty() const { return damagePenalty_; }
const std::vector<PuzzleOption>& Puzzle::getOptions() const { return options_; }

bool Puzzle::checkAnswer(const std::string& answer) const {
    std::string a = answer, b = correctAnswer_;
    std::transform(a.begin(), a.end(), a.begin(), ::tolower);
    std::transform(b.begin(), b.end(), b.begin(), ::tolower);
    // trim
    auto trim = [](std::string& s){
        while (!s.empty() && std::isspace((unsigned char)s.front())) s.erase(s.begin());
        while (!s.empty() && std::isspace((unsigned char)s.back()))  s.pop_back();
    };
    trim(a); trim(b);
    return a == b;
}

void Puzzle::display() const {
    std::cout << "\n╔══════════════════════════════════╗\n";
    std::cout << "║           ГОЛОВОЛОМКА            ║\n";
    std::cout << "╚══════════════════════════════════╝\n";
    std::cout << description_ << "\n";
    if (!options_.empty()) {
        for (size_t i = 0; i < options_.size(); ++i) {
            std::cout << "  " << (i+1) << ") " << options_[i].text << "\n";
        }
        std::cout << "Введите номер варианта: ";
    } else {
        std::cout << "Ваш ответ: ";
    }
}
