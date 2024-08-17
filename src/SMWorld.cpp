#include "SMWorld.h"

SM::WorldCell::WorldCell()
    : current_element {0}
{
    this->element_deque = new std::deque<SM::Object *>();
    this->enemy_queue = new std::queue<SM::Object *>();
}

SM::WorldCell::~WorldCell() 
{
    //every object in the cell is deleted, both in the queue and deque
    for(auto iterator = element_deque->begin(); iterator != element_deque->end(); ++ iterator)
        delete (*iterator); //borra el objeto
    
    while(!enemy_queue->empty())
    {
        auto dummy = enemy_queue->front();
        enemy_queue->pop();
        delete dummy;
    }
    delete this->element_deque;
    delete this->enemy_queue;
}

void SM::WorldCell::addElement(SM::Object *element) 
{
    this->element_deque->push_back(element);      
}

void SM::WorldCell::addEnemy(SM::Object *enemy) 
{
    this->enemy_queue->push(enemy);    
}

SM::Object* SM::WorldCell::getNextObject() 
{
    //if there are any attack generated enemies in this cell, return them.
    //else return the next default cell object
    SM::Object* next_object = nullptr;
    if(!enemy_queue->empty())
    {
        next_object = enemy_queue->front();
        enemy_queue->pop(); //NOTA: SE DEBE BORRAR EL ENEMIGO UNA VEZ QUE SE SACA DE ESTA ESTRUCTURA
    }
    else if(current_element < element_deque->size())
    {
        next_object = element_deque->at(current_element++);
    }
    else
    {
        //count is set back to 0 if the last element has been returned
        this->current_element = 0;
    }
    
    return next_object;
}

bool SM::WorldCell::isEmpty()
{
    return this->element_deque->size() == 0;
}
SM::World::World()
    : current_world_cell {0}
{
    this->world_cell_deque = new std::deque<WorldCell*>(); 
    this->build_world("levels/1-1.txt"); //level name
}

SM::World::~World() 
{
    for(auto iterator = world_cell_deque->begin(); iterator != world_cell_deque->end(); ++iterator)
        delete *iterator; //deletes the object contained in the deque
    delete this->world_cell_deque;    
}

SM::WorldCell* SM::World::addEmptyCell()
{
    auto new_cell = new SM::WorldCell();
    this->world_cell_deque->push_back(new_cell);
    return new_cell;
}

void SM::World::build_world(std::string file_name) 
{
    std::ifstream file(file_name);
    if(!file.is_open())
    {
        throw std::runtime_error("World map file could not be opened.");
    }
    std::string cell_data; //where new line will be stored
    while(std::getline(file, cell_data, ';')) //while there is any data
    {
        SM::WorldCell* new_cell = addEmptyCell();
        std::stringstream cell_stream(cell_data);
        char object_data;

        while(cell_stream >> object_data) //while there are objects in the cell
        {
            SM::Object* new_object = nullptr;

            switch(object_data)
            {
                case 'C':
                    new_object = new SM::Coin();
                    break;
                case 'H':
                    new_object = new SM::Hole();
                    break;
                case 'L':
                    new_object = new SM::LittleGumba();
                    break;
                case 'K':
                    new_object = new SM::KoopaTroopa();
                    break;
                default:
                    break;
            }
            //if there is no object it isn't added if cell is not empty
            if(new_object || new_cell->isEmpty())
                new_cell->addElement(new_object);
        }
    }
}

void SM::World::addEnemy(SM::Object *enemy) 
{
    size_t targetCellPosition = (current_world_cell+10) % this->world_cell_deque->size();
    auto targetCell = this->world_cell_deque->at(targetCellPosition-1);
    targetCell->addEnemy(enemy);
}

#include <iostream>
SM::Object* SM::World::getNextObject() 
{
    //if there aren't any cells left, counter is set to 0
    if(this->current_world_cell == this->world_cell_deque->size())
    {
        this->current_world_cell = 0;
        return nullptr;
    }

    auto cell = this->world_cell_deque->at(current_world_cell);
    SM::Object* next_object = cell->getNextObject();
    
    if(next_object)
        return next_object;
    else //if current cell doesn't have more objects but there are any cells left
    {
        ++this->current_world_cell;
        return nullptr;
    }
}

int SM::World::getCurrentWorldCell()
{
    return this->current_world_cell;
}
