#include "tsshading.h"
#include "QDebug"

#include <string>
#include <QBuffer>
#include <QMetaProperty>
#include <qfile.h>
#include <QImageWriter>

#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QImageReader>

//#include <QtPlugin>
//Q_IMPORT_PLUGIN(qpng)



QString getQString(JNIEnv *env, jstring path){

    //    printf("getQString \n");
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    //    printf("GetByteArrayElements0 \n");
    jstring strencode = env->NewStringUTF("utf-8");
    //    printf("GetByteArrayElements1 \n");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    //    printf("GetByteArrayElements2 \n");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod(path, mid, strencode);
    //    printf("GetByteArrayElements3 \n");
    jsize alen = env->GetArrayLength(barr);
    //    printf("GetByteArrayElements4 \n");
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    //    printf("GetByteArrayElements success \n");
    if (alen > 0)
    {
        rtn = (char*)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    //    LibTest lib;
    //    QString str = lib.getBase64();
    QString str(rtn);
    if(NULL != rtn){
        free(rtn);
    }
    return  str;
}

TSShading::TSShading()
{

}

TSShading::TSShading(JNIEnv *env, jobject obj):xStep(0),yStep(0)
{
    this->env = env;
    this->jobj = obj;
    clazz = env->GetObjectClass(obj);
    if(clazz == NULL){
        printf("create jclass fail \n");
        //        return  env->NewStringUTF(QString("create fail").toUtf8().data());s
    }
}

QByteArray TSShading::getBase64Byte()
{
    bool saveFeil = false;
    //数据校验
    if(this->image.length() == 0)
    {
        printf("can not read image\n");
        return "";
    }
    if(this->width <=0 || this->height <= 0)
    {
        printf("can not write image with with %lld height %lld",this->width,this->height);
        return "";
    }
    if(this->cellH <= 0 || this->cellW <= 0)
    {
        printf("can not write image cellwith with %lld cellheight %lld",this->cellW,this->cellH);
        return "";
    }
    QStringList CTMList;
    if(this->CTM.length()!=0)
    {
        CTMList = this->CTM.split(" ");
        if(CTMList.size() != 6)
        {
            printf("CTM is error : %s\n",CTM.toUtf8().data());
            return "";
        }
    }
    printf("CTM %s \n",CTM.toUtf8().data());
    for(int i = 0;i < CTMList.length();++i){
        printf("CTM[%d] %s\n",i,CTMList[i].toUtf8().data());
    }

    if(this->relative){
       x = 0;
       y = 0;
    }
//    printf()


    //先放大10倍，减小误差

    printf("current xStep : %f",xStep);
    if(xStep > cellW){
        xStep = xStep - cellW;
        printf("xStep > cellW current xStep : %f\n",xStep);
    }else{
        xStep = 0;
    }
    if(this->yStep > cellH){
        this->yStep = this->yStep - cellH;
    }else{
        this->yStep = 0;
    }
    cellW = cellW*10;
    cellH = cellH*10;
    xStep = (xStep)*10;
    yStep = yStep*10;
    printf("xStep : %f yStep : %f\n",xStep,yStep);
    //计算单元大小
    qlonglong pCellW = cellW;
    qlonglong pCellH = cellH;
    tmpColum = this->xStep;
    double yStep = 0;
//    byte1 a;
    printf("set relative is %d \n",this->relative);
    switch(((int)this->reflect)){
    case Normla:{
        printf("is colunm and xStep is %f self xStep is %f \n",xStep,tmpColum);
        pCellW = pCellW + xStep;
        pCellH += this->yStep;
        break;
    }
    case Colunm:
        {

            pCellW += cellW;
            //增加x、y方向上的步进值
//            tmpColum = (tmpColum * 2);
            printf("is colunm and xStep is %f self xStep is %f \n",xStep,tmpColum);
            pCellW = pCellW + xStep;
            pCellW = pCellW + xStep;
            pCellH += this->yStep;

            break;
        }
    case Row:
        {
            pCellH += cellH;
            //增加x、y方向上的步进值
            pCellW += this->xStep;
            yStep = this->yStep * 2;
            pCellH = pCellH + yStep;
            printf("is Row and yStep is %f \n",yStep);
            break;
        }
    case RowColunm:
        {
            pCellW += cellW;
            qDebug()<<"pCellW : "<<pCellW<<" cellW "<< cellW;
            pCellH += cellH;
            qDebug()<<"pCellH : "<<pCellH<<" cellH "<< cellH;
            //增加x、y方向上的步进值
            //            int yStep1 = this->yStep * 2;
//            qlonglong xStep = this->xStep * 2;
//            qDebug()<<"xStep1 : "<<yStep<<" xStep "<< xStep;
//            printf("xStep * 2 is ",)
            pCellW = (pCellW + (this->xStep));
            pCellW = (pCellW + (this->xStep));
            qDebug()<<"pCellW : "<<pCellW<<" xStep "<< xStep;
            pCellH += this->yStep * 2;
            qDebug()<<"pCellH : "<<pCellH<<" xStep "<< yStep;
            printf("is RowColunm and  xStep is %f yStep is %f \n",xStep,yStep);
            break;
        }
    default:
    {
        printf("default is RowColunm and  xStep is %f yStep is %f \n",xStep,yStep);
        break;
    }
    }


    //读入原始图片
    QByteArray imgbyte = QByteArray::fromBase64(this->image.toLocal8Bit());
    QImage oCell = QImage::fromData(imgbyte);
    bool saveRes = false;
    if(saveFeil)
    {
        saveRes = oCell.save("cello.jpg","jpg",100);
        printf("save cello result is %d\n",saveRes);//Format_ARGB32
    }
//    oCell = oCell.scaled(cellW,cellW);
    QImage cellImg = QImage(pCellW,pCellH,QImage::Format_ARGB32);
    printf("final xstep: %f ystep: %f \n",xStep,yStep);
    printf("final cell h : %lld w: %lld \n",pCellH,pCellW);
    cellImg.fill(Qt::transparent);
    //创建画笔
    QPainter painter(&cellImg);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法

    //    if(this->reflect == Colunm/* || this->reflect == RowColunm*/){

    //    }
    switch(this->reflect){
    case Normla:
        {
            QRectF boundary(0,0,this->cellW,this->cellH);
            painter.drawImage(boundary,oCell);
            break;
        }
    case Colunm:
        {
            QRectF boundary(0,0,this->cellW,this->cellH);
            painter.drawImage(boundary,oCell);
            QTransform tr;
            tr.scale(-1,1);
            QImage colImg = oCell.transformed(tr);
            boundary = QRectF(this->cellW + this->xStep,0,this->cellW,this->cellH);
            painter.drawImage(boundary,colImg);
            break;
        }
    case Row:
        {
            QRectF boundary(0,0,this->cellW,this->cellH);
            painter.drawImage(boundary,oCell);
            QTransform tr;
            tr.scale(1,-1);
            QImage rowImg = oCell.transformed(tr);
            boundary = QRectF(0,this->cellH + this->yStep,this->cellW,this->cellH);
            painter.drawImage(boundary,rowImg);
            break;
        }
    case RowColunm:
        {
            QRectF boundary(0,0,this->cellW,this->cellH);
            painter.drawImage(boundary,oCell);
            QTransform tr;
            tr.scale(-1,1);
            QImage colImg = oCell.transformed(tr);
            boundary = QRectF(this->cellW + this->xStep,0,this->cellW,this->cellH);
            painter.drawImage(boundary,colImg);
            tr = QTransform();
            tr.scale(1,-1);
            QImage rowImg = oCell.transformed(tr);
            boundary = QRectF(0,this->cellH + this->yStep,this->cellW,this->cellH);
            painter.drawImage(boundary,rowImg);
            tr = QTransform();
            tr.scale(-1,-1);
            QImage rowColImg = oCell.transformed(tr);
            boundary = QRectF(this->cellW + this->xStep,this->cellH + this->yStep,this->cellW,this->cellH);
            painter.drawImage(boundary,rowColImg);
            break;
        }
    }
    if(saveFeil)
    {
        saveRes = cellImg.save("cell.jpg","jpg",100);
        printf("save png result(cell) is %d\n",saveRes);
    }
    painter.end();
//    cellImg.




    QByteArray ba;

    //    res.save("imageSava1926.jpg","jpg",60);

    QImage result((x+ width)*10,(y+height)*10,QImage::Format_ARGB32);
    QPainter rPainter (&result);
    result.fill(Qt::transparent);
    //    rPainter.setRenderHint(QPainter::Antialiasing, true);
    rPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform); //抗锯齿和使用平滑转换算法
    rPainter.setPen(QColor(0,0,0,0));//透明边框

    QBrush brush(cellImg);
    //    QStringList
    if(CTMList.size() == 6){
        double ctms[6]={0};
        bool isok = false;
        for(int i=0;i < CTMList.length() && i<6; ++i)
        {
            ctms[i] = CTMList[i].toDouble(&isok);
            if(!isok)
            {
                printf("CTM number can not convert to double , %s ",CTMList[i].toUtf8().data());
                ctms[i] = 0;
            }
            printf("double CTM[%d]: %2f\n",i,ctms[i]);
        }
//        ctms[4] = this->mm2px(ctms[4]);
//        ctms[5] = this->mm2px(ctms[5]);
        QMatrix mx(ctms[0] ,ctms[1], ctms[2], ctms[3], ctms[4]*10 ,ctms[5]*10);
//        mx.scale(10,10);
        brush.setMatrix(mx);
//        brush.matrix().
//        QMatrix mx = brush.matrix();
//        printf("QMatrix: %2f %2f ",mx.dx(),mx.dy());
    }

    rPainter.setBrush(brush);//底纹
    rPainter.drawRect(0,0,(x+width)*10,(y+height)*10);
    rPainter.end();

    QImage finalImg;
    QTransform trs;
    trs.scale(0.1,0.1);
    result = result.transformed(trs);
    if(x>0 || y>0)
    {
        printf("painter create fail ");
        finalImg = QImage(width,height,QImage::Format_ARGB32);
        QPainter finalP(&finalImg);
//        bool res = rPainter.begin(&finalImg);
//        if(!res)
//        {
//            printf("painter create fail ");
//            return "";
//        }
        finalP.drawImage(QPointF(0,0),result,QRectF(x,y,width,height));
        printf("x: %lld y: %lld w: %lld  h : %lld \n",x,y,width,height);
        finalP.end();

    }else{
        finalImg = result;
//        finalImg = finalImg.transformed(trs);
    }


    QBuffer buf(&ba);
    buf.open(QIODevice::ReadWrite);
    QImageWriter finalImgWriter(&buf,"png");
    finalImgWriter.write(finalImg);
//    finalImg.save(&buf, "gif",100);
    if(saveFeil)
    {
//        saveRes = finalImg.save("cellresult.jpg","jpg",100);
//        printf("save jpg result(cellresult) is %d\n",saveRes);
        QImageWriter writer("/home/tsit/cellresult.png","png");
        saveRes = writer.write(finalImg);
        if(!saveRes){
            qDebug()<<"save fail "<<writer.errorString();
        }
//        saveRes = finalImg.save("/home/tsit/cellresult.gif","gif",100);
        printf("save gif result(cellresult) is %d\n",saveRes);
//        finalImg.
        //    printf("final image width %d, height %d",finalImg.width(),finalImg.height());
//        qDebug()<<"image read format is ";
    }

//    QImage readImg(ba);
//    qDebug()<<"image read format is "<<readImg.format();
//    QImageReader reader(&buf);
//    reader.setDecideFormatFromContent(true);
//    qDebug()<<"image read format is "<<reader.format();
//    QImage read = reader.read();
//    qDebug()<<"imageReader read  format is "<<read.format();
//    qDebug()<<"imageReader read  error "<<reader.error();
////    finalImg.
//    QByteArray byBk((char*)finalImg.bits());
//    buf.close();
//    QByteArray ba2 = ba.toBase64();


    //    printf("in getBase64 %s",ba2.data());

    return  ba;
    //    return QString();
}

