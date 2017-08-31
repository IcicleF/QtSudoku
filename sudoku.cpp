#include "sudoku.h"
#include "force_solver.h"
#include <random>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <QMessageBox>

Sudoku::Sudoku()
{
    memset(grid, 0, sizeof(grid));
    memset(visible, 0, sizeof(visible));
}

Sudoku::~Sudoku()
{
}

int Sudoku::getElement(int x, int y) const
{
    return visible[x][y] ? grid[x][y] : 0;
}
int Sudoku::getElementCheat(int x, int y) const
{
    return grid[x][y];
}
void Sudoku::setElement(int x, int y, int k)
{
    grid[x][y] = k;
}
void Sudoku::setVisibility(int x, int y, bool v)
{
    visible[x][y] = v;
}

void Sudoku::generate(int blanks)
{
    srand(time(NULL));

    //============trivial generation===============

    //generate a trivial sudoku
    for (int x = 0; x < 3; ++x)
        for (int r = 0; r < 3; ++r)
        {
            int i = x * 3 + r;
            int st = x + r * 3 + 1;
            for (int j = 0; j < 9; ++j)
            {
                grid[i][j] = st;
                if (++st > 9)
                    st -= 9;
            }
        }
    shuffle();

    //randomly delete numbers & check the solution
    int tries = (blanks == 9 * 9 ? 6 : 1);

    vector<bool> res, best;
    int final, bestans = 9 * 9;
    for (int t = 1; t <= tries; ++t)
    {
        final = blanks;
        res = removeBlocks(final);
        if (bestans > final)
        {
            bestans = final;
            best = res;
        }
    }
    for (int i = 0; i < 9 * 9; ++i)
        visible[i / 9][i % 9] = !best[i];
}

vector<bool> Sudoku::removeBlocks(int& blanks)
{
    vector<bool> ans;
    ans.assign(9 * 9, false);

    int order[9 * 9];
    memset(order, 0, sizeof(order));
    for (int i = 0; i < 9 * 9; ++i)
        order[i] = i;
    for (int i = 0; i < 9 * 9 - 1; ++i)
        std::swap(order[i], order[i + rand() % (80 - i)]);

    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            visible[i][j] = true;

    for (int i = 0; i < 9 * 9 && blanks; ++i)
        if (check(order[i]))
        {
            --blanks;
            ans[order[i]] = true;
            visible[order[i] / 9][order[i] % 9] = false;
        }
    return ans;
}

bool Sudoku::check(int pos)
{
    auto getNum = [pos, this](int x, int y) -> int
    {
        if (pos == x * 9 + y)
            return 0;
        return this->getElement(x, y);
    };

    ForceSolver fs;
    return fs.getSolutions(getNum) == 1;
}

void Sudoku::shuffle()
{
    //shuffle number
    std::default_random_engine generator(time(0));
    std::uniform_int_distribution<int> dist_num(1, 9);
    std::uniform_int_distribution<int> dist_row(0, 2);

    for (int count = 0; count < 15; ++count)
    {
        int u = dist_num(generator);
        int v = dist_num(generator);

        if (u != v)
        {
            for (int i = 0; i < 9; ++i)
                for (int j = 0; j < 9; ++j)
                    if (grid[i][j] == u)
                        grid[i][j] = v;
                    else if (grid[i][j] == v)
                        grid[i][j] = u;
        }
    }

    //shuffle line & column
    for (int count = 0; count < 10; ++count)
    {
        int block = dist_row(generator);
        int u = dist_row(generator);
        int v = dist_row(generator);

        if (u != v)
            for (int j = 0; j < 9; ++j)
                std::swap(grid[block * 3 + u][j], grid[block * 3 + v][j]);
    }
    for (int count = 0; count < 10; ++count)
    {
        int block = dist_row(generator);
        int u = dist_row(generator);
        int v = dist_row(generator);

        if (u != v)
            for (int i = 0; i < 9; ++i)
                std::swap(grid[i][block * 3 + u], grid[i][block * 3 + v]);
    }

    //shuffle line-block & column-block
    int rb[3] = {0, 1, 2}, cb[3] = {0, 1, 2};
    int grid_t[10][10];
    memset(grid_t, 0, sizeof(grid_t));

    std::random_shuffle(rb, rb + 3);
    std::random_shuffle(cb, cb + 3);

    for (int x = 0; x < 3; ++x)
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 9; ++j)
                grid_t[rb[x] * 3 + i][j] = grid[x * 3 + i][j];
    memcpy(grid, grid_t, sizeof(grid));

    for (int y = 0; y < 3; ++y)
        for (int j = 0; j < 3; ++j)
            for (int i = 0; i < 9; ++i)
                grid_t[i][cb[y] * 3 + j] = grid[i][y * 3 + j];
    memcpy(grid, grid_t, sizeof(grid));
}
