#include "force_solver.h"
#include <cstring>

#define searchnext(x, y) y == 8 ? search(x + 1, 0) : search(x, y + 1)

int ForceSolver::getSolutions(std::function<int(int, int)> getNum)
{
    memset(r, 0, sizeof(r));
    memset(c, 0, sizeof(c));
    memset(s, 0, sizeof(s));
    memset(a, 0, sizeof(a));
    memset(b, 0, sizeof(b));

    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
        {
            a[i][j] = getNum(i, j);
            if (a[i][j])
                ins(i, j, a[i][j]);
        }

    ans = 0;
    search(0, 0);
    return ans;
}

bool ForceSolver::ins(int x, int y, int k)
{
    if (r[x][k] || c[y][k] || s[x / 3 * 3 + y / 3][k]) return false;
    b[x][y] = k;
    r[x][k] = c[y][k] = s[x / 3 * 3 + y / 3][k] = true;
    return true;
}

void ForceSolver::del(int x, int y, int k)
{
    b[x][y] = 0;
    r[x][k] = c[y][k] = s[x / 3 * 3 + y / 3][k] = false;
}

void ForceSolver::search(int x, int y)
{
    if (x == 9)
    {
        ++ans;
        return;
    }
    if (b[x][y])
        searchnext(x, y);
    else
        for (int i = 1; i <= 9; ++i)
            if (ins(x, y, i))
            {
                searchnext(x, y);
                del(x, y, i);
            }
}
