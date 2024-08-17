#pragma once
#include <string>

#include "SMObject.h"

namespace SM
{

    class Character
    {
    protected:
        std::string name;
        bool is_alive;
        size_t coin_counter;

    protected:
        Character(std::string name);

    public:
        //destructor
        virtual ~Character();
        //modifiers
        void kill();
        SM::EVENT interactWith(SM::Object *object);
        void addCoin();
        bool isAlive();
        //gets
        size_t getCoinCount();
        std::string getName();
    };

    class Mario : public Character
    {
    public:
        //constructor
        Mario();
        ~Mario();
    };
}; // namespace SM