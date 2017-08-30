#ifndef FORCE_SOLVER_H
#define FORCE_SOLVER_H

#include <functional>

class ForceSolver
{
public:
    ForceSolver() { }
    ~ForceSolver() { }

    int getSolutions(std::function<int(int, int)>);

private:
    bool r[10][10], c[10][10], s[10][10];
    int a[10][10], b[10][10];
    int ans;

    void search(int x, int y);
    bool ins(int x, int y, int k);
    void del(int x, int y, int k);
};


#endif // FORCE_SOLVER_H
