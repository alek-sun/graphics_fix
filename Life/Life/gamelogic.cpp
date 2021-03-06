#include "gamelogic.h"

using std::cin;
using std::cout;
using std::endl;

GameLogic::GameLogic(int m, int n)
{
    this->m = m;
    this->n = n;
    this->newM = m;
    this->newN = n;
    this -> k = 10;
    //this->liveBegin = 2.0;
    //this->liveEnd = 3.3;
    this->liveBegin = 2.0;
    this->liveEnd = 4.0;
    this->birthBegin = 2.3;
    //this->birthBegin = 1.7;
    //this->birthEnd = 2.9;
    this->birthEnd = 3.0;
    this -> firstImpact = 1.0;
    this->secondImpact = 3.0;
    this->mode = REPLACE_MODE;
}

void GameLogic::step()
{
    int x, y;
    vector<Cell> newState;
    for (int i = 0; i < curState.size(); ++i){
        Cell c (curState[i]);

        if (!c.getIsAlive() && c.getImpact() >= birthBegin && c.getImpact() <= birthEnd){
            c.setIsAlive(true);
        } else if (c.getIsAlive() && c.getImpact() >= liveBegin && c.getImpact() <= liveEnd){

        } else if (c.getImpact() < liveBegin || c.getImpact() > liveEnd) {
            c.setIsAlive(false);
        }
        newState.push_back(c);
    }
    curState = newState;
    changeColors();
}

void GameLogic::clear()
{    
    for (auto &cell : curState){
        cell.setIsAlive(false);
        cell.setImpact(0.0);
        cell.setState(Cell::DIE);       
    }    
}

void GameLogic::changeColors()
{    
    for (auto &cell : curState){
        cell.setState(liveBegin, liveEnd, birthBegin, birthEnd);
        renewImpact(&cell);     // uses curState
    }   
}

void GameLogic::calculateImpacts()
{    
    for (auto &cell : curState){
        renewImpact(&cell);     // uses curState
    }
}

double GameLogic::findCellImpact(int x, int y)
{
    if (x >= 0 && y >= 0 && x <= n-1 && y <= (m - 1 - x % 2)){
        int c = floor(x/2);
        int x1 = (c + x % 2) * m + c * (m - 1);
        return curState[x1+y].getIsAlive();
    }
    return 0.0;
}

void GameLogic::renewImpact(Cell *cell)
{
    double newFirstImpact = 0.0, newSecondImpact = 0.0;

    if (cell->x%2 == 0){    // coords
        newFirstImpact += findCellImpact(cell->x-1, cell->y-1);
        newFirstImpact += findCellImpact(cell->x-1, cell->y);
        newFirstImpact += findCellImpact(cell->x, cell->y-1);
        newFirstImpact += findCellImpact(cell->x, cell->y+1);
        newFirstImpact += findCellImpact(cell->x+1, cell->y-1);
        newFirstImpact += findCellImpact(cell->x+1, cell->y);
    } else {
        newFirstImpact += findCellImpact(cell->x-1, cell->y+1);
        newFirstImpact += findCellImpact(cell->x-1, cell->y);
        newFirstImpact += findCellImpact(cell->x, cell->y-1);
        newFirstImpact += findCellImpact(cell->x, cell->y+1);
        newFirstImpact += findCellImpact(cell->x+1, cell->y+1);
        newFirstImpact += findCellImpact(cell->x+1, cell->y);
    }
    newFirstImpact*=firstImpact;


    if (cell->x%2 == 0){
        newSecondImpact += findCellImpact(cell->x+1, cell->y+1);
        newSecondImpact += findCellImpact(cell->x-1, cell->y-2);
        newSecondImpact += findCellImpact(cell->x-2, cell->y);
        newSecondImpact += findCellImpact(cell->x-1, cell->y+1);
        newSecondImpact += findCellImpact(cell->x+2, cell->y);
        newSecondImpact += findCellImpact(cell->x+1, cell->y-2);
    } else {
        newSecondImpact += findCellImpact(cell->x-1, cell->y-1);
        newSecondImpact += findCellImpact(cell->x-2, cell->y);
        newSecondImpact += findCellImpact(cell->x-1, cell->y+2);
        newSecondImpact += findCellImpact(cell->x+1, cell->y+2);
        newSecondImpact += findCellImpact(cell->x+2, cell->y);
        newSecondImpact += findCellImpact(cell->x+1, cell->y-1);
    }
    newSecondImpact*=secondImpact;

    cell->setImpact(newFirstImpact+newSecondImpact);

}
