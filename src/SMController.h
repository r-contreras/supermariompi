#include <iostream>
#include <math.h>
#include <mpi.h>

#include "SMPlayer.h"
namespace SM
{
    class Controller
    {
        private:
            bool* is_alive; //vector to keep track of players' playing condition
            int* coins; //vector to keep track of players' coin count
            int* attacker; //vector to keep track of players' attackers.
            int& num_processes; //reference to num_processes count
            int focused_process; //printing process
            int active_processes; //count to keep track of how many players are alive (reduce calculations)
            //requests (requires both an answer (int) and a request to check if it has been sent)
            int active_processes_requester, random_requester, less_coins_requester, more_coins_requester, attacker_requester;
            MPI_Request active_processes_request, random_request, less_coins_request, more_coins_request, attacker_request;
            //signals (requires both an answer (int) and a request to check if it has been sent)
            int died_signaler;
            int coins_sender_data[2];
            MPI_Request died_signal, coins_signal;
        public:
            //constructor
            Controller(int& num_processes, int focused_process);
            //destructor
            ~Controller();
        private: //internal private
            bool winnerDecided();
            int getRandomActiveProcess(int exception); //gets a random active process, does not take exception into account.
            void handleActiveProcessesRequest(); //updates processes internal active_processes count
            void handleRandomRequest(); //random strat
            void handleLowestCoinsRequest(); //lowest coins strat
            void handleHighestCoinsRequest(); //highest coins strat
            void handleAttackerRequest(); //attacker strat
            void handleDiedSignal(); //signal to be called when a player dies
            void handleCoinsSignal(); //signal to update controller's all players' coin count.
            void sendGameOverSignal(); //signal to be sent when the winner has been decided to kill last process.
        public:
            void run();
    };
};