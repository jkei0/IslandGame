#include "player.hh"

namespace Student {


Player::Player()
{

}

Player::Player(int id, int totalPawnCount) :
    id_(id),
    totalPawnCount_(totalPawnCount)
{

}

Player::~Player()
{

}

int Player::getPlayerId() const
{
    return id_;
}

void Player::setActionsLeft(unsigned int actionsLeft)
{
    actionsLeft_ = actionsLeft;
}

unsigned int Player::getActionsLeft() const
{
    return actionsLeft_;
}

}
