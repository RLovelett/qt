/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qppmhandler_p.h"

#ifndef QT_NO_IMAGEFORMAT_PPM

#include <qimage.h>
#include <qvariant.h>
#include <qvector.h>
#include <ctype.h>

QT_BEGIN_NAMESPACE

/*****************************************************************************
  Mapping from type to format

  Warning ! Format is the format string returned by 
      QByteArray QImageIOHandler::format () const
  Not the enum QImage::Format !!!

  In this implementation, with option set to ImageOption::SubType
      QVariant QPpmHandler::option (ImageOption option) const
  returns 
      QByteArray QImageIOHandler::format ()

  Vice-versa, with option set to ImageOption::SubType
      void QPpmHandler::setOption ( ImageOption option, const QVariant & value )
  calls 
      void QImageIOHandler::setFormat ( const QByteArray & format) 

*****************************************************************************/

static QByteArray type_to_format(char type)
{
    switch(type) {
    case '1':
    case '4':
        return QByteArray("pbm");
    case '2':
    case '5':
        return QByteArray("pgm");
    case '3':
    case '6':
        return QByteArray("ppm");
    }
    return QByteArray();
}

static char format_to_type(const QByteArray &subType)
{
    if (subType.startsWith("pbm"))
        return '4';
    else if (subType.startsWith("pgm"))
        return '5';
    else if (subType.startsWith("ppm"))
        return '6';
    return ' ';
}

/*****************************************************************************
  Test if we can read this file
*****************************************************************************/

static bool can_read(QIODevice *device, char & type)
{
    if (!device) {
        qWarning("QPpmHandler::canRead() called with no device");
        return false;
    }

    char head[2];
    if (device->peek(head, sizeof(head)) != sizeof(head))
        return false;

    if (head[0] != 'P')
        return false;

    type = head[1];

    return ('1' <= type && type <= '6'); 
}
 
/*****************************************************************************
  PBM/PGM/PPM (ASCII and RAW) image read/write functions
 *****************************************************************************/

static int pnm_read_int(QIODevice *d)
{
    char c;
    int          val = -1;
    bool  digit;
    const int buflen = 100;
    char  buf[buflen];
    for (;;) {
        if (!d->getChar(&c))                // end of file
            break;
        digit = isdigit((uchar) c);
        if (val != -1) {
            if (digit) {
                val = 10*val + c - '0';
                continue;
            } else {
                if (c == '#')                        // comment
                    d->readLine(buf, buflen);
                break;
            }
        }
        if (digit)                                // first digit
            val = c - '0';
        else if (isspace((uchar) c))
            continue;
        else if (c == '#')
            (void)d->readLine(buf, buflen);
        else
            break;
    }
    return val;
}

static bool pnm_read_header(QIODevice *device, char& type, int& w, int& h, int& mcc, QImage::Format& image_format)
{
    char buf[3];
    if (device->read(buf, 3) != 3)                        // read P[1-6]<white-space>
        return false;

    if (!(buf[0] == 'P' && isdigit((uchar) buf[1]) && isspace((uchar) buf[2])))
        return false;

    type = buf[1];
    if (type < '1' || type > '6')
        return false;

    w = pnm_read_int(device);                        // get image width
    h = pnm_read_int(device);                        // get image height

    if (type == '1' || type == '4')
        mcc = 1;                                  // ignore max color component
    else
        mcc = pnm_read_int(device);               // get max color component

    if (w <= 0 || w > 32767 || h <= 0 || h > 32767 || mcc <= 0)
        return false;                                        // weird P.M image

    switch (type) {
        case '1':                                // ascii PBM
        case '4':                                // raw PBM
            image_format = QImage::Format_Mono;
            break;
        case '2':                                // ascii PGM
        case '5':                                // raw PGM
            image_format = QImage::Format_Indexed8;
            break;
        case '3':                                // ascii PPM
        case '6':                                // raw PPM
            image_format = QImage::Format_RGB32;
            break;
        default:
            return false;
    }

    return true;
}

