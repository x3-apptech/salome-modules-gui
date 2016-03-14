// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

// File:      QtxToolTip.h
// Author:    Sergey TELKOV
//
#ifndef QTXTOOLTIP_H
#define QTXTOOLTIP_H

#include "Qtx.h"

#include <QLabel>

class QTimer;

class QTX_EXPORT QtxToolTip : public QLabel
{
        Q_OBJECT

public:
        QtxToolTip( QWidget* = 0 );
        virtual ~QtxToolTip();

        void          hideTip();
  
        virtual void  showTip( const QPoint& aPos,
                         const QString& text, const QRect& aWidgetRegion );
        virtual void  showTip( const QRect& aRegion,
                         const QString& text, const QRect& aWidgetRegion );

        virtual bool  eventFilter( QObject* o, QEvent* e );

  void          setWakeUpDelayTime( int );
  void          setShowDelayTime( int );

  int           wakeUpDelayTime() const { return myWakeUpDelayTime; }
  int           showDelayTime() const { return myShowDelayTime; }

signals:
  void          maybeTip( QPoint, QString&, QFont&, QRect&, QRect& );

protected slots:
        void          onSleepTimeOut();
        void          onWakeUpTimeOut();

protected:
        virtual void  maybeTip( const QPoint& );
        virtual void  mousePressEvent( QMouseEvent* );
        virtual void  mouseDoubleClickEvent( QMouseEvent* );

  QTimer*       sleepTimer() const;
  QTimer*       wakeUpTimer() const;

private:
        QTimer*       myWakeUpTimer;
        QTimer*       mySleepTimer;
        QRect         myWidgetRegion;

  int           myShowDelayTime;
  int           myWakeUpDelayTime;
};

#endif
