#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QFile>
#include <QFileInfoList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_6_clicked();

private:
    void scan_dir(QString path, QFileInfoList *list, QStringList *nameFilters = NULL);
    void transform_dir(QString src_dir, QString dst_dir, QFileInfoList *list);
    void recover_dir(QString path, QStringList nameFilters, QFileInfoList *list);
    Ui::MainWindow *ui;
    QFile src_file;
    QByteArray src_data;
};
#endif // MAINWINDOW_H
