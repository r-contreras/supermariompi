#include "SMController.h"

SM::Controller::Controller(int& num_processes, int focused_process)
    : num_processes{num_processes}, focused_process {focused_process}, active_processes {num_processes-1}
{
    is_alive = new bool[num_processes]();
    coins = new int[num_processes]();
    attacker = new int[num_processes]();
    //init vectors
    for(int process = 1; process < num_processes; ++process)
    {
        is_alive[process] = true;
        attacker[process] = 0; //although the constructor was called with (), just to be sure
        coins[process] = 0; // same thing here
    }
}

SM::Controller::~Controller()
{
    delete this->is_alive;
    delete this->coins;
    delete this->attacker;
}

void SM::Controller::run() 
{
    int coins_flag, active_processes_flag, random_flag, less_coins_flag, more_coins_flag, attacker_flag, died_flag;
    //init in false (0)
    coins_flag=active_processes_flag=random_flag=less_coins_flag=more_coins_flag=attacker_flag=died_flag= false;

    //listen for all requests
    MPI_Irecv(coins_sender_data, 2, MPI_INT, MPI_ANY_SOURCE, SM_PLAYER_RESPONSE, MPI_COMM_WORLD, &coins_signal);
    MPI_Irecv(&active_processes_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_ACTIVE_PROCESSES, MPI_COMM_WORLD, &active_processes_request);
    MPI_Irecv(&random_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_RANDOM, MPI_COMM_WORLD, &random_request);
    MPI_Irecv(&less_coins_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_LESS_COINS, MPI_COMM_WORLD, &less_coins_request);
    MPI_Irecv(&more_coins_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_MORE_COINS, MPI_COMM_WORLD, &more_coins_request);
    MPI_Irecv(&attacker_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_ATTACKER, MPI_COMM_WORLD, &attacker_request);
    MPI_Irecv(&died_signaler, 1, MPI_INT, MPI_ANY_SOURCE, SM_SIGNAL_DIED, MPI_COMM_WORLD, &died_signal);
    //while winner hasn't been decided
    while(!winnerDecided())
    {
        //see if any process has made any request/signal
        MPI_Test(&active_processes_request, &active_processes_flag, MPI_STATUS_IGNORE);
        MPI_Test(&random_request, &random_flag, MPI_STATUS_IGNORE);
        MPI_Test(&less_coins_request, &less_coins_flag, MPI_STATUS_IGNORE);
        MPI_Test(&more_coins_request, &more_coins_flag, MPI_STATUS_IGNORE);
        MPI_Test(&attacker_request, &attacker_flag, MPI_STATUS_IGNORE);
        MPI_Test(&died_signal, &died_flag, MPI_STATUS_IGNORE);
        if(coins_flag)
        {
            handleCoinsSignal();
            coins_flag = false;
        }
        if(active_processes_flag)
        {
            handleActiveProcessesRequest();
            active_processes_flag = false;
        }
        if(random_flag)
        {
            handleRandomRequest();
            random_flag = false;
        }
        if(less_coins_flag)
        {
            handleLowestCoinsRequest();
            less_coins_flag = false;
        }
        if(more_coins_flag)
        {
            handleHighestCoinsRequest();
            more_coins_flag = false;
        }
        if(attacker_flag)
        {
            handleAttackerRequest();
            attacker_flag = false;
        }
        if(died_flag)
        {
            handleDiedSignal();
            died_flag = false;
        }
    }
    std::cout << "Winner decided! Player [" << focused_process << "] wins!" << std::endl;
    this->sendGameOverSignal();
}

bool SM::Controller::winnerDecided()
{
    if(active_processes > 1)
        return false;
    else
        return true;
}

int SM::Controller::getRandomActiveProcess(int exception)
{
    int random_process = 0;
    while(random_process == 0)
    {
        random_process = rand()%num_processes;
        if(is_alive[random_process] && exception != random_process)
            break;
        else
            random_process = 0;
    }
    return random_process;
}

void SM::Controller::handleActiveProcessesRequest()
{
    //send the requester the number of active processes
    MPI_Send(&active_processes, 1, MPI_INT, active_processes_requester, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD);
    //listen for another request
    MPI_Irecv(&active_processes_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_ACTIVE_PROCESSES, MPI_COMM_WORLD, &active_processes_request);
}

