/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#include "qgl_p.h"

QT_BEGIN_NAMESPACE

bool qt_resolve_framebufferobject_extensions(QGLContext *ctx)
{
#if defined(QT_OPENGL_ES_2)
    static bool have_resolved = false;
    if (have_resolved)
        return true;
    have_resolved = true;
#else
    if (glIsRenderbuffer != 0)
        return true;
#endif

    if (ctx == 0) {
        qWarning("QGLFramebufferObject: Unable to resolve framebuffer object extensions -"
                 " make sure there is a current context when creating the framebuffer object.");
        return false;
    }


    glBlitFramebufferEXT = (_glBlitFramebufferEXT) ctx->getProcAddress(QLatin1String("glBlitFramebufferEXT"));
    glRenderbufferStorageMultisampleEXT =
        (_glRenderbufferStorageMultisampleEXT) ctx->getProcAddress(QLatin1String("glRenderbufferStorageMultisampleEXT"));


#if defined(QT_OPENGL_ES_1) || defined(QT_OPENGL_ES_1_CL)
    //FBO's are supported in GLES1 via an OES extension:
    glIsRenderbuffer = (_glIsRenderbuffer) ctx->getProcAddress(QLatin1String("glIsRenderbufferOES"));
    glBindRenderbuffer = (_glBindRenderbuffer) ctx->getProcAddress(QLatin1String("glBindRenderbufferOES"));
    glDeleteRenderbuffers = (_glDeleteRenderbuffers) ctx->getProcAddress(QLatin1String("glDeleteRenderbuffersOES"));
    glGenRenderbuffers = (_glGenRenderbuffers) ctx->getProcAddress(QLatin1String("glGenRenderbuffersOES"));
    glRenderbufferStorage = (_glRenderbufferStorage) ctx->getProcAddress(QLatin1String("glRenderbufferStorageOES"));
    glGetRenderbufferParameteriv =
        (_glGetRenderbufferParameteriv) ctx->getProcAddress(QLatin1String("glGetRenderbufferParameterivOES"));
    glIsFramebuffer = (_glIsFramebuffer) ctx->getProcAddress(QLatin1String("glIsFramebufferOES"));
    glBindFramebuffer = (_glBindFramebuffer) ctx->getProcAddress(QLatin1String("glBindFramebufferOES"));
    glDeleteFramebuffers = (_glDeleteFramebuffers) ctx->getProcAddress(QLatin1String("glDeleteFramebuffersOES"));
    glGenFramebuffers = (_glGenFramebuffers) ctx->getProcAddress(QLatin1String("glGenFramebuffersOES"));
    glCheckFramebufferStatus = (_glCheckFramebufferStatus) ctx->getProcAddress(QLatin1String("glCheckFramebufferStatusOES"));
   glFramebufferTexture2D = (_glFramebufferTexture2D) ctx->getProcAddress(QLatin1String("glFramebufferTexture2DOES"));
   glFramebufferRenderbuffer = (_glFramebufferRenderbuffer) ctx->getProcAddress(QLatin1String("glFramebufferRenderbufferOES"));
   glGetFramebufferAttachmentParameteriv =
        (_glGetFramebufferAttachmentParameteriv) ctx->getProcAddress(QLatin1String("glGetFramebufferAttachmentParameterivOES"));
    glGenerateMipmap = (_glGenerateMipmap) ctx->getProcAddress(QLatin1String("glGenerateMipmapOES"));

    return glIsRenderbuffer

#elif !defined(QT_OPENGL_ES_2) 

    //TODO: add support for GL3.x and ARB_framebuffer_object.
    //ARB_framebuffer_object is a backport of the framebuffer object
    //functions of GL 3.x that are core to an extension; in
    //both 3.x and ARB_framebuffer_object, functions names do not
    //have the suffix EXT, infact they have no suffix. Additionally
    //MSAA, blit and packed_depth_stencil are available without
    //checking any additional extensions.
    glIsRenderbuffer = (_glIsRenderbuffer) ctx->getProcAddress(QLatin1String("glIsRenderbufferEXT"));
    glBindRenderbuffer = (_glBindRenderbuffer) ctx->getProcAddress(QLatin1String("glBindRenderbufferEXT"));
    glDeleteRenderbuffers = (_glDeleteRenderbuffers) ctx->getProcAddress(QLatin1String("glDeleteRenderbuffersEXT"));
    glGenRenderbuffers = (_glGenRenderbuffers) ctx->getProcAddress(QLatin1String("glGenRenderbuffersEXT"));
    glRenderbufferStorage = (_glRenderbufferStorage) ctx->getProcAddress(QLatin1String("glRenderbufferStorageEXT"));
    glGetRenderbufferParameteriv =
        (_glGetRenderbufferParameteriv) ctx->getProcAddress(QLatin1String("glGetRenderbufferParameterivEXT"));
    glIsFramebuffer = (_glIsFramebuffer) ctx->getProcAddress(QLatin1String("glIsFramebufferEXT"));
    glBindFramebuffer = (_glBindFramebuffer) ctx->getProcAddress(QLatin1String("glBindFramebufferEXT"));
    glDeleteFramebuffers = (_glDeleteFramebuffers) ctx->getProcAddress(QLatin1String("glDeleteFramebuffersEXT"));
    glGenFramebuffers = (_glGenFramebuffers) ctx->getProcAddress(QLatin1String("glGenFramebuffersEXT"));
    glCheckFramebufferStatus = (_glCheckFramebufferStatus) ctx->getProcAddress(QLatin1String("glCheckFramebufferStatusEXT"));
    glFramebufferTexture2D = (_glFramebufferTexture2D) ctx->getProcAddress(QLatin1String("glFramebufferTexture2DEXT"));
    glFramebufferRenderbuffer = (_glFramebufferRenderbuffer) ctx->getProcAddress(QLatin1String("glFramebufferRenderbufferEXT"));
    glGetFramebufferAttachmentParameteriv =
        (_glGetFramebufferAttachmentParameteriv) ctx->getProcAddress(QLatin1String("glGetFramebufferAttachmentParameterivEXT"));
    glGenerateMipmap = (_glGenerateMipmap) ctx->getProcAddress(QLatin1String("glGenerateMipmapEXT"));

    return glIsRenderbuffer;
#else
    return true;
#endif
}

