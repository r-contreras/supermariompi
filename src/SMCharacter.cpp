#include "SMCharacter.h"

SM::Character::Character(std::string name)
    : name{name}, is_alive{true}, coin_counter{0}
{}

SM::Character::~Character()
{}

void SM::Character::kill()
{
    this->is_alive = false;
}

SM::EVENT SM::Character::interactWith(SM::Object* object)
{
    if(object == nullptr)
        return SM::EVENT::NONE;
    return object->getEvent();
}

void SM::Character::addCoin()
{
    ++this->coin_counter;
}

bool SM::Character::isAlive()
{
    return this->is_alive;
}
size_t SM::Character::getCoinCount()
{
    return this->coin_counter;
}

std::string SM::Character::getName()
{
    return this->name;
}

SM::Mario::Mario()
    : Character("Mario")
{}

SM::Mario::~Mario()
{}
