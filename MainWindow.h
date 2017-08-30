#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QPushButton>
#include <QCheckBox>
#include <QTimer>
#include <QCloseEvent>

#include "sudoku.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private:
    QCheckBox      *sel[10], *eraser;
    QPushButton    *grid[10][10];
    QTimer         *timer;
    QSignalMapper  *mapper;
    Ui::MainWindow *ui;

    //------------------------------------------

    int SelectedNumber;
    int Seconds;
    Sudoku* game;
    bool playing;

    bool Waiting;
    int wait_x, wait_y;

    //------------------------------------------

    struct Operation
    {
        int x, y, val;
        bool isFillin;

        Operation(int x = 0, int y = 0, int val = 0, bool isFillin = true):
            x(x), y(y), val(val), isFillin(isFillin) { }
    }op[9 * 9 * 9];
    int top, top_origin;

    //------------------------------------------

    void Paint();
    void Clear();
    void NewGame(int);
    bool CheckGame();
    void PrepareGame();

    //------------------------------------------

    enum
    {
        size = 45,
        space = 4,
        padding = 8,
        crossstart = 5,
        crossend = 40
    };

public slots:
    void React(int);
    void TimerTimeout();
    void SolveGame();
    void Correction();

    void NewTrivialGame();
    void NewEasyGame();
    void NewMediumGame();
    void NewHardGame();
    void NewExpertGame();

    void LoadGame();
    void RetryGame();
    void QuitGame();
    void StartPauseGame();

    void Undo();
    void Redo();
};

#endif // MAINWINDOW_H