QString TSShading::getBase64()
{
    QByteArray ba2 = getBase64Byte();
    QString b64str = QString::fromUtf8(ba2);
    return b64str;
}

void TSShading::init()
{
    jclass jcs = env->GetObjectClass(jobj);
    if(jcs == NULL){
        printf("create jclass fail \n");
        return ;
    }
    const QMetaObject *theMetaObject = metaObject();
//    QVariantMap properties;
    int count = theMetaObject->propertyCount();
//    printf("[\n");
//    for (int i = 0; i < count; ++i)
//    {
//        QMetaProperty metaproperty = theMetaObject->property(i);
//        const char *name = metaproperty.name();
//        printf("property: %s,\n",name);
//    }
//    printf("]\n");
    char check[11] = "objectName";
    std::string checkStr = "objectName";
    bool needContinue = false;
    for (int i = 0; i < count; ++i)
    {
        needContinue = false;
        QMetaProperty metaproperty = theMetaObject->property(i);
        const char *name = metaproperty.name();
        std::string namestr= name;
        if(checkStr.compare(namestr) == 0){
            printf("property name is %s continue\n",namestr.c_str());
            continue;
        }
        QVariant value = property(name);
        int m = 0;
        //        printf("first check is %c name : %c \n",check[m],name[m]);
        while(check[m]!='\0' && name[m] != '\0' ){
            if(m>100)
            {
                printf("while error");
                needContinue = false;
                break;
            }
            if(check[m] != name[m])
            {
//                printf("check is %c name : %c \n",check[m],name[m]);
                needContinue = false;
                break;
            }else{
                needContinue = true;
            }
            ++m;
        }
        if(QString("objectName") == QString(name) || needContinue)
        {
            //容错，QT该版本存在一个objectName 的默认属性，需要剔除
            continue;
        }
        //        printf("read name %s \n",name);
        QVariant readValue = getJAVAValue(value,QString(name));
        if(readValue.isValid()){
            //            properties[name] = readValue;
            setProperty(name, readValue);
            //            printf("set %s value %s \n",name,readValue.toString().toUtf8().data());
        }else{
            //未读取成功使用默认值
            //properties[name] = value;
        }


    }

    //    QStringList names = properties.keys();
    //    foreach (auto name, names)
    //    {
    //        QVariant value = properties.value(name);
    //        setProperty(name.toStdString().c_str(), value);

    //    }
    //    printf("this width is %lld after set \n",this->width);

}

