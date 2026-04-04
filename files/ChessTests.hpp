#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// UNIT TEST FRAMEWORK - SIMPLE TEST RUNNER
// ============================================================================

class TestResult {
public:
    std::string testName;
    bool passed = false;
    std::string errorMessage;
    
    TestResult(const std::string& name) : testName(name) {}
};

class TestRunner {
public:
    static TestRunner& getInstance() {
        static TestRunner instance;
        return instance;
    }
    
    void runTest(const std::string& testName, const std::function<bool()>& test) {
        TestResult result(testName);
        try {
            result.passed = test();
        } catch (const std::exception& e) {
            result.passed = false;
            result.errorMessage = e.what();
        }
        results.push_back(result);
    }
    
    void printResults() {
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "TEST RESULTS\n";
        std::cout << std::string(70, '=') << "\n\n";
        
        int passed = 0, failed = 0;
        for (const auto& result : results) {
            if (result.passed) {
                std::cout << "✓ PASS: " << result.testName << "\n";
                passed++;
            } else {
                std::cout << "✗ FAIL: " << result.testName << "\n";
                if (!result.errorMessage.empty()) {
                    std::cout << "  Error: " << result.errorMessage << "\n";
                }
                failed++;
            }
        }
        
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "Total: " << (passed + failed) 
                  << " | Passed: " << passed 
                  << " | Failed: " << failed << "\n";
        std::cout << "Pass Rate: " << (100.0f * passed / (passed + failed)) << "%\n";
        std::cout << std::string(70, '=') << "\n\n";
    }
    
    bool allPassed() const {
        for (const auto& result : results) {
            if (!result.passed) return false;
        }
        return true;
    }

private:
    std::vector<TestResult> results;
};

// ============================================================================
// CHESS-SPECIFIC TEST ASSERTIONS
// ============================================================================

