#pragma once
//IN THIS HEADER ALL SM (SUPER MARIO) REQUESTS AND SIGNALS TAGS ARE DEFINED.

//Requests to controller
#define SM_REQUEST_RANDOM 1
#define SM_REQUEST_LESS_COINS 2
#define SM_REQUEST_MORE_COINS 3
#define SM_REQUEST_ATTACKER 4
#define SM_REQUEST_ACTIVE_PROCESSES 5
//Signals to controller
#define SM_SIGNAL_DIED 6

//Signals from controller to players
#define SM_SIGNAL_FOCUS 7
//Response from controller
#define SM_CONTROLLER_RESPONSE 8

//Requests to players
#define SM_REQUEST_COIN_COUNT 9
//Response from players
#define SM_PLAYER_RESPONSE 10

//Attacking enemies identifier
#define SM_ENEMY_LITTLE_GOOMBA 11
#define SM_ENEMY_KOOPA_TROOPA 12

//Communication between players
#define SM_ATTACK 13