float TSShading::mm2px(float mm)
{
    int res = (int)(10000*mm*96/25.4f );
    return  res / 10000.0f;
}

int TSShading::getWidth() const
{
    return width;
}

void TSShading::setWidth(int value)
{
    width = value;
}

QVariant TSShading::getJAVAValue(QVariant value,QString name)
{
    QString type="";
    QVariant readValue;
    //    char* cName = name.toUtf8().data();
    jfieldID id;
    //    printf("getJAVAValue name %s type %d \n",name.toUtf8().data(),value.type());
    switch (value.type()) {
    case QVariant::LongLong:
        {
            id = env->GetFieldID(clazz,name.toUtf8().data(),"I");
            qlonglong data = env->GetIntField(jobj,id);
            //        readValue = QVariant((data));
            readValue.setValue(data);
            //            printf("getJAVAValue name %s value %lld \n",name.toUtf8().data(),data);
            //            printf("getJAVAValue name %s QVariant value %s \n",name.toUtf8().data(),readValue.toString().toUtf8().data());
            break;
        }
    case QVariant::Double:
        {
            id = env->GetFieldID(clazz,name.toUtf8().data(),"D");
//             env->Getf
            double data = env->GetDoubleField(jobj,id);
            readValue = QVariant((data));
            //            printf("getJAVAValue name %s value %f \n",name.toUtf8().data(),data);
            break;
        }
    case QVariant::Bool:
        {
            id = env->GetFieldID(clazz,name.toUtf8().data(),"Z");
            bool data = env->GetBooleanField(jobj,id);
            readValue = QVariant((data));
            //            printf("getJAVAValue name %s value %d \n",name.toUtf8().data(),data);
            break;
            break;
        }
    case QVariant::String:
        {
            id = env->GetFieldID(clazz,name.toUtf8().data(),"Ljava/lang/String;");
            //        jstring pathStr = (jstring) env->GetObjectField(info, imagePath);
            jstring data =(jstring) env->GetObjectField(jobj,id);
            QString str = getQString(env,data);
            readValue = QVariant((str));
            //            printf("getJAVAValue name %s value %s \n",name.toUtf8().data(),str.toUtf8().data());
            break;
        }
    }
    return  readValue;
}

