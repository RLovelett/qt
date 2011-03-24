/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qccpueditorinterface_p.h"
#include "ccpueditorproxy.h"

CcpuEditorProxy *proxy = 0;

EXPORT_C MEikCcpuEditor* proxyObject(QCcpuEditorInterface *interface)
{
    if (!proxy)
        proxy = new CcpuEditorProxy(interface);
    return proxy;
}

EXPORT_C void deleteProxy()
{
    if (proxy) {
        delete proxy;
        proxy = 0;
    }
}

TBool CcpuEditorProxy::CcpuIsFocused() const
{
    return m_interface->CcpuIsFocused();
}

TBool CcpuEditorProxy::CcpuCanCut() const
{
    return m_interface->CcpuCanCut();
}

void CcpuEditorProxy::CcpuCutL()
{
    m_interface->CcpuCutL();
}

TBool CcpuEditorProxy::CcpuCanCopy() const
{
    return m_interface->CcpuCanCopy();
}

void CcpuEditorProxy::CcpuCopyL()
{
    m_interface->CcpuCopyL();
}

TBool CcpuEditorProxy::CcpuCanPaste() const
{
    return m_interface->CcpuCanPaste();
}

void CcpuEditorProxy::CcpuPasteL()
{
    m_interface->CcpuPasteL();
}

TBool CcpuEditorProxy::CcpuCanUndo() const
{
    return m_interface->CcpuCanUndo();
}

void CcpuEditorProxy::CcpuUndoL()
{
    m_interface->CcpuUndoL();
}