#if !defined(QT_OPENGL_ES_2)
bool qt_resolve_version_1_3_functions(QGLContext *ctx)
{
    if (glMultiTexCoord4f != 0)
        return true;

    QGLContext cx(QGLFormat::defaultFormat());
    glMultiTexCoord4f = (_glMultiTexCoord4f) ctx->getProcAddress(QLatin1String("glMultiTexCoord4f"));

    glActiveTexture = (_glActiveTexture) ctx->getProcAddress(QLatin1String("glActiveTexture"));
    return glMultiTexCoord4f && glActiveTexture;
}
#endif

#if !defined(QT_OPENGL_ES_2)
bool qt_resolve_stencil_face_extension(QGLContext *ctx)
{
    if (glActiveStencilFaceEXT != 0)
        return true;

    QGLContext cx(QGLFormat::defaultFormat());
    glActiveStencilFaceEXT = (_glActiveStencilFaceEXT) ctx->getProcAddress(QLatin1String("glActiveStencilFaceEXT"));

    return glActiveStencilFaceEXT;
}
#endif


#if !defined(QT_OPENGL_ES_2)
bool qt_resolve_frag_program_extensions(QGLContext *ctx)
{
    if (glProgramStringARB != 0)
        return true;

    // ARB_fragment_program
    glProgramStringARB = (_glProgramStringARB) ctx->getProcAddress(QLatin1String("glProgramStringARB"));
    glBindProgramARB = (_glBindProgramARB) ctx->getProcAddress(QLatin1String("glBindProgramARB"));
    glDeleteProgramsARB = (_glDeleteProgramsARB) ctx->getProcAddress(QLatin1String("glDeleteProgramsARB"));
    glGenProgramsARB = (_glGenProgramsARB) ctx->getProcAddress(QLatin1String("glGenProgramsARB"));
    glProgramLocalParameter4fvARB = (_glProgramLocalParameter4fvARB) ctx->getProcAddress(QLatin1String("glProgramLocalParameter4fvARB"));

    return glProgramStringARB
        && glBindProgramARB
        && glDeleteProgramsARB
        && glGenProgramsARB
        && glProgramLocalParameter4fvARB;
}
#endif


