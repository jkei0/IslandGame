#include "hexgraphics.hh"
#include "coordinates.hh"
#include "gameboard.hh"
#include "illegalmoveexception.hh"
#include "actor.hh"
#include "transport.hh"
#include "actorgraphics.hh"
#include "transportgraphics.hh"
#include "actor.hh"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <vector>


// Lisätty testausta varten, poista ennen palautusta.
#include "iostream"

namespace Student {

hexGraphics* hexGraphics::activeHex_ = nullptr;
int hexGraphics::phase_ = 1;
bool hexGraphics::isRightClicked_ = false;

hexGraphics::hexGraphics(std::shared_ptr<Common::Hex> hex,
                         std::shared_ptr<Common::IGameRunner> runner,
                         std::vector<std::shared_ptr<Common::IPlayer> > playerVector,
                         CustomView *view, std::shared_ptr<Common::IGameState> statePtr,
                         std::shared_ptr<Common::IGameBoard> boardPtr,
                         QGraphicsItem *parent) :
    QGraphicsItem(parent),
    hex_(hex),
    runner_(runner),
    boardPtr_(boardPtr),
    playerVector_(playerVector),
    statePtr_(statePtr),
    view_(view)
{
    actor_ = "";  // alustetaan fliptilen actormuuttuja tyhjäksi.

}

hexGraphics::~hexGraphics()
{

}

void hexGraphics::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::black); // Asettaa kynän värin

    // haetaan pelilaudan layout
    Layout layout = getLayout();

    // Haetaan hexagonin kärkipisteiden QKoordinaatit
    Common::CubeCoordinate coords = hex_->getCoordinates();
    QPolygonF poly = polygon_corners(layout, coords);

    // Värjätään hexagon riippuen sen tyypistä
    if ( hex_->getPieceType() == "Water") {
        painter->setBrush(Qt::blue);
    }
    else if (hex_->getPieceType() == "Beach") {
        painter->setBrush(Qt::yellow);
    }
    else if (hex_->getPieceType() == "Coral") {
        painter->setBrush(Qt::darkBlue);
    }
    else if (hex_->getPieceType() == "Forest") {
        painter->setBrush(Qt::green);
    }
    else if (hex_->getPieceType() == "Mountain") {
        painter->setBrush(Qt::gray);
    }
    else {
        painter->setBrush(Qt::darkGray);
    }

    if ( phase_ == 2 ) {
        QPen pen(QBrush(Qt::black),3);
        painter->setPen(pen);
    }

    if (isRightClicked_) {
        QPen pen(QBrush(Qt::red), 4);
        painter->setPen(pen);
    }

    // Piirretään hexagon pelilaudalle
    painter->drawPolygon(poly);
}


QRectF hexGraphics::boundingRect() const
{
    int pituus = 45;
    int leveys = 40;
    int sade = 20;
    int pystysade = 22;

    Layout layout = getLayout();
    QPointF keskipiste = hex_to_pixel(layout, hex_->getCoordinates());
    return QRect(keskipiste.x()-sade,keskipiste.y()-
                 pystysade,leveys,pituus);
}


void hexGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Tapahtuma eventissä riippuu missä gamephasessa ollaan.
    // 1. GamePhase:MOVEMENT => siirrellään nappuja
    // 2. GamePhase:SINKING => upotetaan saaripaloja
    // 3. GamePhase:SPINNING => siirretään määrättyä actoria/transporttia

    // 1.    
    Common::GamePhase peliVaihde = runner_->currentGamePhase();

    if (peliVaihde == Common::GamePhase::MOVEMENT) {
        // Jos klikataan oikea hiirennappulaa, siirretään mahdollisesti transportteja
        if (event->button() == Qt::RightButton) {
            // Tarkistetaan, onko hexassa venettä, tai delfiiniä
            if (getHex()->getTransports().size() == 0) {
                return;
            } else if (getHex()->getTransports().at(0)->getPawnsInTransport().size() == 0) {
                return;
            } else if (!getHex()->getTransports().at(0)->canMove(statePtr_->currentPlayer())) {
                return;
            } else {
                isRightClicked_ = true;
                activeHex_ = this;
                phase_ = 2;
                update();
            }

        } else if (hexGraphics::isRightClicked_ && phase_ == 2) {
            moveTransWActors();
            return;
        } else {
            movePlayerPawns();
        }

    // 2
    } else if (peliVaihde == Common::GamePhase::SINKING) {
        sinkingFlipTile();

    } else if (peliVaihde == Common::GamePhase::SPINNING) {
        // Selvitetään ensin, mikä on spinwheelistä saatu tieto
        // Haetaan kiekkotiedot Mainwindowista
        std::pair<std::string, std::string>  kiekkoTiedot =
                MainWindow::getKiekkoTiedot();

        // Jos kiekossa oli actortyyppinen olio, liikutetaan actoreita sen mukaan
        if (kiekkoTiedot.first == "shark" || kiekkoTiedot.first ==
                "seamunster"
                || kiekkoTiedot.first == "kraken") {
            moveActorSpinningPhase(kiekkoTiedot);
        }
        // Jos kiekossa oli dolphin olio, liikutetaan sitä jos se on ilman pelaajaa
        if (kiekkoTiedot.first == "dolphin") {
            moveDolphinSpinningPhase(kiekkoTiedot);
        }
    }
}



void hexGraphics::movePlayerPawns()
{
    if (isRightClicked_) {
        return;
    }

    if(hexGraphics::phase_ == 1 and getHex()->getPawnAmount() > 0) {
        if (getPawn() == nullptr) {
            return;
        }
        // jätetään muistiin hexa, josta nappula liikutetaan
        activeHex_ = this;
        phase_ = 2; // "Aktivoidaan" hexa
        update(); // Piirretään hex uudelleen

    // klikataan hexaa, jossa ei ole omia nappuloita
    } else if (hexGraphics::phase_==1) {
        return;

    // edellinen hex on aktivoitu, yritetään liikuttaa nappulaa
    // uuteen hexaan
    } else {
        auto originHex = activeHex_; //"Aktivoitu" hexa
        activeHex_ = nullptr;
        phase_ = 1;
        originHex->update();

        // Haetaan pawn jota halutaan liikuttaa
        auto pawn = originHex->getPawn();

        // Tarkastetaan onko liike mahdollinen
        if ( runner_->checkPawnMovement(originHex->getHex()->getCoordinates(),
                                        getHex()->getCoordinates(),
                                        pawn->getId()) >= 0) {

            // Jos pawni oli transportissa, niin poistetaan se sieltä
            if (originHex->getHex()->getTransports().size() > 0) {
                if (originHex->getHex()->getTransports().at(0)->isPawnInTransport(pawn)) {
                    originHex->getHex()->getTransports().at(0)->removePawn(pawn);
                }
            // Jos pawni liikku hexaan jossa on transportti, se lisätään siihen
            } else if (getHex()->getTransports().size() > 0) {
                getHex()->getTransports().at(0)->addPawn(pawn);
            }

            // Liikutetaan pawn ja asetetaan vuorossa olevalle pelaajalle
            // oikea määrä liikkeitä
            getPlayer()->setActionsLeft(runner_->movePawn(
                        originHex->getHex()->getCoordinates(),
                        getHex()->getCoordinates(), pawn->getId()));

            // Haetaan graafinen esitys nappulalle
            auto pawnGraph = getPawnGraphics(pawn->getId());

            // Tarkastetaan liikkuiko pawn maalihexaan
            if (checkGoalHex()) {
                // Haetaan pointteri itse tehtyyn gamestate luokkaan
                std::shared_ptr<GameState> state =
                        std::dynamic_pointer_cast<GameState>(statePtr_);

                // Annetaan pelaajalle pisteitä
                int pisteet = 100;
                state->addPointsToPlayer(statePtr_->currentPlayer(), pisteet);

                // Poistetaan pawn pelistä
                boardPtr_->removePawn(pawnGraph->getId());
                state->deletePawn(pawnGraph->getId());
                view_->scene()->removeItem(pawnGraph);
                return;
            }

            // Liikutetaan grafiikka uuteen paikkaan
            pawnGraph->animatePosition(getHex()->getPawnAmount());
        }
    }
}


