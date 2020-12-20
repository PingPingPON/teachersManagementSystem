#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    connectSQLite();
    createDB();
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/icon"));
    this->setWindowTitle("教师管理系统");

    initial();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initial() {
    setLineEdit();
    designLogin();
    designAttending();
    designHire();
    designChangeState();

    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->tabBar()->hide();

    connect(ui->pb_confirm_login, SIGNAL(clicked(bool)), this, SLOT(signInConfirm()));
    connect(ui->pb_cancel_login, SIGNAL(clicked(bool)), this, SLOT(signInCancel()));
    connect(ui->pb_search, SIGNAL(clicked(bool)), this, SLOT(updateAttending()));
    connect(ui->pb_hire_input_confirm, SIGNAL(clicked(bool)), this, SLOT(hireConfirm()));
    connect(ui->pb_hire_input_cancel, SIGNAL(clicked(bool)), this, SLOT(hireCancel()));
    connect(ui->pb_hire_output_clear, SIGNAL(clicked(bool)), this, SLOT(hireInformationClear()));
    connect(ui->pb_change_state_search, SIGNAL(clicked(bool)), this, SLOT(changeStateSearch()));
    connect(ui->pb_change_state_fire, SIGNAL(clicked(bool)), this, SLOT(changeStateFire()));
    connect(ui->pb_change_state_vacation, SIGNAL(clicked(bool)), this, SLOT(changeStateVacation()));
    connect(ui->pb_change_state_rework, SIGNAL(clicked(bool)), this, SLOT(changeStateRework()));
    connect(ui->pb_change_state_retire, SIGNAL(clicked(bool)), this, SLOT(changeStateRetire()));

    qDebug() << "Start success.";
}

void MainWindow::designLogin() {
    QPixmap bobo(":/bobo");
    ui->lb_bobo->setPixmap(bobo);
    ui->lb_bobo->show();

}

void MainWindow::setLineEdit() {
    ui->le_user_name->setMaxLength(20);
    ui->le_password->setEchoMode(QLineEdit::Password);

    ui->le_hire_output_id->setReadOnly(true);
    ui->le_hire_output_name->setReadOnly(true);
    ui->le_hire_output_rank->setReadOnly(true);
    ui->le_hire_output_subject->setReadOnly(true);
}

void MainWindow::attending(QString & query) {
    model_tv_attending->setQuery(query);
    ui->tv_attending->setModel(model_tv_attending);
}

