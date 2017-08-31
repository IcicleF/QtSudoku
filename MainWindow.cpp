#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

inline QString Prefix(int x)
{
    return x >= 10 ? QString::number(x) : ("0" + QString::number(x));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mapper = new QSignalMapper(this);

    ui->lblPaused->setVisible(false);
    ui->btnPause->setEnabled(false);
    ui->btnPause->setChecked(false);

    timer = NULL;
    ui->lcdMin->display("00");
    ui->lcdSec->display("00");

    ui->menuCheat->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionSave->setEnabled(false);

    playing = false;
    SelectedNumber = 0;
    Waiting = false;
    wait_x = wait_y = -1;

    top = top_origin = 0;

    //insert buttons
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
        {
            grid[i][j] = new CrossButton("", ui->frame);
            grid[i][j]->setGeometry((size + space) * i + (i / 3) * padding, (size + space) * j + (j / 3) * padding, size, size);
            grid[i][j]->show();

            connect(grid[i][j], SIGNAL(clicked()), mapper, SLOT(map()));
            mapper->setMapping(grid[i][j], 9 * i + j);
        }

    //insert radio-buttons
    char s[] = "0";
    for (int i = 1; i <= 9; ++i)
    {
        s[0] = i + '0';
        sel[i] = new QCheckBox(s);
        sel[i]->setFont(QFont("Lucida Bright", 18, 50));
        sel[i]->setEnabled(false);
        ui->verticalLayout->addWidget(sel[i]);

        connect(sel[i], SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(sel[i], -i);
    }

    //insert radio-button: eraser
    eraser = new QCheckBox("Eraser");
    eraser->setFont(QFont("Lucida Bright", 18, 50));
    eraser->setEnabled(false);
    ui->verticalLayout->addWidget(eraser);
    connect(eraser, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(eraser, -999);

    //set connections
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(React(int)));

    connect(ui->actionTrivial, SIGNAL(triggered()), this, SLOT(NewTrivialGame()));
    connect(ui->actionEasy, SIGNAL(triggered()), this, SLOT(NewEasyGame()));
    connect(ui->actionMedium, SIGNAL(triggered()), this, SLOT(NewMediumGame()));
    connect(ui->actionHard, SIGNAL(triggered()), this, SLOT(NewHardGame()));
    connect(ui->actionExpert, SIGNAL(triggered()), this, SLOT(NewExpertGame()));
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(LoadGame()));

    connect(ui->actionRetry, SIGNAL(triggered()), this, SLOT(RetryGame()));
    connect(ui->actionSolve, SIGNAL(triggered()), this, SLOT(SolveGame()));
    connect(ui->actionShowMistakes, SIGNAL(triggered()), this, SLOT(Correction()));

    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(Undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(Redo()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(SaveGameSlot()));
    connect(ui->actionResume, SIGNAL(triggered()), this, SLOT(ResumeGame()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(QuitGame()));

    connect(ui->btnPause, SIGNAL(clicked()), this, SLOT(StartPauseGame()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (playing)
        this->SaveGame(false);
    event->accept();
}

void MainWindow::RetryGame()
{
    if (playing)
    {
        timer->stop();
        auto rb = QMessageBox::question(this, "Confirm", "Are you sure to retry?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (!ui->btnPause->isChecked())
            timer->start();
        if (rb == QMessageBox::No)
            return;
    }
    this->PrepareGame();
}

void MainWindow::QuitGame()
{
    this->close();
    qApp->quit();
}

void MainWindow::StartPauseGame()
{
    if (ui->btnPause->isChecked())
    {
        ui->frame->setVisible(false);
        for (int i = 1; i <= 9; ++i)
            sel[i]->setVisible(false);
        eraser->setVisible(false);
        ui->menuCheat->setEnabled(false);
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->lblPaused->setVisible(true);
        if (timer != NULL)
            timer->stop();
    }
    else
    {
        ui->frame->setVisible(true);
        for (int i = 1; i <= 9; ++i)
            sel[i]->setVisible(true);
        eraser->setVisible(true);
        ui->menuCheat->setEnabled(true);
        ui->actionUndo->setEnabled(top > 0);
        ui->actionRedo->setEnabled(top < top_origin);
        ui->lblPaused->setVisible(false);
        if (timer != NULL)
            timer->start();
    }
}
void MainWindow::ResumeGame()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    path += "/recent.txt";

    QFile file(path);
    if (!file.exists())
    {
        if (playing)
            timer->stop();
        QMessageBox::warning(this, "Warning", "No save data found.", QMessageBox::Ok);
        if (playing && !ui->btnPause->isChecked())
            timer->start();
        return;
    }
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream fin(&file);

    if (playing)
    {
        timer->stop();
        auto rb = QMessageBox::question(this, "Confirm", "Are you sure to abort current game?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (!ui->btnPause->isChecked())
            timer->start();
        if (rb == QMessageBox::No)
            return;
    }

    if (!playing)
        game = new Sudoku;
    for (int i = 0; i < 9; ++i)
    {
        int num = 0;
        for (int j = 0; j < 9; ++j)
        {
            fin >> num;
            game->setElement(i, j, num);
        }
    }
    for (int i = 0; i < 9; ++i)
    {
        int num = 0;
        for (int j = 0; j < 9; ++j)
        {
            fin >> num;
            game->setVisibility(i, j, num == 1);
            if (num)
                grid[i][j]->setFont(QFont("Microsoft YaHei Light", 18, 50));
            else
                grid[i][j]->setFont(QFont("Microsoft YaHei", 18, 75));
        }
    }
    for (int i = 0; i < 9; ++i)
    {
        int num = 0;
        for (int j = 0; j < 9; ++j)
        {
            fin >> num;
            grid[i][j]->setText(num ? QString::number(num) : "");
        }
    }
    fin >> top >> top_origin;
    for (int i = 0, tmp; i < top_origin; ++i)
    {
        fin >> op[i].x >> op[i].y >> op[i].val >> tmp;
        op[i].isFillin = (tmp == 1);
    }
    fin >> Seconds;
    file.close();

    ui->frame->setVisible(true);
    for (int i = 1; i <= 9; ++i)
        sel[i]->setVisible(true);
    eraser->setVisible(true);
    for (int i = 1; i <= 9; ++i)
    {
        sel[i]->setEnabled(true);
        sel[i]->setChecked(false);
    }
    eraser->setEnabled(true);
    eraser->setChecked(false);
    playing = true;
    SelectedNumber = 0;
    Waiting = false;
    wait_x = wait_y = -1;

    this->Paint();

    ui->lblPaused->setVisible(false);
    ui->btnPause->setEnabled(true);
    ui->btnPause->setChecked(false);
    ui->actionSave->setEnabled(true);

    if (timer != NULL)
        delete timer;
    timer = new QTimer(this);
    ui->lcdMin->display(Prefix(Seconds / 60));
    ui->lcdSec->display(Prefix(Seconds % 60));
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerTimeout()));
    timer->setInterval(1000);
    timer->start();

    ui->menuCheat->setEnabled(true);
    ui->actionUndo->setEnabled(top > 0);
    ui->actionRedo->setEnabled(top < top_origin);
}
void MainWindow::SaveGame(bool showMsg)
{
    if (!playing)
        return;

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir pathdir(path);
    if (!pathdir.exists())
        pathdir.mkdir(path);
    path += "/recent.txt";

    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream fout(&file);

    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            fout << game->getElementCheat(i, j) << " ";
    fout << endl;
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            fout << (game->getElement(i, j) ? 1 : 0) << " ";
    fout << endl;
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            fout << grid[i][j]->text().toInt() << " ";
    fout << endl;
    fout << top << " " << top_origin << endl;
    for (int i = 0; i < top_origin; ++i)
        fout << op[i].x << " " << op[i].y << " "
             << op[i].val << " " << (op[i].isFillin ? 1 : 0) << endl;
    fout << Seconds << endl;
    file.close();

    timer->stop();
    if (showMsg)
        QMessageBox::information(this, "Success", "Progress saved.", QMessageBox::Ok);
    if (!ui->btnPause->isChecked())
        timer->start();
}
void MainWindow::SaveGameSlot() { this->SaveGame(true); }

void MainWindow::Undo()
{
    --top;
    if (op[top].isFillin)
        grid[op[top].x][op[top].y]->setText("");
    else
        grid[op[top].x][op[top].y]->setText(QString::number(op[top].val));
    this->Paint();

    ui->actionRedo->setEnabled(true);
    if (!top)
        ui->actionUndo->setEnabled(false);
}
void MainWindow::Redo()
{
    if (op[top].isFillin)
        grid[op[top].x][op[top].y]->setText(QString::number(op[top].val));
    else
        grid[op[top].x][op[top].y]->setText("");
    this->Paint();

    ++top;
    ui->actionUndo->setEnabled(true);
    if (top == top_origin)
        ui->actionRedo->setEnabled(false);
}

bool MainWindow::CheckGame()
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            if (grid[i][j]->text().toInt() != game->getElementCheat(i, j))
                return false;
    return true;
}

void MainWindow::Paint()
{
    //check if game ends
    if (this->CheckGame())
    {
        this->Clear();
        if (timer != NULL && timer->isActive())
            timer->stop();
        playing = false;

        ui->menuCheat->setEnabled(false);
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->actionSave->setEnabled(false);
        ui->btnPause->setEnabled(false);
        ui->btnPause->setChecked(false);

        for (int i = 1; i <= 9; ++i)
            sel[i]->setEnabled(false);
        eraser->setEnabled(false);

        Seconds = ui->lcdMin->value() * 60 + ui->lcdSec->value();
        QMessageBox::information(NULL, "Congratulations", "You have finished the sudoku!", QMessageBox::Ok);

        return;
    }

    //current status validation
    int r[10][10], c[10][10], s[10][10];
    memset(r, 0, sizeof(r));
    memset(c, 0, sizeof(c));
    memset(s, 0, sizeof(s));

    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
        {
            QString str = grid[i][j]->text();
            int value = str.toInt();
            if (value)
            {
                ++r[i][value];
                ++c[j][value];
                ++s[i / 3 * 3 + j / 3][value];
            }
        }

    //change check-box status
    int cnt[10];
    memset(cnt, 0, sizeof(cnt));
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            ++cnt[grid[i][j]->text().toInt()];
    for (int i = 1; i <= 9; ++i)
        if (cnt[i] == 9)
            sel[i]->setEnabled(false);
        else
            sel[i]->setEnabled(true);

    //change push-button appearance
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
        {
            int value = grid[i][j]->text().toInt();
            bool bordered = false, mistaken = false;
            QString stylesheet = "";

            if ((r[i][value] > 1 || c[j][value] > 1 || s[i / 3 * 3 + j / 3][value] > 1) && game->getElement(i, j) == 0)
            {
                stylesheet += "background-color: #FFD0D0;";
                mistaken = true;
            }
            if (Waiting)
            {
                if (i == wait_x && j == wait_y)
                {
                    stylesheet += "border-color: #51BC51;\
                                   border-width: 3px;\
                                   border-style: solid;";
                    bordered = true;
                }
                else if (!mistaken && (i == wait_x || j == wait_y))
                    stylesheet += "background-color: #FFFDE0;";
            }
            if (!bordered && value == SelectedNumber && value && !(i == wait_x && j == wait_y))
                stylesheet += "border-color: #39719D;\
                               border-width: 2px;\
                               border-style: solid;";
            grid[i][j]->setCrossStatus(ui->actionShowMistakes->isChecked() && value && value != game->getElementCheat(i, j));
            grid[i][j]->setStyleSheet(stylesheet);
            grid[i][j]->update();
        }
}

void MainWindow::Clear()
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            grid[i][j]->setStyleSheet("");
}

void MainWindow::React(int num)
{
    if (!playing)
        return;
    if (num < 0)   //check-box clicked
    {
        if (Waiting)
        {
            Waiting = false;
            if (num != -999)
            {
                sel[-num]->setChecked(false);
                grid[wait_x][wait_y]->setText(QString::number(-num));

                op[top++] = Operation(wait_x, wait_y, -num);
                top_origin = top;
                ui->actionRedo->setEnabled(false);
                ui->actionUndo->setEnabled(true);
            }
            wait_x = wait_y = -1;
            SelectedNumber = 0;

            this->Paint();
            return;
        }
        if (num != -999)  //numbers
        {
            if (sel[-num]->isChecked())
            {
                SelectedNumber = -num;
                for (int i = 1; i <= 9; ++i)
                    if (i != SelectedNumber)
                        sel[i]->setChecked(false);
                eraser->setChecked(false);
            }
            else
                SelectedNumber = 0;
        }
        else  //eraser
        {
            if (eraser->isChecked())
            {
                SelectedNumber = -num;
                for (int i = 1; i <= 9; ++i)
                    sel[i]->setChecked(false);
            }
            else
                SelectedNumber = 0;
        }
    }
    else
    {
        int x = num / 9, y = num % 9;
        if (game->getElement(x, y) != 0)      //clicked fixed block
        {
            for (int i = 1; i <= 9; ++i)
                sel[i]->setChecked(false);
            eraser->setChecked(false);

            if (SelectedNumber != game->getElement(x, y))
                SelectedNumber = game->getElement(x, y);
            else
                SelectedNumber = 0;
            Waiting = false;
            wait_x = wait_y = -1;

            this->Paint();
            return;
        }

        int checkBoxStatus = 0;               //observe check-boxes
        for (int i = 1; i <= 9; ++i)
            if (sel[i]->isChecked())
            {
                checkBoxStatus = i;
                break;
            }
        if (eraser->isChecked())
            checkBoxStatus = 999;

        if (!checkBoxStatus)                  //no checked check-box
        {
            if (Waiting && wait_x == x && wait_y == y)
            {
                Waiting = false;
                wait_x = wait_y = -1;
                SelectedNumber = 0;
            }
            else
            {
                Waiting = true;
                wait_x = x, wait_y = y;
                SelectedNumber = grid[x][y]->text().toInt();
            }
            this->Paint();
            return;
        }

        if (checkBoxStatus != 999 && checkBoxStatus != grid[x][y]->text().toInt())
        {
            op[top++] = Operation(x, y, checkBoxStatus);
            grid[x][y]->setText(QString::number(checkBoxStatus));
        }
        else if ((checkBoxStatus == 999 && grid[x][y]->text().toInt()) || (checkBoxStatus && checkBoxStatus == grid[x][y]->text().toInt()))
        {
            op[top++] = Operation(x, y, grid[x][y]->text().toInt(), false);
            grid[x][y]->setText("");
        }
        top_origin = top;
        ui->actionRedo->setEnabled(false);
        ui->actionUndo->setEnabled(top > 0);
    }
    this->Paint();
}

void MainWindow::PrepareGame()
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
        {
            int k = game->getElement(i, j);
            if (k)
            {
                grid[i][j]->setText(QString::number(k));
                grid[i][j]->setFont(QFont("Microsoft YaHei Light", 18, 50));
            }
            else
            {
                grid[i][j]->setText("");
                grid[i][j]->setFont(QFont("Microsoft YaHei", 18, 75));
            }
            grid[i][j]->update();
        }

    ui->frame->setVisible(true);
    for (int i = 1; i <= 9; ++i)
        sel[i]->setVisible(true);
    eraser->setVisible(true);
    for (int i = 1; i <= 9; ++i)
    {
        sel[i]->setEnabled(true);
        sel[i]->setChecked(false);
    }
    eraser->setEnabled(true);
    eraser->setChecked(false);
    playing = true;
    SelectedNumber = 0;
    Waiting = false;
    wait_x = wait_y = -1;

    this->Paint();

    ui->lblWait->setText("");
    ui->lblWait->update();

    ui->lblPaused->setVisible(false);
    ui->btnPause->setEnabled(true);
    ui->btnPause->setChecked(false);
    ui->actionSave->setEnabled(true);

    if (timer != NULL)
        delete timer;
    timer = new QTimer(this);
    Seconds = 0;
    ui->lcdMin->display("00");
    ui->lcdSec->display("00");
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerTimeout()));
    timer->setInterval(1000);
    timer->start();

    ui->menuCheat->setEnabled(true);
    ui->actionRedo->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    top = top_origin = 0;
}