void hexGraphics::moveTransWActors()
{
    isRightClicked_ = false;
    phase_ = 1;
    auto originHex = activeHex_;
    activeHex_->update();
    activeHex_ = nullptr;

    if (originHex->getHex()->getTransports().size() <= 0) {
        return;
    }

    Common::CubeCoordinate thisHex = originHex->getHex()->getCoordinates();
    Common::CubeCoordinate nextHex = getHex()->getCoordinates();

    // Jos klikataan samaa hexiä, niin ei tapahdu mitään
    if (originHex->getHex() == getHex()) {
        return;
    }

    std::shared_ptr<Common::Transport> transToMove =
            originHex->getHex()->getTransports().at(0);

    std::string moves = std::to_string(originHex->getPlayer()->
                                       getActionsLeft());

    if (runner_->checkTransportMovement(thisHex, nextHex,
                                        transToMove->getId(),
                                           moves) <= 0) {
        return;
    }

    /* -------------------------------------------------------------------------
    // Jos runnerin kautta yrittää liikuttaa transporttia, niin vene liikkuu, ja
    // jäljellä olevien liikkeiden määrä päivittyy oikeaksi. Mutta kyydissä olevat
    // pawnit ei liiku mukana.
    // runner_->moveTransport(thisHex, nextHex, transToMove->getId());
    // -------------------------------------------------------------------------
    */

    // Jos käyttää transportin omaa metodia liikkumiseen, niin pawnit pysyy mukana,
    // mutta jäljellä olevien liikkeiden määrä ei päivity oikeaksi. Joten asetetaan
    // pelaajalle oikea määrä liikkeitä jäljelle:
    originHex->getPlayer()->setActionsLeft(runner_->checkTransportMovement(thisHex, nextHex, transToMove->getId(),moves));
    transToMove->move(getHex());


    // Päivitetään jäljellä olevat liikkeet.
    int movesLeft = runner_->
            checkTransportMovement(thisHex, nextHex,
                                   transToMove->getId(), moves);
    moves = std::to_string(movesLeft);

    // Päivitetään transportin grafiikka
    auto transGraph = getTransportGraphics(transToMove->getId());
    transGraph->updatePos(nextHex);

    // Liikutetaan myös pawnit ja päivitetään niiden grafiikka
    auto pawnVec = transToMove->getPawnsInTransport();
    int i = 1;
    for (auto pit = pawnVec.begin(); pit != pawnVec.end(); ++pit) {
        auto pawnGraph = getPawnGraphics((*pit)->getId());
        pawnGraph->updatePos(i);
        i++;
    }
}

void hexGraphics::sinkingFlipTile()
{
    // Käännetään hexagoni mikäli se on sallittua
    try {
        actor_ = runner_->flipTile(hex_->getCoordinates());  // Käännetään hexagoni
        update();  // Päivitetään hexagonin grafiikka
    } catch (Common::IllegalMoveException) {
        //throw;
        return;
    }

    // Jos flipTilen seurauksena tulee actor tyyppinen olio...
    if ((actor_ == "shark") || (actor_ == "vortex") ||
            (actor_ == "kraken") || (actor_ == "seamunster")) {
        // .. niin piirretään actori pelilaudalle ja..
        if (this->getHex()->getActors().size() == 1) {
            ActorGraphics* newActor =
                    new ActorGraphics(this->getHex()->getActors().at(0),
                                      this->getHex()->getCoordinates());
            view_->scene()->addItem(newActor);

            newActor->updatePos(hex_->getCoordinates());
        }

        // .. ja actori tekee tuhojaan
        getHex()->getActors().at(0)->doAction();

        // Päivitetään grafiikka
        doActionCleanHex(actor_);

    // Tai jos fliptilestä tulee transport-tyyppinen olio.
    } else if (actor_ == "boat" || actor_ == "dolphin") {
        TransportGraphics* newTransport =
                new TransportGraphics(this->getHex()->getTransports().at(0),
                                      this->getHex()->getCoordinates());
        view_->scene()->addItem(newTransport);
        newTransport->updatePos(getHex()->getCoordinates());
    }
    emit vaihdaPeliTilaa();
}

