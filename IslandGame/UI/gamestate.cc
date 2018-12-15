#include "gamestate.hh"
#include "player.hh"


namespace Student {

GameState::GameState()
{

}

GameState::~GameState()
{

}

Common::GamePhase GameState::currentGamePhase() const
{
    return gamePhaseId_;
}

int GameState::currentPlayer() const
{
    return playerInTurn_;
}

void GameState::changeGamePhase(Common::GamePhase nextPhase)
{
    gamePhaseId_ = nextPhase;
}

void GameState::changePlayerTurn(int nextPlayer)
{
    playerInTurn_ = nextPlayer;
}

int GameState::getPawnPerPlayer()
{
    return pawnPerPlayer_;
}

void GameState::addPointsToPlayer(int playerId, int points)
{
    for (auto iter = playerPointVector_.begin();
         iter != playerPointVector_.end(); ++iter){
        if ( iter->first == playerId) {
            iter->second = iter->second + points;
            return;
        }
    }
}

void GameState::deletePawn(int pawnId)
{
    for (auto iter = playerPawnVector_.begin();
         iter != playerPawnVector_.end(); ++iter) {
        if (iter->second == pawnId) {
            playerPawnVector_.erase(iter);
            return;
        }
    }
}

int GameState::playerPawnsLeft(int playerId)
{
    int nappuloitaJaljella = 0;
    for (auto iter = playerPawnVector_.begin();
         iter != playerPawnVector_.end(); iter++) {
        if (iter->first == playerId) {
            ++nappuloitaJaljella;
        }
    }
    return nappuloitaJaljella;
}

void GameState::addPawn(int playerId, int pawnId)
{
    playerPawnVector_.push_back(std::pair<int,int>(playerId, pawnId));
}

void GameState::initializePlayers(std::vector<std::shared_ptr<Common::IPlayer>>
                                  playerVector)
{
    // Lisätään pelaajat vektoriin
    for( auto iter = playerVector.begin(); iter != playerVector.end(); iter++) {
        playerPointVector_.push_back
                (std::pair<int,int>((*iter)->getPlayerId(),0));
    }
}

int GameState::getPoints(int playerId)
{
    for (auto iter = playerPointVector_.begin();
         iter != playerPointVector_.end(); ++iter) {
        if (iter->first == playerId) {
            return iter->second;
        }
    }
    return -1;
}
}

