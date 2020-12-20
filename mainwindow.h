#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void signInConfirm();
    void signInCancel();
    void updateAttending();
    void hireConfirm();
    void hireCancel();
    void hireInformationClear();
    void changeStateSearch();
    void changeStateFire();
    void changeStateVacation();
    void changeStateRework();
    void changeStateRetire();

private:
    Ui::MainWindow *ui;
    bool login = false;

    void connectSQLite();
    void createDB();

    QSqlDatabase database;
    QSqlQueryModel *model_tv_attending;
    QSqlQueryModel *model_tv_change_state;

    void initial();
    void designLogin();
    void setLineEdit();
    void attending(QString &);
    void designAttending();
    void designHire();
    void changeState(QString &);
    void designChangeState();
    void changeStateSelection(int &, int &);
};
#endif // MAINWINDOW_H
