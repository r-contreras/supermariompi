#pragma once
#include <string>
#include <random>

namespace SM
{
    enum EVENT
    {
        NONE,
        DIED,
        ADD_COIN,
        KILLED_ENEMY
    };

class Object
{
    protected:
        bool attack_generated;
        std::string name;
    protected:
        //constructor
        Object(std::string name);
    public:
        //destructor
        virtual ~Object();
        virtual SM::EVENT getEvent() = 0; //retorna un SM::EVENT
        //gets
        std::string getName();
        bool attackGenerated();
        //sets
        void setAttackGenerated();

};

class Coin : public Object
{
    public:
        //constructor
        Coin();
        //destructor
        ~Coin();
        SM::EVENT getEvent() override;
};

class Hole : public Object
{
    public:
        //constructor
        Hole();
        //destructor
        ~Hole();
        SM::EVENT getEvent() override;
};

class LittleGumba : public Object
{
    public:
        //constructor
        LittleGumba();
        //destructor
        ~LittleGumba();
        SM::EVENT getEvent() override;
};

class KoopaTroopa : public Object
{
    public:
        //constructor
        KoopaTroopa();
        //destructor
        ~KoopaTroopa();
        SM::EVENT getEvent() override;
};
};