qlonglong TSShading::getY() const
{
    return y;
}

void TSShading::setY(const qlonglong&value)
{
    y = value;
}

qlonglong TSShading::getX() const
{
    return x;
}

void TSShading::setX(const qlonglong&value)
{
    x = value;
}

QString TSShading::getCTM() const
{
    return CTM;
}

void TSShading::setCTM(const QString &value)
{
    CTM = value;
}

QString TSShading::getImage() const
{
    return image;
}

void TSShading::setImage(const QString &value)
{
    image = value;
}

bool TSShading::getRelative() const
{
    return relative;
}

void TSShading::setRelative(bool value)
{
    relative = value;
}

qlonglong TSShading::getReflect() const
{
    return reflect;
}

void TSShading::setReflect(const qlonglong &value)
{
    reflect = value;
}

double TSShading::getYStep() const
{
    return yStep;
}

void TSShading::setYStep(const double &value)
{
    yStep = value;
}

double TSShading::getXStep() const
{
    return xStep;
}

void TSShading::setXStep(const double &value)
{
    xStep = value;
}

double TSShading::getCellH() const
{
    return cellH;
}

void TSShading::setCellH(const double &value)
{
    cellH = value;
}

double TSShading::getCellW() const
{
    return cellW;
}

void TSShading::setCellW(const double &value)
{
    cellW = value;
}



qlonglong TSShading::getHeight() const
{
    return height;
}

void TSShading::setHeight(const qlonglong &value)
{
    height = value;
}

int main(int argc, char *argv[])
{
    TSShading ts;
    qDebug()<<"sdsdf "<<varName(ts.width);
    ts.setCellH(60);
    ts.setCellW(60);
    ts.setXStep(5);
    ts.setYStep(10);
    ts.setImage("ssdfasd");
    ts.setWidth(100);
    ts.setHeight(100);
    ts.getImage();
}
