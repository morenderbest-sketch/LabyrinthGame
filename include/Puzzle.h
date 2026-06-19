#pragma once
#include "Common.h"

struct PuzzleOption {
    std::string text;
    std::string answer; // correct answer token
};

class Puzzle {
public:
    Puzzle(PuzzleType type,
           const std::string& description,
           const std::string& correctAnswer,
           int damagePenalty,
           const std::vector<PuzzleOption>& options = {});

    PuzzleType getType() const;
    const std::string& getDescription() const;
    const std::string& getCorrectAnswer() const;
    int getDamagePenalty() const;
    const std::vector<PuzzleOption>& getOptions() const;

    bool checkAnswer(const std::string& answer) const;
    void display() const;

private:
    PuzzleType type_;
    std::string description_;
    std::string correctAnswer_;
    int damagePenalty_;
    std::vector<PuzzleOption> options_;
};
