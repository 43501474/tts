#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Player.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextEdit>
#include <QByteArray>
#include <QTextCodec>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    ,ui(new Ui::MainWindow)
	,m_pPlayer(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete m_pPlayer;
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QString sFileName = QFileDialog::getOpenFileName(this, "Open file", ".", "Text files (*.txt)");
	//QString sFileName = "test.txt";
	delete m_pPlayer;
	m_pPlayer = new CPlayer(sFileName);
    QTextEdit* pTextEdit = dynamic_cast<QTextEdit*>(ui->centralContent);
    if (pTextEdit == nullptr)
        return;

    //pTextEdit->setText(m_pPlayer->fileContent());
	m_pPlayer->play();
}
