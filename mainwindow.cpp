#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&tarns_thread, &transform::trans_msg, this, &MainWindow::trans_msg);
    connect(&tarns_thread, &transform::recover_msg, this, &MainWindow::recover_msg);
    ui->statusbar->addWidget(&status_label);
    status_label.setText("welcome");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::trans_msg(const int &per, const int remain)
{
    ui->progressBar->setValue(per);
    if(!remain)
    {
        QMessageBox::information(NULL, "Transform was completed", "Enjoy your life", QMessageBox::Yes, QMessageBox::Yes);
    }
}

void MainWindow::recover_msg(const int &per, const int remain)
{
    ui->progressBar_2->setValue(per);
    if(!remain)
    {
        QMessageBox::information(NULL, "File recover was completed", "Enjoy your life", QMessageBox::Yes, QMessageBox::Yes);
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("选择源文件"), 0, tr("打开类型 (*.*)"));
    ui->lineEdit_file->setText(filename);
    ui->lineEdit_dstfile->setText(filename + "_bkp");
    src_file.setFileName(filename);
    if(src_file.open(QFile::ReadOnly))
    {
        src_data.clear();
        src_data = src_file.readAll();
        src_file.close();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    status_label.setText("转换文件");
    QFile dst_file;
    dst_file.setFileName(ui->lineEdit_dstfile->text());
    if(dst_file.open(QIODevice::WriteOnly))
    {
        dst_file.write(src_data);
        dst_file.close();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择目录"));
    ui->lineEdit_dir->setText(dir);
}

void MainWindow::scan_dir(QString path, QFileInfoList *list, QStringList *nameFilters)
{
    //判断路径是否存在
    status_label.setText("扫描目录...");
    QDir dir(path);
    if(!dir.exists())
    {
        return;
    }
    qDebug() << "Scan dir: " << path;
    if(nameFilters)
    {
        *list += dir.entryInfoList(*nameFilters, QDir::Dirs | QDir::Files | QDir::NoSymLinks);
    }
    else
    {
        *list += dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks);
    }

    QFileInfoList sub_dir_list = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks);
    if(sub_dir_list.length())
    {
        foreach (QFileInfo fileInfo, sub_dir_list)
        {
            if(fileInfo.fileName() != "." && fileInfo.fileName() != "..")
                scan_dir(fileInfo.absoluteFilePath(), list, nameFilters);
        }
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    QString file_type;
    QFileInfoList file_info_list;

    if(ui->lineEdit_type->text().length())
    {
        file_type = ui->lineEdit_type->text();
        QStringList type_list = file_type.split(";"); //"*.c"
        scan_dir(ui->lineEdit_dir->text(), &file_info_list, &type_list);
    }
    else
    {
        scan_dir(ui->lineEdit_dir->text(), &file_info_list);
    }
    status_label.setText("转换目录...");
    tarns_thread.transaction(ui->lineEdit_dir->text(), QCoreApplication::applicationDirPath() + "/tmp", &file_info_list);
}

void MainWindow::on_pushButton_4_clicked()
{
    QFileInfoList file_info_list;

    scan_dir(ui->lineEdit_recover->text(), &file_info_list);
    /* recover file */
    status_label.setText("恢复后缀...");
    tarns_thread.recover(&file_info_list);
}

void MainWindow::on_pushButton_6_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择目录"));
    ui->lineEdit_recover->setText(dir);
}
