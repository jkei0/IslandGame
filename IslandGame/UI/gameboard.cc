#include "gameboard.hh"
#include "mainwindow.hh"
#include "hexgraphics.hh"
#include "igamerunner.hh"
#include "actor.hh"
#include "transport.hh"
#include "hex.hh"
#include "pawngraphics.hh"


namespace Student {

GameBoard::GameBoard()
{

}

GameBoard::~GameBoard()
{

}

int Student::GameBoard::checkTileOccupation
(Common::CubeCoordinate tileCoord) const
{
    auto hexPtr = getHex(tileCoord);

    // jos hexaa ei ole
    if ( hexPtr == nullptr) {
        return -1;
    }

    return hexPtr->getPawnAmount();
}

bool GameBoard::isWaterTile(Common::CubeCoordinate tileCoord) const
{
    //Tarkastaa onko valittu hex water tyyppiä

    auto ptr = getHex(tileCoord); //pointteri hexaan

    // jos hexaa ei ole ollenkaan palautetaan nullptr
    if (ptr == nullptr) {
        return false;
    }
    else if ( ptr->isWaterTile()) {
        return true;
    }
    else {
        return false;
    }

}

std::shared_ptr<Common::Hex> GameBoard::getHex
(Common::CubeCoordinate hexCoord) const
{
    // Palauttaa pointterin hexaan

    auto iter = hexMap_.find(hexCoord);
    if ( iter != hexMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

void GameBoard::addPawn(int playerId, int pawnId)
{
    // Lisää pawnin gameboardiin, jos ei tiedetä sen koordinaatteja

    Common::Pawn newPawn;
    std::shared_ptr<Common::Pawn> pawn =
            std::make_shared<Common::Pawn>(newPawn);
    pawn->setId(pawnId, playerId);
    pawnMap_.insert(std::pair<int, std::shared_ptr<Common::Pawn>>
                    (pawnId, pawn));
}

void GameBoard::addPawn(int playerId, int pawnId, Common::CubeCoordinate coord)
{
    // Lisää pawnin gameboardiin, kun tiedetään koordinaatit
    Common::Pawn newPawn(pawnId, playerId, coord);
    std::shared_ptr<Common::Pawn> pawn =
            std::make_shared<Common::Pawn>(newPawn);
    auto iter = hexMap_.find(coord);
    std::shared_ptr<Common::Hex> hexPtr = iter->second;

    hexPtr->addPawn(pawn);
    pawnMap_.insert(std::pair<int, std::shared_ptr<Common::Pawn>>
                    (pawnId, pawn));
}

void GameBoard::movePawn(int pawnId, Common::CubeCoordinate pawnCoord)
{
    //Vaihtaa pawnin paikkaa pelilaudalla
    auto hex = hexMap_.find(pawnCoord); //pawnin uusi paikka
    auto pawn = pawnMap_.find(pawnId);

    // jos uutta hexaa ei ole laudalla ei tehdä mitään
    if ( hex == hexMap_.end() ) {
        return;
    }

    // Pawnin entinen sijainti
    auto currCoord = pawn->second->getCoordinates();
    auto currHex = hexMap_.find(currCoord);

    // Poistetaan pawn entisesta hexasta ja lisätään
    // uuteen hexaan
    currHex->second->removePawn(pawn->second);
    hex->second->addPawn(pawn->second);
    pawn->second->setCoordinates(pawnCoord);
}

void GameBoard::removePawn(int pawnId)
{
    auto iter = pawnMap_.find(pawnId);
    auto pawnPtr = iter->second; // pointteri pawniin

    // Pawnin coordinaatit
    Common::CubeCoordinate pawnCoords = pawnPtr->getCoordinates();

    // Hexa jossa pawn on
    std::shared_ptr<Common::Hex> targetHex = getHex(pawnCoords);
    targetHex->removePawn(pawnPtr);
    pawnMap_.erase(iter);
    pawnPtr = nullptr;
}


void GameBoard::addActor(std::shared_ptr<Common::Actor> actor,
                         Common::CubeCoordinate actorCoord)
{
    // Haetaan actorin kordinaatit
    auto iter = hexMap_.find(actorCoord);
    std::shared_ptr<Common::Hex> hexPtr = iter->second;

    // Lisätään actori hexiin
    hexPtr->addActor(actor);
    // Ja lisätään actori actorMap_iin
    actorMap_.insert(std::pair<int, Common::CubeCoordinate>(actor->getId(),
                                                            actorCoord));

    // Lisätään myös actorille hexi
    actor->addHex(hexPtr);
}


void GameBoard::moveActor(int actorId, Common::CubeCoordinate actorCoord)
{
    auto hex = hexMap_.find(actorCoord);

    // Jos uutta hexiä ei ole olemassa, ei tehdä mitään
    if (hex == hexMap_.end()) {
        return;
    }

    // Haetaan actorille nykyisen hexin pointteri
    Common::CubeCoordinate currCoord = actorMap_.at(actorId);
    std::shared_ptr<Common::Hex> curHexPtr = hexMap_.at(currCoord);

    // Poistetaan actori nykyisestä hexasta
    std::shared_ptr<Common::Actor> actorPtr = curHexPtr->giveActor(actorId);
    actorPtr->getHex()->removeActor(actorPtr);
    //curHexPtr->removeActor(actorPtr);


    // Lisätään actori uuteen hexaan
    //hexMap_.at(actorCoord)->addActor(actorPtr);
    hex->second->addActor(actorPtr);

    // Päivitetään actormappiin oikeat kordinaatit
    actorMap_.at(actorId) = actorCoord;

    actorPtr->move(hex->second);

}

void GameBoard::removeActor(int actorId)
{
    // Haetaan actorin hexapointteri
    Common::CubeCoordinate actorcoord = actorMap_.at(actorId);
    std::shared_ptr<Common::Hex> hexPtr = hexMap_.at(actorcoord);

    // Selvitetään actorin pointteri
    std::shared_ptr<Common::Actor> actorPtr = hexPtr->giveActor(actorId);

    // Poistetaan actori hexasta
    hexPtr->removeActor(actorPtr);

    // Ja poistetaan actori actorMapista.
    actorMap_.erase(actorId);
}

void GameBoard::addHex(std::shared_ptr<Common::Hex> newHex)
{
    // Haetaan lisättävän hexan koordinaatit
    Common::CubeCoordinate coord = newHex->getCoordinates();

    // Tutkitaan löytyykö onko hexa luotu jo aiemmin
    auto iter = hexMap_.find(coord);
    if (iter != hexMap_.end()) {
        hexMap_.erase(iter);
        hexMap_.insert(std::pair<Common::CubeCoordinate,
                       std::shared_ptr<Common::Hex>>(coord, newHex));
    }

    // Lisätään hexa pelilaudalle
    else {
        hexMap_.insert(std::pair<Common::CubeCoordinate,
                    std::shared_ptr<Common::Hex>>(coord, newHex));
    }

}

void GameBoard::addTransport(std::shared_ptr<Common::Transport> transport,
                             Common::CubeCoordinate coord)
{
    // Haetaan pointteri oikeaan hexaan
    auto iter = hexMap_.find(coord);
    std::shared_ptr<Common::Hex> hexPtr = iter->second;
    // asetetaan transportille hexpointteri
    transport->addHex(hexPtr);

    // Lisätään transportti hexiin
    hexPtr->addTransport(transport);
    // Lisätään transportti transportMap_iin
    transportMap_.insert(std::pair<int, Common::CubeCoordinate>(
                             transport->getId(),coord));
}


void GameBoard::moveTransport(int id, Common::CubeCoordinate coord)
{
    auto hex = hexMap_.find(coord);  // Transportin uusi paikka

    // Jos uutta hexiä ei ole olemassa, ei tehdä mitään
    if (hex == hexMap_.end()) {
        return;
    }

    // Haetaan transportille nykyisen hexin pointteri
    auto currCoord = transportMap_.at(id);
    auto currHexPtr = hexMap_.at(currCoord);

    // Poistetaan transportti nykyisestä hexasta
    std::shared_ptr<Common::Transport> transportPtr =
            currHexPtr->giveTransport(id);
    currHexPtr->removeTransport(transportPtr);

    // Lisätään transportpointeri uuteen hexaan
    hexMap_.at(coord)->addTransport(transportPtr);
    // Lisätään transportille uusi hexapointteri
    transportPtr->addHex(currHexPtr);

    // Päivitetään transportmapiin oikeat kordinaatit kyseiselle transportille.
    transportMap_.at(id) = coord;
}

void GameBoard::removeTransport(int id)
{
    // Haetaan transportin hexapointteri
    Common::CubeCoordinate transcoord = transportMap_.at(id);
    std::shared_ptr<Common::Hex> hexPtr = hexMap_.at(transcoord);

    // Selvitetään transportin pointteri
    std::shared_ptr<Common::Transport> transportPtr = hexPtr->giveTransport(id);

    // Ja poistetaan transportti hexasta
    hexPtr->removeTransport(transportPtr);

    // Ja poistetaan transportti transportVectorista
    transportMap_.erase(id);
}

bool GameBoard::checkPawn(int pawnId)
{
    auto iter = pawnMap_.find(pawnId);
    if(iter == pawnMap_.end()) {
        return false;
    }
    return true;
}

}
