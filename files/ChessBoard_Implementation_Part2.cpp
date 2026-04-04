// CONTINUATION OF ChessBoard_Enhanced.cpp
// PART 2: MOVE EXECUTION AND GAME RULES

// ============================================================================
// MOVE EXECUTION
// ============================================================================

void ChessBoard::executeMoveWithEffects(int from, int to) {
    int8_t piece = boardState[from];
    bool captureOccurred = (boardState[to] != Chess::EMPTY_SQUARE || to == enPassantIndex);

    // Handle captures and scoring
    if (boardState[to] != Chess::EMPTY_SQUARE) {
        handleCapture(to);
        movesSinceLastCaptureOrPawnMove = 0;
    }

    // Handle special moves
    if (to == enPassantIndex && std::abs(piece) == Chess::WHITE_PAWN) {
        handleEnPassantCapture(from, to);
        movesSinceLastCaptureOrPawnMove = 0;
    }

    if (std::abs(piece) == Chess::WHITE_KING && std::abs(to - from) == 2) {
        handleCastling(from, to);
    }

    // Track castling rights
    updateCastlingRights(from);
    updateEnPassantSquare(from, to, piece);
    updateMoveCounters(from, to);

    // Execute the move
    boardState[to] = piece;
    boardState[from] = Chess::EMPTY_SQUARE;

    // Play sounds and effects
    if (captureOccurred) {
        captureSound.play();
        triggerShake(1.2f, 0.1f);
        gameStats.whiteCaptureCount += isWhiteTurn ? 1 : 0;
        gameStats.blackCaptureCount += !isWhiteTurn ? 1 : 0;
    } else {
        moveSound.play();
    }

    // Check for pawn promotion
    if (std::abs(piece) == Chess::WHITE_PAWN) {
        if ((piece > 0 && to / 8 == 0) || (piece < 0 && to / 8 == 7)) {
            isPromoting = true;
            promotionIndex = to;
            addMoveToLog(from, to, piece);
            return;  // Don't switch turn yet
        }
    }

    // Switch turn and check for check/checkmate/stalemate
    isWhiteTurn = !isWhiteTurn;
    inCheck = isKingInCheck(isWhiteTurn);

    if (inCheck) {
        if (!hasLegalMoves(isWhiteTurn)) {
            isCheckmate = true;
            triggerShake(5.0f, 0.4f);
            gameStats.result = isWhiteTurn ? GameStatistics::BLACK_WIN : GameStatistics::WHITE_WIN;
        } else {
            checkSound.play();
            triggerShake(2.5f, 0.15f);
            gameStats.checkCount++;
        }
    } else if (!hasLegalMoves(isWhiteTurn)) {
        // Stalemate
        isStaleMate = true;
        triggerShake(2.0f, 0.2f);
        gameStats.result = GameStatistics::DRAW;
    }

    // Check for 50-move rule
    if (isFiftyMoveRuleApplicable()) {
        isStaleMate = true;
        gameStats.result = GameStatistics::DRAW;
        std::cout << "Draw by 50-move rule\n";
    }

    // Check for threefold repetition
    if (isThreefoldRepetition()) {
        isStaleMate = true;  // Treated as draw
        gameStats.result = GameStatistics::DRAW;
        gameStats.isThreefoldRepetition = true;
        std::cout << "Draw by threefold repetition\n";
    }

    syncPiecesWithBoard();
    addMoveToLog(from, to, piece);
    updateBoardHistory();
    gameStats.totalMoves++;
}

void ChessBoard::handleCapture(int target) {
    int8_t capturedPiece = boardState[target];
    int value = getPieceValue(capturedPiece);

    if (isWhite(capturedPiece)) {
        capturedByBlack.push_back(capturedPiece);
        blackScore += value;
        gameStats.blackPointsCaptured += value;
    } else {
        capturedByWhite.push_back(capturedPiece);
        whiteScore += value;
        gameStats.whitePointsCaptured += value;
    }
}

void ChessBoard::handleEnPassantCapture(int from, int to) {
    int victimIndex = (boardState[from] > 0) ? to + 8 : to - 8;
    int8_t capturedPiece = boardState[victimIndex];

    if (capturedPiece != Chess::EMPTY_SQUARE) {
        int value = getPieceValue(capturedPiece);
        if (isWhite(capturedPiece)) {
            capturedByBlack.push_back(capturedPiece);
            blackScore += value;
        } else {
            capturedByWhite.push_back(capturedPiece);
            whiteScore += value;
        }
        boardState[victimIndex] = Chess::EMPTY_SQUARE;
    }
    gameStats.enPassantCount++;
}

void ChessBoard::handleCastling(int from, int to) {
    if (to == 62) {  // White king-side
        boardState[61] = boardState[63];
        boardState[63] = Chess::EMPTY_SQUARE;
    } else if (to == 58) {  // White queen-side
        boardState[59] = boardState[56];
        boardState[56] = Chess::EMPTY_SQUARE;
    } else if (to == 6) {  // Black king-side
        boardState[5] = boardState[7];
        boardState[7] = Chess::EMPTY_SQUARE;
    } else if (to == 2) {  // Black queen-side
        boardState[3] = boardState[0];
        boardState[0] = Chess::EMPTY_SQUARE;
    }
    gameStats.castlingCount++;
}

