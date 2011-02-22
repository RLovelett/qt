/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

/*!
    \class QWBMPHandler
    \since 4.7
    \brief The QWBMPHandler class provides support for the WBMP image format.
    \internal
*/




#include "qwbmphandler.h"
#include <QtGui/QImage>
#include <qvariant.h>

QT_BEGIN_NAMESPACE

// These struct represent header of WBMP image file
typedef struct
{
    quint8 bType;              // Type of WBMP image (always equal to 0)
    quint8 bFormat;            // Format of WBMP image
    quint32 iWidth;            // Width of the image already decoded from multibyte integer
    quint32 iHeight;           // Height of the image already decoded from multibyte integer
} WBMPHEADER, *LPWBMPHEADER;
#define WBMPFIXEDHEADER_SIZE 2

// Data renderers and writers which takes care of data alignment endiness and stuff
static bool readMultiByteInt(QIODevice* iodev, quint32* num)
{
    quint32 res = 0;

    unsigned int count = 0;
    do {
        // Do not allow to read longer
        // then we can store in num
        if (++count > sizeof(*num))
            return false;

        quint8 c;
        if (!iodev->getChar((char*)&c))
            return false;

        res = (res << 7) | (c & 0x7F);
        if (!(c & 0x80)) break;

    } while (true);

    *num = res;
    return true;
}

static bool writeMultiByteInt(QIODevice* iodev, quint32 num)
{
    quint64 tmp = 0;
    {
        quint8 c = num & 0x7F;
        num = num >> 7;
        tmp = (tmp << 8) | c;
    }

    while (num)
    {
        quint8 c = num & 0x7F;
        num = num >> 7;
        tmp = (tmp << 8) | (c | 0x80);
    }

    while (tmp)
    {
        quint8 c = tmp & 0xFF;
        if (!iodev->putChar(c))
            return false;
        tmp >>= 8;
    }
    return true;
}

static bool readWBMPHeader(QIODevice* iodev, LPWBMPHEADER hdr)
{
    if (iodev)
    {
        uchar tmp[WBMPFIXEDHEADER_SIZE];
        if (iodev->read((char*)tmp, WBMPFIXEDHEADER_SIZE) == WBMPFIXEDHEADER_SIZE)
        {
            hdr->bType = tmp[0];
            hdr->bFormat = tmp[1];
        }

        if (readMultiByteInt(iodev, &hdr->iWidth) &&
            readMultiByteInt(iodev, &hdr->iHeight))
            return true;
    }
    return false;
}

static bool writeWBMPHeader(QIODevice* iodev, const WBMPHEADER& hdr)
{
    if (iodev)
    {
        uchar tmp[WBMPFIXEDHEADER_SIZE];
        tmp[0] = hdr.bType;
        tmp[1] = hdr.bFormat;
        if (!iodev->write((char*)tmp,  WBMPFIXEDHEADER_SIZE) == WBMPFIXEDHEADER_SIZE)
            return false;

        if (writeMultiByteInt(iodev, hdr.iWidth) &&
            writeMultiByteInt(iodev, hdr.iHeight))
            return true;
    }
    return false;
}

static bool writeWBMPData(QIODevice* iodev, const QImage& image)
{
    if (iodev)
    {
        int h = image.height();
        int bpl = (image.width() + 7)/8;

        int l = 0;
        do {
            // TBD: replace scanLine with constScanLine() in Qt 4.7
            if (iodev->write((char*)image.scanLine(l), bpl) != bpl)
                break;
        } while (++l < h);

        if (l == h)
            return true;
    }
    return false;
}

static bool readWBMPData(QIODevice* iodev, QImage& image)
{
    if (iodev)
    {
        int h = image.height();
        int bpl = (image.width() + 7)/8;

        int l = 0;
        do {
            if (iodev->read((char*)image.scanLine(l), bpl) != bpl)
                break;
        } while (++l < h);

        if (l == h)
            return true;
    }
    return false;
}

class WBMPReader
{
public:
    WBMPReader(QIODevice* iodevice);

    QImage readImage();
    bool writeImage(QImage image);

