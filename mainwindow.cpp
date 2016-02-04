#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextEdit>
#include <QByteArray>
#include <QTextCodec>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QString sFileName = QFileDialog::getOpenFileName(this, "Open file", ".", "Text files (*.txt)");
    QFile f(sFileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&f);
    in.setCodec("GB18030");

    m_sFileContent = in.readAll();
    QTextEdit* pTextEdit = dynamic_cast<QTextEdit*>(ui->centralContent);
    if (pTextEdit == nullptr)
        return;

    pTextEdit->setText(m_sFileContent.left(10000)); // TODO: encoding things
}
