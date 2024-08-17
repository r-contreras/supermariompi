#pragma once
#include <iostream>
#include <mpi.h>
#include <random>
#include <time.h>
#include <unistd.h>

#include "SMCharacter.h"
#include "SMComm.h"
#include "SMWorld.h"

namespace SM
{
    enum STRATEGY
    {
        RANDOM,
        LESS_COINS,
        MORE_COINS,
        ATTACKER
    };

class Player
{
    private:
        bool has_focus;
        int& rank, &num_processes;
        int victim; //currently attacked process
        int my_attacker;
        int active_processes;
        SM::World* world;
        SM::Character* character;
        SM::STRATEGY strategy;
        //signals
        int received_enemy; //buffer for attacked_signal
        MPI_Request attacked_signal, focus_signal;
    public:
        //default constructor
        Player(int& rank, int& num_processes);
        //strategy constructor, sets focus
        Player(int& rank, int& num_processes, char strategy);
        //destructor        
        ~Player();
    private: //interno
        //requests made to the controller
        void sendAttackerRequest(); //updates local my_attacker (current process that is targeting this process)
        void sendStrategyRequest(); //requests new victim process
        void sendActiveProcessesRequest(); //requests for local active_processes update
        void sendCoinsSignal(); //sends updated coin count to controller
        void sendDiedSignal(); //informs controller that this player has died
        //signals
        void handleFocusSignal(); //sets this players' has_focus to true
        void handleAttackSignal(); //creates new enemies from other players' attacks
        void sendAttackSignal(SM::Object* killed_enemy); //sends enemies to other players
    public:
        void run();
        void moveCharacter(); //moves character through the SM::World
        //sets
        void setRandomStrategy();
};
    //static method to get strategy string
    static std::string getStrategyString(SM::STRATEGY strategy);
};//namespace SM