void ChessBoard::updateCastlingRights(int from) {
    if (from == Chess::WHITE_KING_START) {
        whiteKingMoved = true;
    } else if (from == Chess::WHITE_ROOK_RIGHT_START) {
        whiteRookRightMoved = true;
    } else if (from == Chess::WHITE_ROOK_LEFT_START) {
        whiteRookLeftMoved = true;
    } else if (from == Chess::BLACK_KING_START) {
        blackKingMoved = true;
    } else if (from == Chess::BLACK_ROOK_RIGHT_START) {
        blackRookRightMoved = true;
    } else if (from == Chess::BLACK_ROOK_LEFT_START) {
        blackRookLeftMoved = true;
    }
}

void ChessBoard::updateEnPassantSquare(int from, int to, int8_t piece) {
    enPassantIndex = Chess::NO_VALID_INDEX;
    
    if (std::abs(piece) == Chess::WHITE_PAWN && std::abs(to - from) == 16) {
        enPassantIndex = (piece > 0) ? to + 8 : to - 8;
    }
}

void ChessBoard::updateMoveCounters(int from, int to) {
    int8_t piece = boardState[to];
    
    // Update 50-move rule counter
    if (std::abs(piece) == Chess::WHITE_PAWN) {
        movesSinceLastCaptureOrPawnMove = 0;
    } else {
        movesSinceLastCaptureOrPawnMove++;
    }
}

void ChessBoard::updateBoardHistory() {
    boardHistory.push_back(boardState);
    std::string position = getBoardPositionHash();
    boardPositionCount[position]++;
    
    // Keep only last 100 positions for threefold rep
    if (boardHistory.size() > 100) {
        boardHistory.pop_front();
    }
}

// ============================================================================
// MOVE VALIDATION
// ============================================================================

bool ChessBoard::validateMove(int from, int to, bool ignoreCheck) {
    if (from == to) return false;

    int8_t movingPiece = boardState[from];
    if (movingPiece == Chess::EMPTY_SQUARE) return false;
    if (isSameTeam(movingPiece, boardState[to])) return false;

    bool isValid = false;
    switch (std::abs(movingPiece)) {
        case Chess::WHITE_PAWN: isValid = validatePawn(from, to, movingPiece); break;
        case Chess::WHITE_BISHOP: isValid = validateBishop(from, to); break;
        case Chess::WHITE_KNIGHT: isValid = validateKnight(from, to); break;
        case Chess::WHITE_ROOK: isValid = validateRook(from, to); break;
        case Chess::WHITE_QUEEN: isValid = validateQueen(from, to); break;
        case Chess::WHITE_KING: isValid = validateKing(from, to); break;
    }

    if (isValid && !ignoreCheck) {
        if (simulateMoveAndCheck(from, to, isWhite(movingPiece))) {
            isValid = false;
        }
    }

    return isValid;
}

bool ChessBoard::isValidMove(int from, int to, bool ignoreCheck) const {
    // Const version for AI
    return const_cast<ChessBoard*>(this)->validateMove(from, to, ignoreCheck);
}

void ChessBoard::simulateMoveForAI(int from, int to) {
    // Make move on board copy (for AI evaluation)
    int8_t temp = boardState[to];
    boardState[to] = boardState[from];
    boardState[from] = Chess::EMPTY_SQUARE;
    
    // Update check status
    bool wasCheckmate = isCheckmate;
    bool wasStaleMate = isStaleMate;
    bool wasInCheck = inCheck;
    
    isCheckmate = false;
    isStaleMate = false;
    
    // Don't call syncPiecesWithBoard - AI doesn't need visuals
    // Don't update game stats - this is temporary
}

bool ChessBoard::validatePawn(int from, int to, int8_t piece) {
    int fromCol = from % 8;
    int fromRow = from / 8;
    int toCol = to % 8;
    int toRow = to / 8;
    int direction = (piece > 0) ? -1 : 1;

    // Forward one square
    if (fromCol == toCol && toRow == fromRow + direction && 
        boardState[to] == Chess::EMPTY_SQUARE) {
        return true;
    }

    // Forward two squares from start
    if (fromCol == toCol && 
        ((piece > 0 && fromRow == 6) || (piece < 0 && fromRow == 1)) &&
        toRow == fromRow + 2 * direction &&
        boardState[to] == Chess::EMPTY_SQUARE &&
        boardState[fromRow + direction * 8 + fromCol] == Chess::EMPTY_SQUARE) {
        return true;
    }

    // Capture diagonally
    if (std::abs(fromCol - toCol) == 1 && toRow == fromRow + direction &&
        (boardState[to] != Chess::EMPTY_SQUARE || to == enPassantIndex)) {
        return true;
    }

    return false;
}

