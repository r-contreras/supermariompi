#pragma once

#include <deque>
#include <fstream>
#include <sstream>
#include <queue>

#include "SMCharacter.h"
#include "SMObject.h"

namespace SM
{
    class WorldCell
        {
        private: 
            std::deque<SM::Object *> *element_deque;
            std::queue<SM::Object *> *enemy_queue;
            size_t current_element;
        public:
            //constructor
            WorldCell();
            //destructor
            ~WorldCell();

        public:
            void addElement(SM::Object *element); //adds an enemy to the element deque
            void addEnemy(SM::Object *enemy); //adds an enemy to the enemy_queue
            //get
            SM::Object *getNextObject();
            bool isEmpty();
        };

    class World
    {
    private:
        std::deque<WorldCell *> *world_cell_deque;
        size_t current_world_cell;
    public:
        //constructor
        World();
        //destructor
        ~World();

    private:
        SM::WorldCell* addEmptyCell();
    public:
        void build_world(std::string file_name);
        void addEnemy(SM::Object *enemy);
        //gets
        SM::Object *getNextObject();
        int getCurrentWorldCell();
    };
}; // namespace SM