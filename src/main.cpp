#include <random>
#include <time.h>
#include <mpi.h>

#include "SMController.h"
#include "SMPlayer.h"

int main(int argc, char* argv[])
{
    srand(time(NULL));
    if(argc!=3)
    {
        return 1;
    }
    int rank, num_processes;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int selected_process = strtoul(argv[1], NULL, 10);
    char strategy = argv[2][0];

    SM::Controller* controller = nullptr;
    SM::Player* player = nullptr;
    if(rank == 0)
    {
        controller = new SM::Controller(num_processes, selected_process);
        controller->run();
    }
    else
    {
        if(rank == selected_process)
            player = new SM::Player(rank, num_processes, strategy);
        else
            player = new SM::Player(rank, num_processes);

        player->run();
    }

    if(controller)
        delete controller;
    if(player)
        delete player;
    MPI_Finalize();
    return 0;
}