bool qt_resolve_buffer_extensions(QGLContext *ctx)
{
    if (glMapBufferARB && glUnmapBufferARB
#if !defined(QT_OPENGL_ES_2)
        && glBindBuffer && glDeleteBuffers && glGenBuffers && glBufferData
#endif
        )
        return true;

#if !defined(QT_OPENGL_ES_2)
    glBindBuffer = (_glBindBuffer) ctx->getProcAddress(QLatin1String("glBindBufferARB"));
    glDeleteBuffers = (_glDeleteBuffers) ctx->getProcAddress(QLatin1String("glDeleteBuffersARB"));
    glGenBuffers = (_glGenBuffers) ctx->getProcAddress(QLatin1String("glGenBuffersARB"));
    glBufferData = (_glBufferData) ctx->getProcAddress(QLatin1String("glBufferDataARB"));
#endif
    glMapBufferARB = (_glMapBufferARB) ctx->getProcAddress(QLatin1String("glMapBufferARB"));
    glUnmapBufferARB = (_glUnmapBufferARB) ctx->getProcAddress(QLatin1String("glUnmapBufferARB"));

    return glMapBufferARB
        && glUnmapBufferARB
#if !defined(QT_OPENGL_ES_2)
        && glBindBuffer
        && glDeleteBuffers
        && glGenBuffers
        && glBufferData
#endif
        ;
}