void MainWindow::designAttending() {
    model_tv_attending = new QSqlQueryModel(ui->tv_attending);
    QString query = QString("select * from teachers;");
    attending(query);
    model_tv_attending->setHeaderData(0, Qt::Horizontal, tr("编号"));
    model_tv_attending->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    model_tv_attending->setHeaderData(2, Qt::Horizontal, tr("职称"));
    model_tv_attending->setHeaderData(3, Qt::Horizontal, tr("科目"));
    model_tv_attending->setHeaderData(4, Qt::Horizontal, tr("入职日期"));
    model_tv_attending->setHeaderData(5, Qt::Horizontal, tr("在职状态"));
    ui->tv_attending->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::designHire() {
    ui->lb_hire_output->hide();
    ui->pb_hire_output_clear->hide();
}

void MainWindow::changeState(QString & query) {
    model_tv_change_state->setQuery(query);
    ui->tv_change_state->setModel(model_tv_change_state);
}

void MainWindow::designChangeState() {
    model_tv_change_state = new QSqlQueryModel(ui->tv_change_state);

    QString query = QString("select * from teachers;");
    changeState(query);
    model_tv_change_state->setHeaderData(0, Qt::Horizontal, tr("编号"));
    model_tv_change_state->setHeaderData(1, Qt::Horizontal, tr("姓名"));
    model_tv_change_state->setHeaderData(2, Qt::Horizontal, tr("职称"));
    model_tv_change_state->setHeaderData(3, Qt::Horizontal, tr("科目"));
    model_tv_change_state->setHeaderData(4, Qt::Horizontal, tr("入职日期"));
    model_tv_change_state->setHeaderData(5, Qt::Horizontal, tr("在职状态"));
    ui->tv_change_state->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::signInConfirm() {
    QSqlQuery query;
    QString id = ui->le_user_name->text();
    QString password = ui->le_password->text();

    QString s = QString("select * from administrator where id = '%1' and password = '%2';").arg(id).arg(password);
    query.exec(s);

    if (query.first()) {
        QMessageBox::information(NULL, "登录成功", "登录成功", QMessageBox::Yes);
        login = true;
        ui->pb_confirm_login->setEnabled(false);
        ui->pb_cancel_login->setEnabled(false);
        ui->tabWidget->tabBar()->show();
    }
    else
        QMessageBox::warning(NULL, "Error", "登录失败", "重试");
}

void MainWindow::signInCancel() {
    ui->le_user_name->clear();
    ui->le_password->clear();
}

void MainWindow::updateAttending() {
    const int index0 = ui->cmbBx_search_rank->currentIndex();
    const int index1 = ui->cmbBx_search_subject->currentIndex();
    const int index2 = ui->cmbBx_search_state->currentIndex();
    QString query = QString("select * from teachers ");
    int count = 0;
    if (index0 || index1 || index2) {
        query += "where ";
        if (index0){
            if (count)
                query += "and ";
            query += QString("rank='%1' ").arg(ui->cmbBx_search_rank->currentText());
            ++count;
        }
        if (index1){
            if (count)
                query += "and ";
            query += QString("subject='%1' ").arg(ui->cmbBx_search_subject->currentText());
            ++count;
        }
        if (index2){
            if (count)
                query += "and ";
            query += QString("state='%1' ").arg(ui->cmbBx_search_state->currentText());
            ++count;
        }
    }
    query += ";";
    qDebug() << query;
    attending(query);
}

void MainWindow::hireConfirm() {
    QString name = ui->le_hire_input_name->text();
    const int rank_index = ui->cmbBx_hire_input_rank->currentIndex();
    const int subject_index = ui->cmbBx_hire_input_subject->currentIndex();

    if (name.isEmpty() || !rank_index || !subject_index) {
        QMessageBox::warning(NULL, "warning", "输入项不可有空", "重试");
        return;
    }
    QString id;
    QString rank = ui->cmbBx_hire_input_rank->currentText();
    QString subject = ui->cmbBx_hire_input_subject->currentText();
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString state = "在职";
    QSqlQuery query;
    QString s = QString(
                "insert into teachers ("
                "name, rank, subject, hireDate, state"
                ") values ("
                "'%1', '%2', '%3', '%4', '%5'"
                ");")
            .arg(name)
            .arg(rank)
            .arg(subject)
            .arg(date)
            .arg(state);
    qDebug() << s;
    query.exec(s);
    updateAttending();
    changeStateSearch();

    if (database.lastError().isValid())
    {
        qDebug() << database.lastError();
        return;
    }
    QMessageBox::information(NULL, "聘用成功", "聘用成功", QMessageBox::Yes);

    s = QString("select * from teachers where id=last_insert_rowid();");
    if (query.exec(s))
        while(query.next()) {
            id = query.value(0).toString();
            name = query.value(1).toString();
            rank = query.value(2).toString();
            subject = query.value(3).toString();
            date = query.value(4).toString();
            state = query.value(5).toString();
        }
    else {
        qDebug() << query.lastError();
        return;
    }
    ui->le_hire_input_name->clear();
    ui->cmbBx_hire_input_rank->setCurrentIndex(0);
    ui->cmbBx_hire_input_subject->setCurrentIndex(0);

    ui->lb_hire_output->show();
    ui->pb_hire_output_clear->show();
    ui->le_hire_output_id->setText(id);
    ui->le_hire_output_name->setText(name);
    ui->le_hire_output_rank->setText(rank);
    ui->le_hire_output_subject->setText(subject);
    ui->le_hire_output_hireDate->setText(date);
}

void MainWindow::hireCancel() {
    ui->le_hire_input_name->clear();
    ui->cmbBx_hire_input_rank->setCurrentIndex(0);
    ui->cmbBx_hire_input_subject->setCurrentIndex(0);
}

void MainWindow::hireInformationClear() {
    ui->lb_hire_output->hide();
    ui->le_hire_output_id->clear();
    ui->le_hire_output_name->clear();
    ui->le_hire_output_rank->clear();
    ui->le_hire_output_subject->clear();
    ui->le_hire_output_hireDate->clear();
    ui->pb_hire_output_clear->hide();
}

void MainWindow::changeStateSearch() {
    const int index0 = ui->cmbBx_change_state_rank->currentIndex();
    const int index1 = ui->cmbBx_change_state_subject->currentIndex();
    const int index2 = ui->cmbBx_change_state_state->currentIndex();
    const QString id = ui->le_change_state_id->text();
    const QString name = ui->le_change_state_name->text();
    QString query = QString("select * from teachers ");
    int count = 0;
    if (index0 || index1 || index2 || !id.isEmpty() || !name.isEmpty()) {
        query += "where ";
        if (index0) {
            if (count)
                query += "and ";
            query += QString("rank='%1' ").arg(ui->cmbBx_change_state_rank->currentText());
            ++count;
        }
        if (index1) {
            if (count)
                query += "and ";
            query += QString("subject='%1' ").arg(ui->cmbBx_change_state_subject->currentText());
            ++count;
        }
        if (index2) {
            if (count)
                query += "and ";
            query += QString("state='%1' ").arg(ui->cmbBx_change_state_state->currentText());
            ++count;
        }
        if (!id.isEmpty()) {
            if (count)
                query += "and ";
            query += QString("id='%1' ").arg(id);
            ++count;
        }
        if (!name.isEmpty()) {
            if (count)
                query += "and ";
            query += QString("name='%1' ").arg(name);
            ++count;
        }
    }
    query += ";";
    qDebug() << query;
    changeState(query);
    ui->le_change_state_id->clear();
    ui->le_change_state_name->clear();
    ui->cmbBx_change_state_rank->setCurrentIndex(0);
    ui->cmbBx_change_state_subject->setCurrentIndex(0);
    ui->cmbBx_change_state_state->setCurrentIndex(0);
}

void MainWindow::changeStateSelection(int & row, int & column) {
    row = ui->tv_change_state->selectionModel()->currentIndex().row();
    column = ui->tv_change_state->selectionModel()->currentIndex().column();
    qDebug() << row <<column;
    ui->tv_change_state->selectionModel()->clear();
}

void MainWindow::changeStateFire() {
    int row;
    int column;
    changeStateSelection(row, column);
    if (row < 0 || column < 0) {
        QMessageBox::warning(NULL, "warning", "请选中一行", "重试");
        return;
    }
    auto* model = ui->tv_change_state->model();
    const QString id = model->data(model->index(row, 0)).toString();
    const QString state = model->data(model->index(row, 5)).toString();
    const QString newState = "离职";
    if (state == "离职" || state == "退休") {
        QMessageBox::warning(NULL, "warning", "该教师已退休或离职", "重试");
        return;
    }
    QSqlQuery query;
    QString s;
    s = QString("update teachers set state='%1' where id='%2';").arg(newState).arg(id);
    qDebug() << s;
    query.exec(s);
    updateAttending();
    changeStateSearch();

    if (database.lastError().isValid())
    {
        qDebug() << database.lastError();
        return;
    }
    QMessageBox::information(NULL, "离职成功", "离职成功", QMessageBox::Yes);
}

void MainWindow::changeStateVacation() {
    int row;
    int column;
    changeStateSelection(row, column);
    if (row < 0 || column < 0) {
        QMessageBox::warning(NULL, "warning", "请选中一行", "重试");
        return;
    }
    auto* model = ui->tv_change_state->model();
    const QString id = model->data(model->index(row, 0)).toString();
    const QString state = model->data(model->index(row, 5)).toString();
    const QString newState = "休假";
    if (state != "在职") {
        QMessageBox::warning(NULL, "warning", "该教师正在休假或已离职或退休", "重试");
        return;
    }
    QSqlQuery query;
    QString s;
    s = QString("update teachers set state='%1' where id='%2';").arg(newState).arg(id);
    qDebug() << s;
    query.exec(s);
    updateAttending();
    changeStateSearch();

    if (database.lastError().isValid())
    {
        qDebug() << database.lastError();
        return;
    }
    QMessageBox::information(NULL, "休假成功", "休假成功", QMessageBox::Yes);
}

void MainWindow::changeStateRework() {
    int row;
    int column;
    changeStateSelection(row, column);
    if (row < 0 || column < 0) {
        QMessageBox::warning(NULL, "warning", "请选中一行", "重试");
        return;
    }
    auto* model = ui->tv_change_state->model();
    const QString id = model->data(model->index(row, 0)).toString();
    const QString state = model->data(model->index(row, 5)).toString();
    const QString newState = "在职";
    if (state != "休假") {
        QMessageBox::warning(NULL, "warning", "该教师未在休假", "重试");
        return;
    }
    QSqlQuery query;
    QString s;
    s = QString("update teachers set state='%1' where id='%2';").arg(newState).arg(id);
    qDebug() << s;
    query.exec(s);
    updateAttending();
    changeStateSearch();

    if (database.lastError().isValid())
    {
        qDebug() << database.lastError();
        return;
    }
    QMessageBox::information(NULL, "返岗成功", "返岗成功", QMessageBox::Yes);
}

void MainWindow::changeStateRetire() {
    int row;
    int column;
    changeStateSelection(row, column);
    if (row < 0 || column < 0) {
        QMessageBox::warning(NULL, "warning", "请选中一行", "重试");
        return;
    }
    auto* model = ui->tv_change_state->model();
    const QString id = model->data(model->index(row, 0)).toString();
    const QString state = model->data(model->index(row, 5)).toString();
    const QString newState = "退休";
    if (state == "离职" || state == "退休") {
        QMessageBox::warning(NULL, "warning", "该教师已离职或退休", "重试");
        return;
    }
    QSqlQuery query;
    QString s;
    s = QString("update teachers set state='%1' where id='%2';").arg(newState).arg(id);
    qDebug() << s;
    query.exec(s);
    updateAttending();
    changeStateSearch();

    if (database.lastError().isValid())
    {
        qDebug() << database.lastError();
        return;
    }
    QMessageBox::information(NULL, "退休成功", "退休成功", QMessageBox::Yes);
}

void MainWindow::connectSQLite() {
    database=QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("teachers_managerment_system.db");
    if(!database.open()) {
        qDebug() << database.lastError();
        return;
    }
}

void MainWindow::createDB() {
    if (!database.isOpen()) {
        qDebug() << database.lastError();
        return;
    }
    database.exec("create table if not exists teachers"
        "("
        "id       integer primary key autoincrement,"
        "name     text not null,"
        "rank     varchar(20) check ( rank in ('正高级教师', '高级教师', '一级教师', '二级教师', '三级教师')),"
        "subject  text check ( subject in ('语文', '数学', '外语', '物理', '化学', '生物', '政治', '历史', '地理', '通用技术', '信息技术', '体育', '美术', '音乐') ),"
        "hireDate date not null,"
        "state    varchar(20) check ( state in ('在职', '休假', '离职', '退休') ) not null"
        ");");

    if (database.lastError().isValid())
    {
        qDebug() << database.lastError();
        return;
    }
}
