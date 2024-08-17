#include "SMPlayer.h"

SM::Player::Player(int& rank, int& num_processes)
    : has_focus(false), 
    rank{rank}, 
    num_processes{num_processes}, 
    active_processes{num_processes-1},
    victim{0},
    my_attacker{0}
{
    
    this->setRandomStrategy();
    this->world = new World();
    this->character = new Mario();
}
SM::Player::Player(int& rank, int& num_processes, char strategy)
    : has_focus{true},
    rank{rank}, 
    num_processes{num_processes}, 
    active_processes{num_processes-1},
    victim{0},
    my_attacker{0}
{
    switch(strategy)
    {
        case 'R':
            this->strategy = SM::STRATEGY::RANDOM;
        break;
        case 'L':
            this->strategy = SM::STRATEGY::LESS_COINS;
        break;
        case 'M':
            this->strategy = SM::STRATEGY::MORE_COINS;
        break;
        case 'A':
            this->strategy = SM::STRATEGY::ATTACKER;
        break;
        default:
            throw std::runtime_error("Error: Invalid strategy");
    }
    this->world = new World();
    this->character = new Mario();
}

SM::Player::~Player()
{
    delete this->world;
    delete this->character;
}

void SM::Player::run()
{
    int attacked_flag, focus_flag;
    //init flags in false (0)
    attacked_flag=focus_flag = false;

    //listen for signals
    MPI_Irecv(NULL, 0, MPI_C_BOOL, 0, SM_SIGNAL_FOCUS, MPI_COMM_WORLD, &focus_signal);
    MPI_Irecv(&received_enemy, 1, MPI_INT, MPI_ANY_SOURCE, SM_ATTACK, MPI_COMM_WORLD, &attacked_signal);
    while(this->character->isAlive() && active_processes > 1)
    {
        //update my_attacker
        this->sendAttackerRequest();
        //update controller coint count
        this->sendCoinsSignal();
        //check if any signal must be handled
        MPI_Test(&attacked_signal, &attacked_flag, MPI_STATUS_IGNORE);
        MPI_Test(&focus_signal, &focus_flag, MPI_STATUS_IGNORE);
        if(focus_flag)
        {
            this->handleFocusSignal();
            focus_flag = false;
        }
        if(attacked_flag)
        {
            this->handleAttackSignal();
            attacked_flag = false;
        }
        this->moveCharacter(); //move character in local world
        sleep(1); //sleep for simulation purposes
        this->sendActiveProcessesRequest(); //request active processes update
    }
    this->sendDiedSignal();
}

void SM::Player::moveCharacter()
{
    SM::Object* current_element = world->getNextObject();
    SM::EVENT result = character->interactWith(current_element);

    //prepare printing string
    std::stringstream stream;
    stream << "World pos. " << world->getCurrentWorldCell() << ": " << character->getName() << " #" << rank;
    switch (result)
    {
        case (SM::EVENT::NONE):
            stream << " is running";
        break;
        case (SM::EVENT::DIED):
            character->kill();
            stream << " didn't jump and got killed by a " << current_element->getName();
        break;
        case (SM::EVENT::ADD_COIN):
            character->addCoin();
            stream << " jumped and grabbed a coin";
        break;
        case (SM::EVENT::KILLED_ENEMY):
            character->addCoin();
            stream << " jumped and grabbed a coin by killing a " << current_element->getName();
            this->sendAttackSignal(current_element);
        break;
    }
    if(this->has_focus)
    {
        stream << ". Coins: " << character->getCoinCount() << "| attacking #" << ( (victim)? std::to_string(victim): "NONE") << " | being attacked by #" << ( (my_attacker)? std::to_string(my_attacker): "NONE") 
        << " | attack strategy: " << getStrategyString(this->strategy) << " | Total playing: " << active_processes;
        //print string to std::cout
        std::cout << stream.str() << std::endl;
        if(!this->character->isAlive())
        {
            std::cout << "World pos. " << world->getCurrentWorldCell() << ": " << character->getName() << " #" << rank << " Game Over." << std::endl;
        }
    }
}

void SM::Player::setRandomStrategy()
{
    unsigned int temp = (rand()*rank);
    int random_number = temp % 4;
    switch(random_number)
    {
        case 0:
            this->strategy = SM::STRATEGY::RANDOM;
        break;
        case 1:
            this->strategy = SM::STRATEGY::LESS_COINS;
        break;
        case 2:
            this->strategy = SM::STRATEGY::MORE_COINS;
        break;
        case 3:
            this->strategy = SM::STRATEGY::ATTACKER;
        break;
        default:
            throw std::runtime_error("Error: Invalid strategy");
        break;
    }
}

