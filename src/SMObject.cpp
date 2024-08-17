#include "SMObject.h"

SM::Object::Object(std::string name)
    : name {name}, attack_generated {false}
{}

SM::Object::~Object()
{}

std::string SM::Object::getName()
{
    return this->name;
}

bool SM::Object::attackGenerated()
{
    return this->attack_generated;
}

void SM::Object::setAttackGenerated()
{
    this->attack_generated = true;
}

SM::Coin::Coin()
    : Object("coin")
{}

SM::Coin::~Coin()
{}

SM::EVENT SM::Coin::getEvent()
{
    float random = static_cast<float>(rand()) / static_cast <float> (RAND_MAX);
    if(random < 0.5)
        return SM::EVENT::NONE;
    else
        return SM::EVENT::ADD_COIN;
}

SM::Hole::Hole()
    : Object("hole")
{}

SM::Hole::~Hole()
{}

SM::EVENT SM::Hole::getEvent()
{
    float random = static_cast<float>(rand()) / static_cast <float> (RAND_MAX);
    if(random > 0.05)
        return SM::EVENT::NONE;
    else
        return SM::EVENT::DIED;
}

SM::LittleGumba::LittleGumba()
    : Object("little goomba")
{}

SM::LittleGumba::~LittleGumba()
{}

SM::EVENT SM::LittleGumba::getEvent()
{
    float random = static_cast<float>(rand()) / static_cast <float> (RAND_MAX);
    if(random <= 0.55)
        return SM::EVENT::NONE;
    else if (random <= 0.95)
        return SM::EVENT::KILLED_ENEMY;
    else
        return SM::EVENT::DIED;
}

SM::KoopaTroopa::KoopaTroopa()
    : Object("koopa troopa")
{}

SM::KoopaTroopa::~KoopaTroopa()
{}

SM::EVENT SM::KoopaTroopa::getEvent()
{
    float random = static_cast<float>(rand()) / static_cast<float> (RAND_MAX);
    if(random <= 0.53)
        return SM::EVENT::NONE;
    else if (random <= 0.90)
        return SM::EVENT::KILLED_ENEMY;
    else
        return SM::EVENT::DIED;
}