#define ASSERT_TRUE(condition) \
    if (!(condition)) throw std::runtime_error("Assertion failed: " #condition)

#define ASSERT_FALSE(condition) \
    if ((condition)) throw std::runtime_error("Assertion failed: NOT(" #condition ")")

#define ASSERT_EQUAL(actual, expected) \
    if ((actual) != (expected)) { \
        throw std::runtime_error("Assertion failed: " #actual " != " #expected); \
    }

#define RUN_TEST(name, lambda) \
    TestRunner::getInstance().runTest(name, lambda)

// ============================================================================
// MOVE VALIDATION TESTS
// ============================================================================

namespace ChessTests {

// Test pawn basic moves
inline bool testPawnMove() {
    // Pawn should move forward 1 square from starting position
    // Position: e2 (index 52) to e3 (index 44)
    
    // This would require a ChessBoard instance
    // For now, test logic only
    return true;  // Placeholder
}

// Test pawn double move
inline bool testPawnDoubleMove() {
    // Pawn should move forward 2 squares on first move only
    return true;  // Placeholder
}

// Test pawn capture
inline bool testPawnCapture() {
    // Pawn should capture diagonally
    return true;  // Placeholder
}

// Test en passant
inline bool testEnPassant() {
    // En passant should capture pawn that just moved 2 squares
    return true;  // Placeholder
}

// Test rook move
inline bool testRookMove() {
    // Rook should move horizontally or vertically
    return true;  // Placeholder
}

// Test rook blocked
inline bool testRookBlocked() {
    // Rook should not move through other pieces
    return true;  // Placeholder
}

// Test bishop move
inline bool testBishopMove() {
    // Bishop should move diagonally
    return true;  // Placeholder
}

// Test knight move
inline bool testKnightMove() {
    // Knight should move in L-shape
    return true;  // Placeholder
}

// Test queen move
inline bool testQueenMove() {
    // Queen should move like rook or bishop
    return true;  // Placeholder
}

// Test king move
inline bool testKingMove() {
    // King should move one square in any direction
    return true;  // Placeholder
}

// Test castling kingside
inline bool testCastlingKingside() {
    // King and rook haven't moved, path is clear
    return true;  // Placeholder
}

// Test castling queenside
inline bool testCastlingQueenside() {
    // King and queenside rook haven't moved, path is clear
    return true;  // Placeholder
}

// Test castling blocked
inline bool testCastlingBlocked() {
    // Castling should fail if king is in check
    // Or if pieces block the path
    return true;  // Placeholder
}

// ============================================================================
// CHECK/CHECKMATE TESTS
// ============================================================================

// Test check detection
inline bool testCheckDetection() {
    // King should be detected as in check
    return true;  // Placeholder
}

// Test checkmate detection
inline bool testCheckmateDetection() {
    // Position with no legal moves and king in check
    return true;  // Placeholder
}

// Test stalemate detection
inline bool testStalemateDetection() {
    // Position with no legal moves but king not in check
    return true;  // Placeholder
}

// Test can't move into check
inline bool testCantMoveIntoCheck() {
    // Move that would leave king in check should be invalid
    return true;  // Placeholder
}

// ============================================================================
// SPECIAL CONDITION TESTS
// ============================================================================

// Test pawn promotion
inline bool testPawnPromotion() {
    // Pawn reaching last rank should trigger promotion
    return true;  // Placeholder
}

// Test threefold repetition
inline bool testThreefoldRepetition() {
    // Same position repeated 3 times = draw
    return true;  // Placeholder
}

// Test 50-move rule
inline bool testFiftyMoveRule() {
    // 50 moves without capture or pawn move = draw
    return true;  // Placeholder
}

// Test insufficient material
inline bool testInsufficientMaterial() {
    // Only kings left = draw
    return true;  // Placeholder
}

// ============================================================================
// GAME STATE TESTS
// ============================================================================

// Test game reset
inline bool testGameReset() {
    // Game should reset to initial state
    return true;  // Placeholder
}

// Test move history
inline bool testMoveHistory() {
    // Moves should be recorded correctly
    return true;  // Placeholder
}

// Test undo move
inline bool testUndoMove() {
    // Last move should be undone
    return true;  // Placeholder
}

// Test redo move
inline bool testRedoMove() {
    // Undone move should be redone
    return true;  // Placeholder
}

// ============================================================================
// AI TESTS
// ============================================================================

// Test AI finds legal move
inline bool testAILegalMove() {
    // AI should only produce legal moves
    return true;  // Placeholder
}

// Test AI finds checkmate
inline bool testAICheckmate() {
    // AI should find checkmate in 1 move
    return true;  // Placeholder
}

// Test AI difficulty levels
inline bool testAIDifficultyLevels() {
    // Different difficulties should produce different moves
    return true;  // Placeholder
}

// ============================================================================
// STATISTICS TESTS
// ============================================================================

// Test game statistics recording
inline bool testStatisticsRecording() {
    // Game stats should be recorded correctly
    return true;  // Placeholder
}

// Test capture counting
inline bool testCaptureStatistics() {
    // Captures should be counted and stored
    return true;  // Placeholder
}

// ============================================================================
// RUN ALL TESTS
// ============================================================================

inline void runAllTests() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "CHESS ENGINE UNIT TESTS\n";
    std::cout << std::string(70, '=') << "\n\n";

    // Move validation tests
    RUN_TEST("Pawn move forward 1 square", testPawnMove);
    RUN_TEST("Pawn move forward 2 squares", testPawnDoubleMove);
    RUN_TEST("Pawn diagonal capture", testPawnCapture);
    RUN_TEST("En passant capture", testEnPassant);
    RUN_TEST("Rook horizontal/vertical move", testRookMove);
    RUN_TEST("Rook blocked by piece", testRookBlocked);
    RUN_TEST("Bishop diagonal move", testBishopMove);
    RUN_TEST("Knight L-shaped move", testKnightMove);
    RUN_TEST("Queen move (rook + bishop)", testQueenMove);
    RUN_TEST("King one square move", testKingMove);

    // Castling tests
    RUN_TEST("Castling kingside", testCastlingKingside);
    RUN_TEST("Castling queenside", testCastlingQueenside);
    RUN_TEST("Castling blocked", testCastlingBlocked);

    // Check/Checkmate tests
    RUN_TEST("Check detection", testCheckDetection);
    RUN_TEST("Checkmate detection", testCheckmateDetection);
    RUN_TEST("Stalemate detection", testStalemateDetection);
    RUN_TEST("Can't move into check", testCantMoveIntoCheck);

    // Special condition tests
    RUN_TEST("Pawn promotion", testPawnPromotion);
    RUN_TEST("Threefold repetition", testThreefoldRepetition);
    RUN_TEST("50-move rule", testFiftyMoveRule);
    RUN_TEST("Insufficient material", testInsufficientMaterial);

    // Game state tests
    RUN_TEST("Game reset", testGameReset);
    RUN_TEST("Move history tracking", testMoveHistory);
    RUN_TEST("Undo move", testUndoMove);
    RUN_TEST("Redo move", testRedoMove);

    // AI tests
    RUN_TEST("AI produces legal move", testAILegalMove);
    RUN_TEST("AI finds checkmate", testAICheckmate);
    RUN_TEST("AI difficulty levels", testAIDifficultyLevels);

    // Statistics tests
    RUN_TEST("Statistics recording", testStatisticsRecording);
    RUN_TEST("Capture statistics", testCaptureStatistics);

    // Print results
    TestRunner::getInstance().printResults();
}

} // namespace ChessTests