void SM::Player::sendActiveProcessesRequest()
{
    MPI_Send(&rank, 1, MPI_INT, 0, SM_REQUEST_ACTIVE_PROCESSES, MPI_COMM_WORLD);
    //wait to receive the attacker
    MPI_Recv(&active_processes, 1, MPI_INT, 0, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void SM::Player::sendDiedSignal()
{
    //send died signal
    MPI_Send(&rank, 1, MPI_INT, 0, SM_SIGNAL_DIED, MPI_COMM_WORLD);
    //player no longer has focus (if it had it)
    has_focus = false;
}

void SM::Player::sendAttackerRequest()
{
    //send my attacker request
    MPI_Send(&rank, 1, MPI_INT, 0, SM_REQUEST_ATTACKER, MPI_COMM_WORLD);
    //store response in my_attacker
    MPI_Recv(&my_attacker, 1, MPI_INT, 0, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void SM::Player::sendStrategyRequest()
{
    //send petition to receive the attacker
    int tag;
    switch(strategy)
    {
        case SM::STRATEGY::RANDOM:
            tag = SM_REQUEST_RANDOM;
        break;
        case SM::STRATEGY::LESS_COINS:
            tag = SM_REQUEST_LESS_COINS;
        break;
        case SM::STRATEGY::MORE_COINS:
            tag = SM_REQUEST_MORE_COINS;
        break;
        case SM::STRATEGY::ATTACKER:
            tag = SM_REQUEST_ATTACKER;
    }
    MPI_Send(&rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    //wait to receive the attacker
    MPI_Recv(&victim, 1, MPI_INT, 0, SM_CONTROLLER_RESPONSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void SM::Player::handleFocusSignal()
{
    this->has_focus = true;
    //Process no longer needs to listen for the signal.
}

void SM::Player::handleAttackSignal()
{
    SM::Object* new_enemy;
    switch(received_enemy)
    {
        case SM_ENEMY_LITTLE_GOOMBA:
            new_enemy = new SM::LittleGumba();
        break;
        case SM_ENEMY_KOOPA_TROOPA:
            new_enemy = new SM::KoopaTroopa();
        break;
        default:
            throw std::runtime_error("Invalid enemy ID");
        break;
    }
    new_enemy->setAttackGenerated();
    world->addEnemy(new_enemy);
    //listen for another attack signal
    MPI_Irecv(&received_enemy, 1, MPI_INT, MPI_ANY_SOURCE, SM_ATTACK, MPI_COMM_WORLD, &attacked_signal);
}

void SM::Player::sendAttackSignal(SM::Object *killed_enemy)
{
    this->sendStrategyRequest(); //requests controller to update victim tag
    int killed_enemy_tag;
    std::string killed_enemy_type = killed_enemy->getName();

    if(killed_enemy_type == "little goomba")
        killed_enemy_tag = SM_ENEMY_LITTLE_GOOMBA;
    if(killed_enemy_type == "koopa troopa")
        killed_enemy_tag = SM_ENEMY_KOOPA_TROOPA;
    
    //if the enemy was attack generated, its memory must be freed. Otherwise it would get lost.
    if(killed_enemy->attackGenerated())
        delete killed_enemy;

    //send the enemy the new enemy to victim process
    MPI_Send(&killed_enemy_tag, 1, MPI_INT, victim, SM_ATTACK, MPI_COMM_WORLD);
}

void SM::Player::sendCoinsSignal()
{
    int coin_data[2];
    coin_data[0] = character->getCoinCount();
    coin_data[1] = rank;
    MPI_Send(coin_data, 2, MPI_INT, 0, SM_PLAYER_RESPONSE, MPI_COMM_WORLD);
}

std::string SM::getStrategyString(SM::STRATEGY strategy)
{
    std::string strat_string;
    switch(strategy)
    {
        case SM::STRATEGY::RANDOM:
            strat_string = "RANDOM";
        break;
        case SM::STRATEGY::LESS_COINS:
            strat_string = "LESS_COINS";
        break;
        case SM::STRATEGY::MORE_COINS:
            strat_string = "MORE_COINS";
        break;
        case SM::STRATEGY::ATTACKER:
            strat_string = "ATTACKER";
    }
    return strat_string;
}