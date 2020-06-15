#include "transform.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDir>


transform::transform(QObject *parent) : QThread(parent)
{

}

transform::~transform()
{
}


void transform::transform_dir(QString src_dir, QString dst_dir, QFileInfoList *list)
{
    QByteArray src_data;
    int remain;
    QDir new_dir(dst_dir);
    if(!new_dir.exists())
        new_dir.mkpath(dst_dir);
    qDebug() << "transform file number " << list->length();
    remain = list->length();
    /* mkpath subdir */
    foreach (QFileInfo file_info, *list)
    {
        if(file_info.isDir())
        {
            if(file_info.fileName() != "." && file_info.fileName() != "..")
            {
                QString sub_dir_path = dst_dir + file_info.absoluteFilePath().remove(0, src_dir.length());
                QDir sub_dir(sub_dir_path);
                if(!sub_dir.exists())
                {
                    sub_dir.mkpath(sub_dir_path);
                }
                qDebug() << "mkpath " << sub_dir_path;
            }
            remain--;
            emit trans_msg((list->length() - remain) * 100 / list->length(), remain);
        }
    }
    /* transform file */
    foreach (QFileInfo file_info, *list)
    {
        if(file_info.isFile())
        {
            if(file_info.fileName() != "." && file_info.fileName() != "..")
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
            remain--;
            emit trans_msg((list->length() - remain) * 100 / list->length(), remain);
        }
    }
}

void transform::recover_dir(QFileInfoList *list)
{
    int remain;
    remain = list->length();
    foreach (QFileInfo file_info, *list)
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
        remain--;
        emit recover_msg((list->length() - remain) * 100 / list->length(), remain);
    }
}

void transform::run(void)
{
    if(dst_path.length())
        transform_dir(src_path, dst_path, &file_list);
    else
        recover_dir(&file_list);
}

void transform::transaction(QString src_dir, QString dst_dir, QFileInfoList *list)
{
    src_path = src_dir;
    dst_path = dst_dir;
    file_list = *list;
    if(!isRunning())
        start();
}

void transform::recover(QFileInfoList *list)
{
    src_path.clear();
    dst_path.clear();
    file_list = *list;
    if(!isRunning())
        start();
}
