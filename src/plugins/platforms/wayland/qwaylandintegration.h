/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QPLATFORMINTEGRATION_WAYLAND_H
#define QPLATFORMINTEGRATION_WAYLAND_H

#include <QSocketNotifier>
#include <QObject>
#include <QtGui/QPlatformIntegration>
#include <QtGui/QPlatformScreen>

#include <wayland-client.h>

QT_BEGIN_NAMESPACE

class QWaylandInputDevice;

QWaylandInputDevice *
waylandInputDeviceCreate(struct wl_display *display, uint32_t id);

class QWaylandDisplay : public QObject {
    Q_OBJECT;

public:
    QWaylandDisplay(void);
    ~QWaylandDisplay(void);

    QList<QPlatformScreen *> screens() const { return mScreens; }
    struct wl_surface *createSurface();
    struct wl_buffer *createShmBuffer(int fd, int width, int height,
				      uint32_t stride,
				      struct wl_visual *visual);
    struct wl_visual *argbVisual();

public slots:
    void eventDispatcher(void);
    void flushRequests(void);

private:
    struct wl_display *mDisplay;
    struct wl_compositor *mCompositor;
    struct wl_drm *mDrm;
    struct wl_shm *mShm;
    char *mDeviceName;
    int mFd;
    bool mAuthenticated;
    QList<QPlatformScreen *> mScreens;
    QList<QWaylandInputDevice *> mInputDevices;
    QSocketNotifier *mReadNotifier;
    QSocketNotifier *mWriteNotifier;

    static void displayHandleGlobal(struct wl_display *display,
				    uint32_t id,
				    const char *interface,
				    uint32_t version, void *data);

    static void drmHandleDevice(void *data,
				struct wl_drm *drm, const char *device);
    static void drmHandleAuthenticated(void *data, struct wl_drm *drm);

    static void outputHandleGeometry(void *data,
				     struct wl_output *output,
				     int32_t width, int32_t height);

    static int sourceUpdate(uint32_t mask, void *data);

    static const struct wl_drm_listener drmListener;
    static const struct wl_output_listener outputListener;
};

class QWaylandScreen : public QPlatformScreen
{
public:
    QWaylandScreen() {}

    QRect geometry() const { return mGeometry; }
    int depth() const { return mDepth; }
    QImage::Format format() const { return mFormat; }

public:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
    struct wl_output *mOutput;
};


class QWaylandWindow : public QPlatformWindow
{
public:
    QWaylandWindow(QWidget *window, QWaylandDisplay *display);
    ~QWaylandWindow();
    struct wl_surface *surface() { return mSurface; }

    void setVisible(bool visible);
    WId winId() const;

private:
    struct wl_surface *mSurface;
    QWaylandDisplay *mDisplay;
    WId mWindowId;
};

class QWaylandIntegration : public QPlatformIntegration
{
public:
    QWaylandIntegration();

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;

    QList<QPlatformScreen *> screens() const;

    QPlatformFontDatabase *fontDatabase() const;

private:
    QPlatformFontDatabase *mFontDb;
    QWaylandDisplay *mDisplay;
};

QT_END_NAMESPACE

#endif