static bool pnm_read_body(QIODevice *device, char type, int w, int h, int mcc, QImage::Format image_format,  QImage *outImage)
{
    int nbits, y;
    int pbm_bpl;
    bool raw;

    switch (image_format) {
        case QImage::Format_Mono:
            nbits = 1;
            break;
        case QImage::Format_Indexed8: 
            nbits = 8;
            break;
        case QImage::Format_RGB32:
            nbits = 32;
            break;
        default:
            return false;
    }
    raw = type >= '4';

    int maxc = mcc;
    if (maxc > 255)
        maxc = 255;

    pbm_bpl = (nbits*w+7)/8;                        // bytes per scanline in PBM

    if (raw) {                                // read raw data
        if (nbits == 32) {                        // type 6
            pbm_bpl = mcc < 256 ? 3*w : 6*w;
            uchar *buf24 = new uchar[pbm_bpl], *b;
            QRgb  *p;
            QRgb  *end;
            for (y=0; y<h; y++) {
                if (device->read((char *)buf24, pbm_bpl) != pbm_bpl) {
                    delete[] buf24;
                    return false;
                }
                p = (QRgb *)outImage->scanLine(y);
                end = p + w;
                b = buf24;
                while (p < end) {
                    if (mcc < 256) {
                        *p++ = qRgb(b[0],b[1],b[2]);
                        b += 3;
                    } else {
                        *p++ = qRgb(((int(b[0]) * 256 + int(b[1]) + 1) * 256) / (mcc + 1) - 1,
                                    ((int(b[2]) * 256 + int(b[3]) + 1) * 256) / (mcc + 1) - 1,
                                    ((int(b[4]) * 256 + int(b[5]) + 1) * 256) / (mcc + 1) - 1);
                        b += 6;
                    }
                }
            }
            delete[] buf24;
        } else {                                // type 4,5
            for (y=0; y<h; y++) {
                if (device->read((char *)outImage->scanLine(y), pbm_bpl)
                        != pbm_bpl)
                    return false;
            }
        }
    } else {                                        // read ascii data
        register uchar *p;
        int n;
        for (y=0; y<h; y++) {
            p = outImage->scanLine(y);
            n = pbm_bpl;
            if (nbits == 1) {
                int b;
                int bitsLeft = w;
                while (n--) {
                    b = 0;
                    for (int i=0; i<8; i++) {
                        if (i < bitsLeft)
                            b = (b << 1) | (pnm_read_int(device) & 1);
                        else
                            b = (b << 1) | (0 & 1); // pad it our self if we need to
                    }
                    bitsLeft -= 8;
                    *p++ = b;
                }
            } else if (nbits == 8) {
                if (mcc == maxc) {
                    while (n--) {
                        *p++ = pnm_read_int(device);
                    }
                } else {
                    while (n--) {
                        *p++ = pnm_read_int(device) * maxc / mcc;
                    }
                }
            } else {                                // 32 bits
                n /= 4;
                int r, g, b;
                if (mcc == maxc) {
                    while (n--) {
                        r = pnm_read_int(device);
                        g = pnm_read_int(device);
                        b = pnm_read_int(device);
                        *((QRgb*)p) = qRgb(r, g, b);
                        p += 4;
                    }
                } else {
                    while (n--) {
                        r = pnm_read_int(device) * maxc / mcc;
                        g = pnm_read_int(device) * maxc / mcc;
                        b = pnm_read_int(device) * maxc / mcc;
                        *((QRgb*)p) = qRgb(r, g, b);
                        p += 4;
                    }
                }
            }
        }
    }

    if (nbits == 1) {                                // bitmap
        outImage->setColorCount(2);
        outImage->setColor(0, qRgb(255,255,255)); // white
        outImage->setColor(1, qRgb(0,0,0));        // black
    } else if (nbits == 8) {                        // graymap
        outImage->setColorCount(maxc+1);
        for (int i=0; i<=maxc; i++)
            outImage->setColor(i, qRgb(i*255/maxc,i*255/maxc,i*255/maxc));
    }

    return true;
}

