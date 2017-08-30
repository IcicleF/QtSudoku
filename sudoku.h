#ifndef SUDOKU_H
#define SUDOKU_H

#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>

using std::string;
using std::vector;

class Sudoku
{
public:
    Sudoku();
    ~Sudoku();

    int getElement(int, int) const;
    int getElementCheat(int, int) const;
    void generate(int);
    void setElement(int, int, int);
    void setVisibility(int, int, bool);

private:
    int grid[10][10];
    bool visible[10][10];

    void shuffle();
    vector<bool> removeBlocks(int&);
    bool check(int);
};

#endif // SUDOKU_H