    static bool canRead(QIODevice* iodevice);

private:
    QIODevice* iodev;
    WBMPHEADER hdr;
};

// WBMP common reader and writer implementation
WBMPReader::WBMPReader(QIODevice* iodevice) : iodev(iodevice)
{
    memset(&hdr, 0, sizeof(hdr));
}

QImage WBMPReader::readImage()
{
    if (!readWBMPHeader(iodev, &hdr))
        return QImage();

    QImage image(hdr.iWidth, hdr.iHeight, QImage::Format_Mono);
    if (!readWBMPData(iodev, image))
        return QImage();

    return image;
}

bool WBMPReader::writeImage(QImage image)
{
    // We need to invert pixels of whole image before saving it
    // WBMP has a different convention about black and white
    image.invertPixels();

    if (image.format() != QImage::Format_Mono)
        image = image.convertToFormat(QImage::Format_Mono);

    hdr.bType = 0;
    hdr.bFormat = 0;
    hdr.iWidth = image.width();
    hdr.iHeight = image.height();

    if (!writeWBMPHeader(iodev, hdr))
        return false;

    if (!writeWBMPData(iodev, image))
        return false;

    return true;
}

bool WBMPReader::canRead(QIODevice *device)
{
    if (device)
    {
        if (device->isSequential())
        {
            qWarning("QWBMPHandler::canRead() called with not sequential device");
            return false;
        }

        // Save previous position
        qint64 oldPos = device->pos();

        WBMPHEADER hdr;
        if (readWBMPHeader(device, &hdr))
        {
            device->seek(oldPos);

            if (hdr.bType == 0 &&
                hdr.bFormat == 0)
                return true;
            else
                return false;
        }
        device->seek(oldPos);
    }
    return false;
}

/*!
    Constructs an instance of QWBMPHandler initialized to use \a device.
*/
QWBMPHandler::QWBMPHandler(QIODevice* device) :
    m_reader(new WBMPReader(device))
{
}

/*!
    Destructor for QWBMPHandler.
*/
QWBMPHandler::~QWBMPHandler()
{
    delete m_reader;
}

/*!
 * Verifies if some values (magic bytes) are set as expected in the header of the file.
 * If the magic bytes were found, it is assumed that the QWBMPHandler can read the file.
 *
 */
bool QWBMPHandler::canRead() const
{
    bool bCanRead = false;
    QIODevice* device = QImageIOHandler::device();
    if (device)
    {
        bCanRead = QWBMPHandler::canRead(device);
        if (bCanRead)
            setFormat("wbmp");

    } else {
        qWarning("QWBMPHandler::canRead() called with no device");
    }

    return bCanRead;
}

/*! \reimp

*/
bool QWBMPHandler::read(QImage* image)
{
    bool bSuccess = false;
    QImage img = m_reader->readImage();

    if (!img.isNull())
    {
        bSuccess = true;
        *image = img;
    }

    return bSuccess;
}

/*! \reimp

*/
bool QWBMPHandler::write(const QImage& image)
{
    if (image.isNull())
        return false;

    return m_reader->writeImage(image);
}

/*!
    Only Size option is supported
*/
QVariant QWBMPHandler::option(ImageOption option) const
{
    if (option == QImageIOHandler::Size)
    {
        QIODevice* device = QImageIOHandler::device();
        if (device->isSequential())
            return QVariant();

        // Save old position
        qint64 oldPos = device->pos();

        WBMPHEADER hdr;
        if (readWBMPHeader(device, &hdr))
        {
            device->seek(oldPos);
            return QSize(hdr.iWidth, hdr.iHeight);
        }

        device->seek(oldPos);

    } else if (option == QImageIOHandler::ImageFormat) {
        return QVariant(QImage::Format_Mono);
    }

    return QVariant();
}

bool QWBMPHandler::supportsOption(ImageOption option) const
{
    return (option == QImageIOHandler::Size) ||
           (option == QImageIOHandler::ImageFormat);
}

bool QWBMPHandler::canRead(QIODevice* device)
{
    return WBMPReader::canRead(device);
}

QT_END_NAMESPACE