bool qt_resolve_glsl_extensions(QGLContext *ctx)
{
#if defined(QT_OPENGL_ES_2)
    // The GLSL shader functions are always present in OpenGL/ES 2.0.
    // The only exceptions are glGetProgramBinaryOES and glProgramBinaryOES.
    if (!QGLContextPrivate::extensionFuncs(ctx).qt_glslResolved) {
        glGetProgramBinaryOES = (_glGetProgramBinaryOES) ctx->getProcAddress(QLatin1String("glGetProgramBinaryOES"));
        glProgramBinaryOES = (_glProgramBinaryOES) ctx->getProcAddress(QLatin1String("glProgramBinaryOES"));
        QGLContextPrivate::extensionFuncs(ctx).qt_glslResolved = true;
    }
    return true;
#else
    if (glCreateShader)
        return true;

    glCreateShader = (_glCreateShader) ctx->getProcAddress(QLatin1String("glCreateShader"));
    if (glCreateShader) {
        glShaderSource = (_glShaderSource) ctx->getProcAddress(QLatin1String("glShaderSource"));
        glShaderBinary = (_glShaderBinary) ctx->getProcAddress(QLatin1String("glShaderBinary"));
        glCompileShader = (_glCompileShader) ctx->getProcAddress(QLatin1String("glCompileShader"));
        glDeleteShader = (_glDeleteShader) ctx->getProcAddress(QLatin1String("glDeleteShader"));
        glIsShader = (_glIsShader) ctx->getProcAddress(QLatin1String("glIsShader"));

        glCreateProgram = (_glCreateProgram) ctx->getProcAddress(QLatin1String("glCreateProgram"));
        glAttachShader = (_glAttachShader) ctx->getProcAddress(QLatin1String("glAttachShader"));
        glDetachShader = (_glDetachShader) ctx->getProcAddress(QLatin1String("glDetachShader"));
        glLinkProgram = (_glLinkProgram) ctx->getProcAddress(QLatin1String("glLinkProgram"));
        glUseProgram = (_glUseProgram) ctx->getProcAddress(QLatin1String("glUseProgram"));
        glDeleteProgram = (_glDeleteProgram) ctx->getProcAddress(QLatin1String("glDeleteProgram"));
        glIsProgram = (_glIsProgram) ctx->getProcAddress(QLatin1String("glIsProgram"));

        glGetShaderInfoLog = (_glGetShaderInfoLog) ctx->getProcAddress(QLatin1String("glGetShaderInfoLog"));
        glGetShaderiv = (_glGetShaderiv) ctx->getProcAddress(QLatin1String("glGetShaderiv"));
        glGetShaderSource = (_glGetShaderSource) ctx->getProcAddress(QLatin1String("glGetShaderSource"));
        glGetProgramiv = (_glGetProgramiv) ctx->getProcAddress(QLatin1String("glGetProgramiv"));
        glGetProgramInfoLog = (_glGetProgramInfoLog) ctx->getProcAddress(QLatin1String("glGetProgramInfoLog"));

        glGetUniformLocation = (_glGetUniformLocation) ctx->getProcAddress(QLatin1String("glGetUniformLocation"));
        glUniform4fv = (_glUniform4fv) ctx->getProcAddress(QLatin1String("glUniform4fv"));
        glUniform3fv = (_glUniform3fv) ctx->getProcAddress(QLatin1String("glUniform3fv"));
        glUniform2fv = (_glUniform2fv) ctx->getProcAddress(QLatin1String("glUniform2fv"));
        glUniform1fv = (_glUniform1fv) ctx->getProcAddress(QLatin1String("glUniform1fv"));
        glUniform1i = (_glUniform1i) ctx->getProcAddress(QLatin1String("glUniform1i"));
        glUniform1iv = (_glUniform1iv) ctx->getProcAddress(QLatin1String("glUniform1iv"));
        glUniformMatrix2fv = (_glUniformMatrix2fv) ctx->getProcAddress(QLatin1String("glUniformMatrix2fv"));
        glUniformMatrix3fv = (_glUniformMatrix3fv) ctx->getProcAddress(QLatin1String("glUniformMatrix3fv"));
        glUniformMatrix4fv = (_glUniformMatrix4fv) ctx->getProcAddress(QLatin1String("glUniformMatrix4fv"));
        glUniformMatrix2x3fv = (_glUniformMatrix2x3fv) ctx->getProcAddress(QLatin1String("glUniformMatrix2x3fv"));
        glUniformMatrix2x4fv = (_glUniformMatrix2x4fv) ctx->getProcAddress(QLatin1String("glUniformMatrix2x4fv"));
        glUniformMatrix3x2fv = (_glUniformMatrix3x2fv) ctx->getProcAddress(QLatin1String("glUniformMatrix3x2fv"));
        glUniformMatrix3x4fv = (_glUniformMatrix3x4fv) ctx->getProcAddress(QLatin1String("glUniformMatrix3x4fv"));
        glUniformMatrix4x2fv = (_glUniformMatrix4x2fv) ctx->getProcAddress(QLatin1String("glUniformMatrix4x2fv"));
        glUniformMatrix4x3fv = (_glUniformMatrix4x3fv) ctx->getProcAddress(QLatin1String("glUniformMatrix4x3fv"));

        glBindAttribLocation = (_glBindAttribLocation) ctx->getProcAddress(QLatin1String("glBindAttribLocation"));
        glGetAttribLocation = (_glGetAttribLocation) ctx->getProcAddress(QLatin1String("glGetAttribLocation"));
        glVertexAttrib1fv = (_glVertexAttrib1fv) ctx->getProcAddress(QLatin1String("glVertexAttrib1fv"));
        glVertexAttrib2fv = (_glVertexAttrib2fv) ctx->getProcAddress(QLatin1String("glVertexAttrib2fv"));
        glVertexAttrib3fv = (_glVertexAttrib3fv) ctx->getProcAddress(QLatin1String("glVertexAttrib3fv"));
        glVertexAttrib4fv = (_glVertexAttrib4fv) ctx->getProcAddress(QLatin1String("glVertexAttrib4fv"));
        glVertexAttribPointer = (_glVertexAttribPointer) ctx->getProcAddress(QLatin1String("glVertexAttribPointer"));
        glDisableVertexAttribArray = (_glDisableVertexAttribArray) ctx->getProcAddress(QLatin1String("glDisableVertexAttribArray"));
        glEnableVertexAttribArray = (_glEnableVertexAttribArray) ctx->getProcAddress(QLatin1String("glEnableVertexAttribArray"));

    } else {
        // We may not have the standard shader functions, but we might
        // have the older ARB functions instead.
        glCreateShader = (_glCreateShader) ctx->getProcAddress(QLatin1String("glCreateShaderObjectARB"));
        glShaderSource = (_glShaderSource) ctx->getProcAddress(QLatin1String("glShaderSourceARB"));
        glShaderBinary = (_glShaderBinary) ctx->getProcAddress(QLatin1String("glShaderBinaryARB"));
        glCompileShader = (_glCompileShader) ctx->getProcAddress(QLatin1String("glCompileShaderARB"));
        glDeleteShader = (_glDeleteShader) ctx->getProcAddress(QLatin1String("glDeleteObjectARB"));
        glIsShader = 0;

        glCreateProgram = (_glCreateProgram) ctx->getProcAddress(QLatin1String("glCreateProgramObjectARB"));
        glAttachShader = (_glAttachShader) ctx->getProcAddress(QLatin1String("glAttachObjectARB"));
        glDetachShader = (_glDetachShader) ctx->getProcAddress(QLatin1String("glDetachObjectARB"));
        glLinkProgram = (_glLinkProgram) ctx->getProcAddress(QLatin1String("glLinkProgramARB"));
        glUseProgram = (_glUseProgram) ctx->getProcAddress(QLatin1String("glUseProgramObjectARB"));
        glDeleteProgram = (_glDeleteProgram) ctx->getProcAddress(QLatin1String("glDeleteObjectARB"));
        glIsProgram = 0;

        glGetShaderInfoLog = (_glGetShaderInfoLog) ctx->getProcAddress(QLatin1String("glGetInfoLogARB"));
        glGetShaderiv = (_glGetShaderiv) ctx->getProcAddress(QLatin1String("glGetObjectParameterivARB"));
        glGetShaderSource = (_glGetShaderSource) ctx->getProcAddress(QLatin1String("glGetShaderSourceARB"));
        glGetProgramiv = (_glGetProgramiv) ctx->getProcAddress(QLatin1String("glGetObjectParameterivARB"));
        glGetProgramInfoLog = (_glGetProgramInfoLog) ctx->getProcAddress(QLatin1String("glGetInfoLogARB"));

        glGetUniformLocation = (_glGetUniformLocation) ctx->getProcAddress(QLatin1String("glGetUniformLocationARB"));
        glUniform4fv = (_glUniform4fv) ctx->getProcAddress(QLatin1String("glUniform4fvARB"));
        glUniform3fv = (_glUniform3fv) ctx->getProcAddress(QLatin1String("glUniform3fvARB"));
        glUniform2fv = (_glUniform2fv) ctx->getProcAddress(QLatin1String("glUniform2fvARB"));
        glUniform1fv = (_glUniform1fv) ctx->getProcAddress(QLatin1String("glUniform1fvARB"));
        glUniform1i = (_glUniform1i) ctx->getProcAddress(QLatin1String("glUniform1iARB"));
        glUniform1iv = (_glUniform1iv) ctx->getProcAddress(QLatin1String("glUniform1ivARB"));
        glUniformMatrix2fv = (_glUniformMatrix2fv) ctx->getProcAddress(QLatin1String("glUniformMatrix2fvARB"));
        glUniformMatrix3fv = (_glUniformMatrix3fv) ctx->getProcAddress(QLatin1String("glUniformMatrix3fvARB"));
        glUniformMatrix4fv = (_glUniformMatrix4fv) ctx->getProcAddress(QLatin1String("glUniformMatrix4fvARB"));
        glUniformMatrix2x3fv = (_glUniformMatrix2x3fv) ctx->getProcAddress(QLatin1String("glUniformMatrix2x3fvARB"));
        glUniformMatrix2x4fv = (_glUniformMatrix2x4fv) ctx->getProcAddress(QLatin1String("glUniformMatrix2x4fvARB"));
        glUniformMatrix3x2fv = (_glUniformMatrix3x2fv) ctx->getProcAddress(QLatin1String("glUniformMatrix3x2fvARB"));
        glUniformMatrix3x4fv = (_glUniformMatrix3x4fv) ctx->getProcAddress(QLatin1String("glUniformMatrix3x4fvARB"));
        glUniformMatrix4x2fv = (_glUniformMatrix4x2fv) ctx->getProcAddress(QLatin1String("glUniformMatrix4x2fvARB"));
        glUniformMatrix4x3fv = (_glUniformMatrix4x3fv) ctx->getProcAddress(QLatin1String("glUniformMatrix4x3fvARB"));

        glBindAttribLocation = (_glBindAttribLocation) ctx->getProcAddress(QLatin1String("glBindAttribLocationARB"));
        glGetAttribLocation = (_glGetAttribLocation) ctx->getProcAddress(QLatin1String("glGetAttribLocationARB"));
        glVertexAttrib1fv = (_glVertexAttrib1fv) ctx->getProcAddress(QLatin1String("glVertexAttrib1fvARB"));
        glVertexAttrib2fv = (_glVertexAttrib2fv) ctx->getProcAddress(QLatin1String("glVertexAttrib2fvARB"));
        glVertexAttrib3fv = (_glVertexAttrib3fv) ctx->getProcAddress(QLatin1String("glVertexAttrib3fvARB"));
        glVertexAttrib4fv = (_glVertexAttrib4fv) ctx->getProcAddress(QLatin1String("glVertexAttrib4fvARB"));
        glVertexAttribPointer = (_glVertexAttribPointer) ctx->getProcAddress(QLatin1String("glVertexAttribPointerARB"));
        glDisableVertexAttribArray = (_glDisableVertexAttribArray) ctx->getProcAddress(QLatin1String("glDisableVertexAttribArrayARB"));
        glEnableVertexAttribArray = (_glEnableVertexAttribArray) ctx->getProcAddress(QLatin1String("glEnableVertexAttribArrayARB"));
    }

    // Note: glShaderBinary(), glIsShader(), glIsProgram(), and
    // glUniformMatrixNxMfv() are optional, but all other functions
    // are required.

    return glCreateShader &&
        glShaderSource &&
        glCompileShader &&
        glDeleteProgram &&
        glCreateProgram &&
        glAttachShader &&
        glDetachShader &&
        glLinkProgram &&
        glUseProgram &&
        glDeleteProgram &&
        glGetShaderInfoLog &&
        glGetShaderiv &&
        glGetShaderSource &&
        glGetProgramiv &&
        glGetProgramInfoLog &&
        glGetUniformLocation &&
        glUniform1fv &&
        glUniform2fv &&
        glUniform3fv &&
        glUniform4fv &&
        glUniform1i &&
        glUniform1iv &&
        glUniformMatrix2fv &&
        glUniformMatrix3fv &&
        glUniformMatrix4fv &&
        glBindAttribLocation &&
        glGetAttribLocation &&
        glVertexAttrib1fv &&
        glVertexAttrib2fv &&
        glVertexAttrib3fv &&
        glVertexAttrib4fv &&
        glVertexAttribPointer &&
        glDisableVertexAttribArray &&
        glEnableVertexAttribArray;
#endif
}

#if !defined(QT_OPENGL_ES_2)
bool qt_resolve_version_2_0_functions(QGLContext *ctx)
{
    bool gl2supported = true;
    if (!qt_resolve_glsl_extensions(ctx))
        gl2supported = false;

    if (!qt_resolve_version_1_3_functions(ctx))
        gl2supported = false;

    if (!qt_resolve_framebufferobject_extensions(ctx))
        gl2supported = false;

    if (glStencilOpSeparate)
        return gl2supported;

    glStencilOpSeparate = (_glStencilOpSeparate) ctx->getProcAddress(QLatin1String("glStencilOpSeparate"));
    if (!glStencilOpSeparate)
        gl2supported = false;

    return gl2supported;
}
#endif


QT_END_NAMESPACE
