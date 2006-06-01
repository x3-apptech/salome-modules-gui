// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxSplash.h
// Author:    Vadim SANDLER

#ifndef QTXSPLASH_H
#define QTXSPLASH_H

#include "Qtx.h"

#include <qpixmap.h>
#include <qwidget.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxSplash : public QWidget
{
    Q_OBJECT

private:
    QtxSplash( const QPixmap& = QPixmap() );

public:
    enum { Horizontal, Vertical };

    virtual ~QtxSplash();

    static QtxSplash* splash( const QPixmap& = QPixmap() );
    static void       setStatus( const QString&, const int = 0 );
    static void       error( const QString&, const QString& = QString::null, const int = -1 );

    void              setPixmap( const QPixmap& );
    QPixmap           pixmap() const;

    void              setHideOnClick( const bool );
    bool              hideOnClick() const;

    void              setTotalSteps( const int );
    int               totalSteps() const;
    void              setProgress( const int );
    int               progress() const;
    void              setProgress( const int, const int );
    void              setProgressColors( const QColor&, 
					 const QColor& = QColor(),
					 const int     = Vertical );
    int               progressColors( QColor&, QColor& );

    void              setTextAlignment( const int );
    int               textAlignment() const;

    void              setTextColor( const QColor& );
    QColor            textColor() const;
    void              setTextColors( const QColor&, const QColor& = QColor() );
    void              textColors( QColor&, QColor& ) const;
    
    QString           message() const;

    int               error() const;

    void              finish( QWidget* );
    void              repaint();

public slots:
    void              message( const QString&, 
			       const int,
			       const QColor& = white );
    void              message( const QString& );
    void              clear();

protected:
    virtual void      mousePressEvent( QMouseEvent* );
    virtual void      customEvent( QCustomEvent* );
    virtual void      drawContents( QPainter* );

private:
    void              drawContents();
    void              setError( const int );

private:
    static QtxSplash* mySplash;

    QPixmap           myPixmap;
    QString           myMessage;
    int               myAlignment;
    QColor            myColor;
    QColor            myShadowColor;
    bool              myHideOnClick;
    int               myProgress;
    int               myTotal;
    QColor            myStartColor;
    QColor            myEndColor;
    int               myGradientType;
    int               myError;
};

#endif