static bool pnm_write_image(QIODevice *out, const QImage &sourceImage, const QByteArray &sourceFormat)
{
    QByteArray str;
    QImage image = sourceImage;
    QByteArray format = sourceFormat;

    format = format.left(3);                        // ignore RAW part
    bool gray = format == "pgm";

    if (format == "pbm") {
        image = image.convertToFormat(QImage::Format_Mono);
    } else if (image.depth() == 1) {
        image = image.convertToFormat(QImage::Format_Indexed8);
    } else {
        switch (image.format()) {
        case QImage::Format_RGB16:
        case QImage::Format_RGB666:
        case QImage::Format_RGB555:
        case QImage::Format_RGB888:
        case QImage::Format_RGB444:
            image = image.convertToFormat(QImage::Format_RGB32);
            break;
        case QImage::Format_ARGB8565_Premultiplied:
        case QImage::Format_ARGB6666_Premultiplied:
        case QImage::Format_ARGB8555_Premultiplied:
        case QImage::Format_ARGB4444_Premultiplied:
            image = image.convertToFormat(QImage::Format_ARGB32);
            break;
        default:
            break;
        }
    }

    if (image.depth() == 1 && image.colorCount() == 2) {
        if (qGray(image.color(0)) < qGray(image.color(1))) {
            // 0=dark/black, 1=light/white - invert
            image.detach();
            for (int y=0; y<image.height(); y++) {
                uchar *p = image.scanLine(y);
                uchar *end = p + image.bytesPerLine();
                while (p < end)
                    *p++ ^= 0xff;
            }
        }
    }

    uint w = image.width();
    uint h = image.height();

    str = "P\n";
    str += QByteArray::number(w);
    str += ' ';
    str += QByteArray::number(h);
    str += '\n';

    switch (image.depth()) {
        case 1: {
            str.insert(1, '4');
            if (out->write(str, str.length()) != str.length())
                return false;
            w = (w+7)/8;
            for (uint y=0; y<h; y++) {
                uchar* line = image.scanLine(y);
                if (w != (uint)out->write((char*)line, w))
                    return false;
            }
            }
            break;

        case 8: {
            str.insert(1, gray ? '5' : '6');
            str.append("255\n");
            if (out->write(str, str.length()) != str.length())
                return false;
            QVector<QRgb> color = image.colorTable();
            uint bpl = w*(gray ? 1 : 3);
            uchar *buf   = new uchar[bpl];
            for (uint y=0; y<h; y++) {
                uchar *b = image.scanLine(y);
                uchar *p = buf;
                uchar *end = buf+bpl;
                if (gray) {
                    while (p < end) {
                        uchar g = (uchar)qGray(color[*b++]);
                        *p++ = g;
                    }
                } else {
                    while (p < end) {
                        QRgb rgb = color[*b++];
                        *p++ = qRed(rgb);
                        *p++ = qGreen(rgb);
                        *p++ = qBlue(rgb);
                    }
                }
                if (bpl != (uint)out->write((char*)buf, bpl))
                    return false;
            }
            delete [] buf;
            }
            break;

        case 32: {
            str.insert(1, gray ? '5' : '6');
            str.append("255\n");
            if (out->write(str, str.length()) != str.length())
                return false;
            uint bpl = w*(gray ? 1 : 3);
            uchar *buf = new uchar[bpl];
            for (uint y=0; y<h; y++) {
                QRgb  *b = (QRgb*)image.scanLine(y);
                uchar *p = buf;
                uchar *end = buf+bpl;
                if (gray) {
                    while (p < end) {
                        uchar g = (uchar)qGray(*b++);
                        *p++ = g;
                    }
                } else {
                    while (p < end) {
                        QRgb rgb = *b++;
                        *p++ = qRed(rgb);
                        *p++ = qGreen(rgb);
                        *p++ = qBlue(rgb);
                    }
                }
                if (bpl != (uint)out->write((char*)buf, bpl))
                    return false;
            }
            delete [] buf;
            }
            break;

    default:
        return false;
    }

    return true;
}

/*****************************************************************************

 *****************************************************************************/

QPpmHandler::QPpmHandler()
    : state(Ready)
{
}

bool QPpmHandler::readHeader()
{
    if (!canRead()) {
        return false;
    }

    if(state == Ready) {
        type = format_to_type(format());

        if (('1' <= type) && (type <= '6') && !pnm_read_header(device(), type, width, height, maxval, image_format)) {
            state = Error;
            return false;
        }

        state = ReadHeader;
    }

    return true;
}

bool QPpmHandler::canRead() const
{
    if (state == Ready) {
        QByteArray subType;
        if (canRead(device(), &subType))
            setFormat(subType);
	else
            return false;
    }

    return (state == Error)?false:true;
}

bool QPpmHandler::canRead(QIODevice *device, QByteArray *subType)
{
    if (!device) {
        qWarning("QPpmHandler::canRead() called with no device");
        return false;
    }

    char type;
    if(!can_read(device, type))
        return false;

    if(subType)
        *subType = type_to_format(type);

    return true;
}

bool QPpmHandler::read(QImage *image)
{
    if (!readHeader())
        return false;

    if (image->size() != QSize(width, height) || image->format() != image_format) {
        *image = QImage(width, height, image_format);
        if (image->isNull()) {
            state = Error;
            return false;
        }
    }

    if (('1' <= type) && (type <= '6') && !pnm_read_body(device(), type, width, height, maxval, image_format, image)) {
        state = Error;
        return false;
    }

    state = Ready;
    return true;
}

bool QPpmHandler::write(const QImage &image)
{
    return pnm_write_image(device(), image, format());
}

bool QPpmHandler::supportsOption(ImageOption option) const
{
    return option == SubType
        || option == Size
        || option == ImageFormat;
}

QVariant QPpmHandler::option(ImageOption option) const
{
    if (!const_cast<QPpmHandler *>(this)->readHeader())
        return false;

    if (option == SubType)
        return format();
    else if (option == Size)
        return QSize(width, height);
    else if (option == ImageFormat)
        return image_format;

    return QVariant();
}

void QPpmHandler::setOption(ImageOption option, const QVariant &value)
{
    if (option == SubType)
        setFormat(value.toByteArray().toLower());
}

QT_END_NAMESPACE

#endif // QT_NO_IMAGEFORMAT_PPM