void MainWindow::NewTrivialGame() { this->NewGame(20); }
void MainWindow::NewEasyGame()    { this->NewGame(30); }
void MainWindow::NewMediumGame()  { this->NewGame(40); }
void MainWindow::NewHardGame()    { this->NewGame(50); }
void MainWindow::NewExpertGame()  { this->NewGame(81); }

void MainWindow::NewGame(int blanks)
{
    if (timer != NULL && timer->isActive())
    {
        timer->stop();
        auto rb = QMessageBox::question(NULL, "Confirm", "Are you sure to abort current game?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        timer->start();
        if (rb == QMessageBox::No)
            return;
    }

    ui->lblWait->setText("Please wait...");
    ui->lblWait->update();

    game = new Sudoku;
    game->generate(blanks);

    this->PrepareGame();
}

void MainWindow::LoadGame()
{
    if (timer != NULL && timer->isActive())
    {
        timer->stop();
        auto rb = QMessageBox::question(NULL, "Confirm", "Are you sure to abort current game?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        timer->start();
        if (rb == QMessageBox::No)
            return;
    }
    if (timer != NULL)
        timer->stop();
    QFileDialog dialog(this);
    dialog.setWindowTitle(tr("Open Problem File"));
    dialog.setDirectory(QCoreApplication::applicationDirPath());
    dialog.setNameFilter(tr("Sudoku Problem Files(*.problem)"));
    if (dialog.exec() == QDialog::Accepted)
    {
        game = new Sudoku;

        QString path = dialog.selectedFiles()[0];
        QFile file(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream fin(&file);
        for (int i = 0; i < 9; ++i)
        {
            int num = 0;
            for (int j = 0; j < 9; ++j)
            {
                fin >> num;
                game->setElement(i, j, num);
            }
        }
        for (int i = 0; i < 9; ++i)
        {
            int num = 0;
            for (int j = 0; j < 9; ++j)
            {
                fin >> num;
                game->setVisibility(i, j, num == 1);
            }
        }
        file.close();
        this->PrepareGame();
        return;
    }
    if (timer != NULL)
        timer->start();
}

void MainWindow::SolveGame()
{
    playing = false;
    Waiting = false;
    wait_x = wait_y = -1;
    timer->stop();

    ui->menuCheat->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->btnPause->setEnabled(false);
    ui->btnPause->setChecked(false);

    for (int i = 1; i <= 9; ++i)
        sel[i]->setEnabled(false);
    eraser->setEnabled(false);

    this->Clear();
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            grid[i][j]->setText(QString::number(game->getElementCheat(i, j)));
}

void MainWindow::Correction() { this->Paint(); }
void MainWindow::TimerTimeout()
{
    ++Seconds;
    ui->lcdMin->display(Prefix(Seconds / 60));
    ui->lcdSec->display(Prefix(Seconds % 60));
}