bool ChessBoard::validateKing(int from, int to) {
    int fromCol = from % 8;
    int fromRow = from / 8;
    int toCol = to % 8;
    int toRow = to / 8;
    
    int dx = std::abs(fromCol - toCol);
    int dy = std::abs(fromRow - toRow);

    // Normal king move
    if (dx <= 1 && dy <= 1 && !(dx == 0 && dy == 0)) {
        return true;
    }

    // Castling
    if (dy == 0 && dx == 2 && !inCheck) {
        if (isWhiteTurn) {
            if (!whiteKingMoved) {
                if (to == 62 && !whiteRookRightMoved && isPathClear(from, 63)) {
                    return true;
                }
                if (to == 58 && !whiteRookLeftMoved && isPathClear(from, 56)) {
                    return true;
                }
            }
        } else {
            if (!blackKingMoved) {
                if (to == 6 && !blackRookRightMoved && isPathClear(from, 7)) {
                    return true;
                }
                if (to == 2 && !blackRookLeftMoved && isPathClear(from, 0)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool ChessBoard::simulateMoveAndCheck(int from, int to, bool white) {
    int8_t originalFrom = boardState[from];
    int8_t originalTo = boardState[to];
    
    boardState[to] = boardState[from];
    boardState[from] = Chess::EMPTY_SQUARE;
    
    bool inCheck = isKingInCheck(white);
    
    boardState[from] = originalFrom;
    boardState[to] = originalTo;
    
    return inCheck;
}

bool ChessBoard::isKingInCheck(bool white) {
    int kingPiece = white ? Chess::WHITE_KING : Chess::BLACK_KING;
    int kingPos = Chess::NO_VALID_INDEX;
    
    for (int i = 0; i < Chess::BOARD_SQUARES; ++i) {
        if (boardState[i] == kingPiece) {
            kingPos = i;
            break;
        }
    }

    if (kingPos == Chess::NO_VALID_INDEX) return false;

    for (int i = 0; i < Chess::BOARD_SQUARES; ++i) {
        if (boardState[i] != Chess::EMPTY_SQUARE && isWhite(boardState[i]) != white) {
            if (validateMove(i, kingPos, true)) {
                return true;
            }
        }
    }

    return false;
}

bool ChessBoard::hasLegalMoves(bool white) {
    for (int from = 0; from < Chess::BOARD_SQUARES; ++from) {
        if (boardState[from] != Chess::EMPTY_SQUARE && 
            isWhite(boardState[from]) == white) {
            for (int to = 0; to < Chess::BOARD_SQUARES; ++to) {
                if (validateMove(from, to, false)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ChessBoard::isPathClear(int from, int to) {
    int fromCol = from % 8;
    int fromRow = from / 8;
    int toCol = to % 8;
    int toRow = to / 8;

    int dCol = (toCol > fromCol) ? 1 : (toCol < fromCol ? -1 : 0);
    int dRow = (toRow > fromRow) ? 1 : (toRow < fromRow ? -1 : 0);

    int currentCol = fromCol + dCol;
    int currentRow = fromRow + dRow;

    while (currentCol != toCol || currentRow != toRow) {
        if (boardState[currentRow * 8 + currentCol] != Chess::EMPTY_SQUARE) {
            return false;
        }
        currentCol += dCol;
        currentRow += dRow;
    }

    return true;
}

bool ChessBoard::validateRook(int from, int to) {
    return (from % 8 == to % 8 || from / 8 == to / 8) && isPathClear(from, to);
}

bool ChessBoard::validateBishop(int from, int to) {
    return std::abs(from % 8 - to % 8) == std::abs(from / 8 - to / 8) && 
           isPathClear(from, to);
}

bool ChessBoard::validateKnight(int from, int to) {
    int dx = std::abs(from % 8 - to % 8);
    int dy = std::abs(from / 8 - to / 8);
    return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
}

bool ChessBoard::validateQueen(int from, int to) {
    return validateRook(from, to) || validateBishop(from, to);
}

void ChessBoard::calculatePossibleMoves(int from) {
    possibleMoves.clear();
    for (int to = 0; to < Chess::BOARD_SQUARES; ++to) {
        if (validateMove(from, to, false)) {
            possibleMoves.push_back({to, (boardState[to] != Chess::EMPTY_SQUARE || 
                                          to == enPassantIndex)});
        }
    }
}

bool ChessBoard::isThreefoldRepetition() const {
    if (boardPositionCount.empty()) return false;
    
    std::string currentPosition = 
        const_cast<ChessBoard*>(this)->getBoardPositionHash();
    
    return boardPositionCount.at(currentPosition) >= Chess::THREEFOLD_REPETITION_LIMIT;
}

bool ChessBoard::isFiftyMoveRuleApplicable() const {
    return movesSinceLastCaptureOrPawnMove >= Chess::FIFTY_MOVE_RULE_LIMIT;
}

void ChessBoard::checkForGameEnd() {
    if (isThreefoldRepetition()) {
        isStaleMate = true;
        gameStats.isThreefoldRepetition = true;
    }
    if (isFiftyMoveRuleApplicable()) {
        isStaleMate = true;
        gameStats.isFiftyMoveRule = true;
    }
}

// Continued in next part...
