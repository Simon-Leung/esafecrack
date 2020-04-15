#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#define TRANSFORM_SUFFIX "_tmp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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

//void MainWindow::on_pushButton_5_clicked()
//{
////    QFileInfo fileInfo;
//    QString file_type;
//    file_type = ui->lineEdit_type->text();
//    QStringList type_list = file_type.split(";");
//    QDir dir(ui->lineEdit_dir->text());
//    if(!dir.exists())
//        return;
//    QDir dir_dst(ui->lineEdit_dirdest->text());
//    if(!dir_dst.exists())
//        dir_dst.mkpath(ui->lineEdit_dirdest->text());
//    dir.setFilter(QDir::Dirs|QDir::Files);
//    dir.setSorting(QDir::DirsFirst);
////    QFileInfoList list = dir.entryInfoList(QDir::Files);
////    for (int i = 0; i < list.size(); ++i)
//    foreach (QFileInfo fileInfo, dir.entryList(QDir::Files))
//    {
//        int exist_type = 0;
//        //fileInfo = list.at(i);
//        if(!ui->lineEdit_type->text().length())
//            exist_type = 1;
//        else
//        {
//            for (int k = 0; k < type_list.count(); k++)
//            {
//                if(fileInfo.fileName().contains(type_list.at(k)))
//                {
//                    exist_type = 1;
//                    qDebug() << "find file type:" << type_list.at(k);
//                }
//            }
//        }
//        if(exist_type)
//        {
//            qDebug() << "find file:" << fileInfo.filePath();
//            QFile file;
//            file.setFileName(ui->lineEdit_dir->text() + "/" + fileInfo.filePath());
//            if(file.open(QFile::ReadOnly))
//            {
//                src_data.clear();
//                src_data = file.readAll();
//                file.close();
//                QFile dst_file;
//                QString src_file_path = fileInfo.filePath();
//                QString dst_file_name = ui->lineEdit_dirdest->text() + "/"  + src_file_path + "_bkp";
//                dst_file.setFileName(dst_file_name);
//                qDebug() << "save file:" << dst_file_name;
//                if(dst_file.open(QIODevice::WriteOnly | QIODevice::Text))
//                {
//                    dst_file.write(src_data);
//                    dst_file.close();
//                    //dst_file.rename(ui->lineEdit_dirdest->text() + src_file_path);
//                }
//            }
//        }
//    }
//}

void MainWindow::scan_dir(QString path, QFileInfoList *list, QStringList *nameFilters)
{
    //判断路径是否存在
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

void MainWindow::transform_dir(QString src_dir, QString dst_dir, QFileInfoList *list)
{
    QDir new_dir(dst_dir);
    if(!new_dir.exists())
        new_dir.mkpath(dst_dir);
    qDebug() << "transform file number " << list->length();
    /* mkpath subdir */
    foreach (QFileInfo file_info, *list)
    {
        if(file_info.isDir() && file_info.fileName() != "." && file_info.fileName() != "..")
        {
            QString sub_dir_path = dst_dir + file_info.absoluteFilePath().remove(0, src_dir.length());
            QDir sub_dir(sub_dir_path);
            if(!sub_dir.exists())
            {
                sub_dir.mkpath(sub_dir_path);
            }
            qDebug() << "mkpath " << sub_dir_path;
        }
    }
    /* transform file */
    foreach (QFileInfo file_info, *list)
    {
        if(file_info.isFile() && file_info.fileName() != "." && file_info.fileName() != "..")
        {
            QString save_file_name = dst_dir + file_info.absoluteFilePath().remove(0, src_dir.length()) + TRANSFORM_SUFFIX;
            QFile file_src;
            file_src.setFileName(file_info.absoluteFilePath());
            if(file_src.open(QFile::ReadOnly))
            {
                src_data.clear();
                src_data = file_src.readAll();
                file_src.close();
            }
            QFile file_dst;
            file_dst.setFileName(save_file_name);
            if(file_dst.open(QIODevice::WriteOnly))
            {
                file_dst.write(src_data);
                file_dst.close();
                src_data.clear();
            }
            qDebug() << "transform " << file_info.absoluteFilePath() << " to " << save_file_name;
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
    transform_dir(ui->lineEdit_dir->text(), QCoreApplication::applicationDirPath() + "/tmp", &file_info_list);
    QMessageBox::information(NULL, "Transform was completed", "Enjoy your life", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

void MainWindow::on_pushButton_4_clicked()
{
    QFileInfoList file_info_list;

    scan_dir(ui->lineEdit_recover->text(), &file_info_list);
    /* recover file */
    foreach (QFileInfo file_info, file_info_list)
    {
        if(file_info.isFile())
        {
            QFile file;
            if(file_info.fileName().contains(TRANSFORM_SUFFIX))
            {
                file.rename(file_info.absoluteFilePath(), file_info.absoluteFilePath().remove(file_info.absoluteFilePath().length() - 4, 4));
                qDebug() << "recover " << file_info.absoluteFilePath().remove(file_info.absoluteFilePath().length() - 4);
            }
        }
    }
    QMessageBox::information(NULL, "File recover was completed", "Enjoy your life", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

void MainWindow::on_pushButton_6_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择目录"));
    ui->lineEdit_recover->setText(dir);
}