void hexGraphics::moveActorSpinningPhase
(std::pair<std::string, std::string> kiekkoTiedot)
{
    // Klikataan hexaa jossa on oikeanlainen actori
    if((hexGraphics::phase_ == 1) && (getHex()->getActors().size() > 0)) {

        // Jos ei löydy oikea actoria kyseisellä hexillä, palataan.
        if (getActorPtr(getHex(), kiekkoTiedot.first) == nullptr) {
            return;
        }
        activeHex_ = this;
        phase_ = 2;
        update();
    } else if (hexGraphics::phase_== 1) {
        return;
    } else {
        auto originHex = activeHex_;
        activeHex_ = nullptr;
        phase_ = 1;
        originHex->update();

        // Haetaan cordinaatit nykyiseen ja tulevaan paikkaan
        Common::CubeCoordinate currentHex =
                originHex->getHex()->getCoordinates();
        Common::CubeCoordinate nextHex = getHex()->getCoordinates();

        // Haetaan actorin pointteri
        std::shared_ptr<Common::Actor> actorToMove =
                getActorPtr(originHex->getHex(), kiekkoTiedot.first);

        // Siirretään actori mikäli sallittu, ja päivitetään grafiikka
        if (runner_->checkActorMovement(currentHex, nextHex,
                                        actorToMove->getId(),
                                        kiekkoTiedot.second)) {
            runner_->moveActor(currentHex, nextHex, actorToMove->getId(),
                               kiekkoTiedot.second);

            // Siirretään actorin grafiikka
            auto actorGraph = getActorGraphics(actorToMove->getId());
            actorGraph->updatePos(nextHex);

            // Actori tekee tuhojaan uudessa paikassaan
            actorToMove->doAction();
            // Siivotaan hexi ja mahdollisesti ympärillä olevat hexit
            doActionCleanHex(actorToMove->getActorType());
            emit deletePawnsFromCube(nextHex);

            emit vaihdaPeliTilaa();
        }
    }
}

void hexGraphics::moveDolphinSpinningPhase
(std::pair<std::string, std::string> kiekkoTiedot)
{
    // Klikataan hexaa jossa on dolphin
    if(hexGraphics::phase_ == 1 && getHex()->getTransports().size() > 0) {

        // jos ei löydy dolphinia, palataan
        if (getTransPtr(getHex(), kiekkoTiedot.first) == nullptr) {
            return;
        // Tai jos dolphinin selässä on jonkun pelinappula
        } else if (getTransPtr(getHex(), kiekkoTiedot.first)->
                   getCapacity() != 1) {
            return;
        }
        activeHex_ = this;
        phase_ = 2;
        update();
    } else if (hexGraphics::phase_ == 1) {
        return;
    } else {
        auto originHex = activeHex_;
        activeHex_ = nullptr;
        phase_ = 1;
        originHex->update();

        // cordinaatit vanhaan ja uuteen paikkaan
        Common::CubeCoordinate currentHex =
                originHex->getHex()->getCoordinates();
        Common::CubeCoordinate nextHex = getHex()->getCoordinates();

        // Haetaan dolphinin pointteri
        auto dolphinPtr = getTransPtr(originHex->getHex(), kiekkoTiedot.first);

        // Tarkistetaan onko sallittu liike
        if (runner_->checkTransportMovement(currentHex, nextHex,
                                            dolphinPtr->getId(),
                                            kiekkoTiedot.second) == 0) {
            runner_->moveTransportWithSpinner(currentHex, nextHex,
                                              dolphinPtr->getId(),
                                              kiekkoTiedot.second);
            auto transGraph = getTransportGraphics(dolphinPtr->getId());
            transGraph->updatePos(nextHex);
            emit vaihdaPeliTilaa();
        }
    }
}

