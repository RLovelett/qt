/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QFile>

#include "sunspider.h"

Sunspider::Sunspider()
{
    m_testfiles = (QStringList()
                   << "3d_cube.js"
                   << "3d_morph.js"
                   << "3d_raytrace.js"
                   << "access_binary_trees.js"
                   << "access_fannkuch.js"
                   << "access_nbody.js"
                   << "access_nsieve.js"
                   << "bitops_3bit_bits_in_byte.js"
                   << "bitops_bits_in_byte.js"
                   << "bitops_bitwise_and.js"
                   << "bitops_nsieve_bits.js"
                   << "controlflow_recursive.js"
                   << "crypto_aes.js"
                   << "crypto_md5.js"
                   << "crypto_sha1.js"
                   << "date_format_tofte.js"
                   << "date_format_xparb.js"
                   << "math_cordic.js"
                   << "math_partial_sums.js"
                   << "math_spectral_norm.js"
                   << "regexp_dna.js"
                   << "string_base64.js"
                   << "string_fasta.js"
                   << "string_tagcloud.js"
                   << "string_unpack_code.js"
                   << "string_validate_input.js"
                   );
}

Sunspider::~Sunspider()
{
    m_testfiles.clear();
}

QStringList Sunspider::testFiles()
{
    return m_testfiles;
}
