#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QThread>
#include <QFileInfoList>

#define TRANSFORM_SUFFIX "_tmp"

class transform : public QThread
{
    Q_OBJECT
public:
    explicit transform(QObject *parent = nullptr);
    ~transform();
    void run() Q_DECL_OVERRIDE;
    void transaction(QString src_dir, QString dst_dir, QFileInfoList *list);
    void recover(QFileInfoList *list);

signals:
    void trans_msg(const int &per, const int remain);
    void recover_msg(const int &per, const int remain);

private:
    void transform_dir(QString src_dir, QString dst_dir, QFileInfoList *list);
    void recover_dir(QFileInfoList *list);
    QString src_path;
    QString dst_path;
    QFileInfoList file_list;
};

#endif // TRANSFORM_H
