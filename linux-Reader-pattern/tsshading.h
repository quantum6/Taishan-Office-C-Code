#ifndef TSSHADING1_H
#define TSSHADING1_H

#define varName(x) #x

#include "TSShading_global.h"

#include <QString>
#include <QObject>

QString getQString(JNIEnv *env, jstring path);

typedef struct {
    double width = 0;
    double height = 0;
    QString path = "";
    double eWidth = 0;
    double eHeight = 0;
}ImageInfo;


class TSSHADING_EXPORT TSShading : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qlonglong x READ getX WRITE setX)
    Q_PROPERTY(qlonglong y READ getY WRITE setY)
    Q_PROPERTY(qlonglong width READ getWidth WRITE setWidth)
    Q_PROPERTY(qlonglong height READ getHeight WRITE setHeight)
    Q_PROPERTY(double cellW READ getCellW WRITE setCellW)
    Q_PROPERTY(double cellH READ getCellH WRITE setCellH)
    Q_PROPERTY(double xStep READ getXStep WRITE setXStep)
    Q_PROPERTY(double yStep READ getYStep WRITE setYStep)
    Q_PROPERTY(qlonglong reflect READ getReflect WRITE setReflect)
    Q_PROPERTY(bool relative READ getRelative WRITE setRelative)
    Q_PROPERTY(QString image READ getImage WRITE setImage)
    Q_PROPERTY(QString CTM READ getCTM WRITE setCTM)

    enum ReflectType{
        Normla,
        Colunm,
        Row,
        RowColunm
    };
public:
    TSShading();
    TSShading(JNIEnv *env, jobject obj);
    QByteArray getBase64Byte();
    QString getBase64();

    void init();

    float mm2px(float mm);
public :
    Q_INVOKABLE int getWidth() const;
    Q_INVOKABLE  void setWidth(int value);


    Q_INVOKABLE qlonglong getHeight() const;
    Q_INVOKABLE void setHeight(const qlonglong &value);

    Q_INVOKABLE double getCellW() const;
    Q_INVOKABLE void setCellW(const double &value);

    Q_INVOKABLE double getCellH() const;
    Q_INVOKABLE void setCellH(const double &value);

    Q_INVOKABLE double getXStep() const;
    Q_INVOKABLE void setXStep(const double &value);

    Q_INVOKABLE double getYStep() const;
    Q_INVOKABLE void setYStep(const double &value);

    Q_INVOKABLE qlonglong getReflect() const;
    Q_INVOKABLE void setReflect(const qlonglong &value);

    Q_INVOKABLE bool getRelative() const;
    Q_INVOKABLE void setRelative(bool value);

    Q_INVOKABLE QString getImage() const;
    Q_INVOKABLE void setImage(const QString &value);

    Q_INVOKABLE QString getCTM() const;
    Q_INVOKABLE void setCTM(const QString &value);

    Q_INVOKABLE qlonglong getX() const;
    Q_INVOKABLE void setX(const qlonglong&value);

    Q_INVOKABLE qlonglong getY() const;
    Q_INVOKABLE void setY(const qlonglong&value);

protected:
    QVariant getJAVAValue(QVariant value, QString name);


protected:
    qlonglong x = 0;
    qlonglong y = 0;
    qlonglong width = 0;
    qlonglong height = 0;
    double cellW = 0;
    double cellH = 0;
    double xStep = 0;
    double yStep = 0;
    qlonglong reflect = 0;
    QString image = "";
    QString CTM = "";
    bool relative = true;  //true object  fasle page


    JNIEnv* env;
    jobject jobj;
    jclass clazz;
    double tmpColum = 0;
};
//Q_DECLARE_METATYPE(TSShading)
#endif // TSSHADING1_H