void SM::Controller::handleRandomRequest()
{
    //search for a random active process
    int random_process = getRandomActiveProcess(random_requester);
    //update attacker for this process
    attacker[random_process] = random_requester;
    //send the random process to the requester
    MPI_Send(&random_process, 1, MPI_INT, random_requester, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD);
    //listen for another request
    MPI_Irecv(&random_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_RANDOM, MPI_COMM_WORLD, &random_request);
}
void SM::Controller::handleLowestCoinsRequest()
{
    int less_coins = INT32_MAX;
    int lowest_process = 0;
    for(int process = 1; process < num_processes; ++process)
    {
        if(is_alive[process] && coins[process] < less_coins && process != less_coins_requester)
        {
            less_coins = coins[process];
            lowest_process = process;
        }
    }
    //update attacker for this process
    attacker[lowest_process] = less_coins_requester;
    //send the requester the rank of the lowest coin count process
    MPI_Send(&lowest_process, 1, MPI_INT, less_coins_requester, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD);
    //listen for another request
    MPI_Irecv(&less_coins_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_LESS_COINS, MPI_COMM_WORLD, &less_coins_request);
}
void SM::Controller::handleHighestCoinsRequest()
{
    int more_coins = -1;
    int highest_process = 0;
    for(int process = 1; process < num_processes; ++process)
    {
        if(is_alive[process] && coins[process] > more_coins && process!=more_coins_requester)
        {
            more_coins = coins[process];
            highest_process = process;
        }
    }
    //update attacker for this process
    attacker[highest_process] = more_coins_requester;
    //send the requester the rank of the highest coin count process
    MPI_Send(&highest_process, 1, MPI_INT, more_coins_requester, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD);
    //listen for another request
    MPI_Irecv(&more_coins_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_MORE_COINS, MPI_COMM_WORLD, &more_coins_request);
}
void SM::Controller::handleAttackerRequest()
{
    int attacker_process = attacker[attacker_requester];
    //if there is no player attacking this process, a random one is chosen
    if(attacker_process == 0)
        attacker_process = getRandomActiveProcess(attacker_requester);
    //send the requester the rank of the highest coin process
    MPI_Send(&attacker_process, 1, MPI_INT, attacker_requester, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD);
    //listen for another request
    MPI_Irecv(&attacker_requester, 1, MPI_INT, MPI_ANY_SOURCE, SM_REQUEST_ATTACKER, MPI_COMM_WORLD, &attacker_request);
}

void SM::Controller::handleDiedSignal()
{
    this->is_alive[died_signaler] = false;
    --this->active_processes;
    //if the winner has been decided
    if(active_processes < 2)
        return;
    //if the signaler was the focused process
    if(died_signaler == focused_process)
    {
        std::cout << "Player [" << focused_process << "] died." << std::endl;
        while(!is_alive[focused_process])
        {
            std::cout << "Choose another player to watch: ";
            std::cin >> focused_process;
            if(focused_process < 1 || focused_process > num_processes-1)
            {
                std::cout << "Error: Player [" << focused_process << "] isn't playing. Range: [1," << num_processes-1 << "]" << std::endl;
                focused_process = 0;
            }
            if(!is_alive[focused_process])
            {
                std::cout << "Error: Player [" << focused_process << "] is dead." << std::endl;
            }
        }
    }
    //send focus signal to the new focused process
    MPI_Send(nullptr, 0, MPI_C_BOOL, focused_process, SM_SIGNAL_FOCUS, MPI_COMM_WORLD);
    //listen for another signal
    MPI_Irecv(&died_signaler, 1, MPI_INT, MPI_ANY_SOURCE, SM_SIGNAL_DIED, MPI_COMM_WORLD, &died_signal);
}

void SM::Controller::sendGameOverSignal()
{
    //Searches the last process alive and sends updates the active_processes. There is no handler for this signal
    int last_process = 0;
    for(int process = 1; process < num_processes; ++process)
    {
        if(is_alive[process])
        {
            last_process = process;
            break;
        }
    }
    //Since this signal meets the stopping condition of the last_process, the process is killed.
    MPI_Send(&active_processes, 1, MPI_INT, last_process, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD);
}

void SM::Controller::handleCoinsSignal()
{
    int sender = coins_sender_data[1];
    coins[sender] = coins_sender_data[0];
    std::cout << "updated process [" << sender << "] coins : " << coins[sender] << std::endl;
    //listen for another request
    MPI_Irecv(coins_sender_data, 2, MPI_INT, MPI_ANY_SOURCE, SM_PLAYER_RESPONSE, MPI_COMM_WORLD, &coins_signal);
}