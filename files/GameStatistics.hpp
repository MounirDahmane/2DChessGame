#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <map>

// ============================================================================
// GAME STATISTICS - TRACK GAME DATA FOR ANALYSIS
// ============================================================================

struct GameStatistics {
    // Basic game info
    std::string gameID;
    time_t startTime = 0;
    time_t endTime = 0;
    
    // Players
    std::string whitePlayer = "Player 1";
    std::string blackPlayer = "AI";
    
    // Game result
    enum Result { ONGOING, WHITE_WIN, BLACK_WIN, DRAW, ABANDONED };
    Result result = ONGOING;
    
    // Game duration (in seconds)
    float gameDuration = 0.0f;
    
    // Move statistics
    int totalMoves = 0;
    int whiteMoves = 0;
    int blackMoves = 0;
    float avgWhiteMoveTime = 0.0f;  // In seconds
    float avgBlackMoveTime = 0.0f;
    
    // Captures
    int whiteCaptureCount = 0;
    int blackCaptureCount = 0;
    int whitePointsCaptured = 0;  // Material value
    int blackPointsCaptured = 0;
    
    // Special moves
    int castlingCount = 0;
    int enPassantCount = 0;
    int pawnPromotionCount = 0;
    
    // Check/Checkmate
    int checkCount = 0;
    int movesSinceLastCapture = 0;  // For 50-move rule
    
    // Game conditions
    bool isCheckmate = false;
    bool isStaleMate = false;
    bool isThreefoldRepetition = false;
    bool isFiftyMoveRule = false;
    
    // Ratings (for tournaments)
    float whiteELOBefore = 0.0f;
    float whiteELOAfter = 0.0f;
    float blackELOBefore = 0.0f;
    float blackELOAfter = 0.0f;
    
    // Move history
    std::vector<std::string> moveHistory;
    
    // Constructor
    GameStatistics() {
        startTime = std::time(nullptr);
        gameID = std::to_string(startTime);
    }
    
    // Get game duration string
    std::string getDurationString() const {
        int hours = (int)gameDuration / 3600;
        int minutes = ((int)gameDuration % 3600) / 60;
        int seconds = (int)gameDuration % 60;
        
        std::string result;
        if (hours > 0) result += std::to_string(hours) + "h ";
        if (minutes > 0) result += std::to_string(minutes) + "m ";
        result += std::to_string(seconds) + "s";
        return result;
    }
    
    // Get result string
    std::string getResultString() const {
        switch (result) {
            case WHITE_WIN: return "White wins";
            case BLACK_WIN: return "Black wins";
            case DRAW: return "Draw";
            case ONGOING: return "Ongoing";
            case ABANDONED: return "Abandoned";
            default: return "Unknown";
        }
    }
    
    // Calculate accuracy (based on best moves from AI evaluation)
    float calculateAccuracy(int engineDepth = 4) const {
        // This would require comparing moves against engine evaluation
        // Placeholder for now
        return 0.0f;
    }
    
    // Get opening phase info (first 10 moves of each side)
    std::vector<std::string> getOpening() const {
        std::vector<std::string> opening;
        int moveCount = 0;
        for (const auto& move : moveHistory) {
            if (moveCount >= 20) break;  // First 10 full moves
            opening.push_back(move);
            moveCount++;
        }
        return opening;
    }
    
    // Finalize statistics after game ends
    void finalizeGame() {
        endTime = std::time(nullptr);
        gameDuration = static_cast<float>(endTime - startTime);
    }
};

// ============================================================================
// STATISTICS MANAGER - STORE AND RETRIEVE GAME DATA
// ============================================================================

class StatisticsManager {
public:
    // Add a completed game
    void addGame(const GameStatistics& stats) {
        games.push_back(stats);
    }
    
    // Get all games
    const std::vector<GameStatistics>& getAllGames() const {
        return games;
    }
    
    // Get win/loss statistics
    struct WinLossRecord {
        int wins = 0;
        int losses = 0;
        int draws = 0;
        
        int getTotalGames() const { return wins + losses + draws; }
        float getWinRate() const {
            if (getTotalGames() == 0) return 0.0f;
            return (100.0f * wins) / getTotalGames();
        }
    };
    
    WinLossRecord getWhiteRecord() const {
        WinLossRecord record;
        for (const auto& game : games) {
            if (game.result == GameStatistics::WHITE_WIN) record.wins++;
            else if (game.result == GameStatistics::BLACK_WIN) record.losses++;
            else if (game.result == GameStatistics::DRAW) record.draws++;
        }
        return record;
    }
    
    WinLossRecord getBlackRecord() const {
        WinLossRecord record;
        for (const auto& game : games) {
            if (game.result == GameStatistics::BLACK_WIN) record.wins++;
            else if (game.result == GameStatistics::WHITE_WIN) record.losses++;
            else if (game.result == GameStatistics::DRAW) record.draws++;
        }
        return record;
    }
    
    // Get average game duration
    float getAverageGameDuration() const {
        if (games.empty()) return 0.0f;
        float total = 0.0f;
        for (const auto& game : games) {
            total += game.gameDuration;
        }
        return total / games.size();
    }
    
    // Get favorite opening moves
    std::map<std::string, int> getFavoriteOpenings() const {
        std::map<std::string, int> openings;
        for (const auto& game : games) {
            if (!game.moveHistory.empty()) {
                std::string firstMove = game.moveHistory[0];
                openings[firstMove]++;
            }
        }
        return openings;
    }
    
    // Clear all statistics
    void clear() {
        games.clear();
    }

private:
    std::vector<GameStatistics> games;
};