void hexGraphics::doActionCleanHex(std::string actor)
{
    // Poistetaan pawnit
    Common::CubeCoordinate coords = hex_->getCoordinates();

    if (actor == "shark") {
        emit deletePawnsFromCube(coords);
    }
    else if (actor == "vortex") {
        auto vector = hex_->getNeighbourVector();
        emit deletePawnsFromCube(coords);
        emit deleteTransportsFromCube(coords);
        emit deleteActorsFromCube(coords);
        for(auto iter = vector.begin(); iter != vector.end(); iter++) {
            if(boardPtr_->getHex(*iter)->getPieceType() == "Water") {
                emit deletePawnsFromCube(*iter);
                emit deleteTransportsFromCube(*iter);
                emit deleteActorsFromCube(*iter);
            }
        }
        // Tuhoa actorit
    }
    else if (actor == "kraken") {
        emit deletePawnsFromCube(coords);
    }
    else if (actor == "seamunster") {
        emit deleteTransportsFromCube(coords);
        emit deletePawnsFromCube(coords);
    }
}

std::shared_ptr<Common::Actor> hexGraphics::getActorPtr(std::shared_ptr<Common::Hex> hexPtr,
                                                        std::string kiekkoActor)
{
    auto actorVec = hexPtr->getActors();
    for (auto ait = actorVec.begin(); ait != actorVec.end(); ait++) {
        if ((*ait)->getActorType() == kiekkoActor) {
            return (*ait);
        }
    }
    return nullptr;
}

std::shared_ptr<Common::Transport> hexGraphics::getTransPtr(std::shared_ptr<Common::Hex> hexPtr,
                                                            std::string kiekkoTrans)
{
    auto transpVec = hexPtr->getTransports();
    for (auto tit = transpVec.begin(); tit != transpVec.end(); tit++) {
        if ((*tit)->getTransportType() == kiekkoTrans) {
            return (*tit);
        }
    }
    return nullptr;
}


std::shared_ptr<Common::Hex> hexGraphics::getHex()
{
    return hex_;
}


pawnGraphics *hexGraphics::getPawnGraphics(int pawnId)
{
    auto pawnVec = pawnGraphics::getPawnVector();
    for( auto it = pawnVec.begin(); it != pawnVec.end(); it++) {
        if ((*it)->getId() == pawnId) {
            return *it;
        }
    }
    return nullptr;
}


std::shared_ptr<Common::Pawn> hexGraphics::getPawn()
{
    auto vec = getHex()->getPawns();
    for(auto iter = vec.begin(); iter != vec.end(); iter++) {
        if( (*iter)->getPlayerId() == runner_->currentPlayer() ) {
            return (*iter);
        }
    }
    return nullptr;
}


std::shared_ptr<Common::IPlayer> hexGraphics::getPlayer()
{
    for( auto iter = playerVector_.begin(); iter != playerVector_.end(); iter++) {
        if ( (*iter)->getPlayerId() == runner_->currentPlayer()) {
            return *iter;
        }
    }
    return nullptr;
}

ActorGraphics *hexGraphics::getActorGraphics(int actorId)
{
    auto actorVec = ActorGraphics::getActorGraphVector();
    for (auto it = actorVec.begin(); it != actorVec.end(); it++) {
        if ((*it)->getActorId() == actorId) {
            return (*it);
        }
    }
    return nullptr;
}


TransportGraphics *hexGraphics::getTransportGraphics(int transportId)
{
    auto transVec = TransportGraphics::getTransportVector();
    for (auto it = transVec.begin(); it != transVec.end(); it++) {
        if ((*it)->getTransportId() == transportId) {
            return *it;
        }
    }
    return nullptr;
}

std::string hexGraphics::getFlipTileActor()
{
    return actor_;
}


bool hexGraphics::checkGoalHex()
{
    if (hex_->getPieceType() == "Coral") {
        return true;
    }
    return false;
}

void hexGraphics::nollaaTila()
{
    if (phase_ == 2) {
        phase_ = 1;
        isRightClicked_ = false;
        activeHex_->update();
    }
}
}
