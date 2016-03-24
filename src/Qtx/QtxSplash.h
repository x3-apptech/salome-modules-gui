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

// File   : QtxSplash.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef QTXSPLASH_H
#define QTXSPLASH_H

#include "Qtx.h"

#include <QWidget>
#include <QPixmap>
#include <QGradient>
#include <QMap>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QtxResourceMgr;

class QTX_EXPORT QtxSplash : public QWidget
{
  Q_OBJECT

private:
  QtxSplash( const QPixmap& );

public:
  //! Progress bar position and direction
  typedef enum {
    LeftSide     = 0x0001,      //!< progress bar is displayed at the left side
    RightSide    = 0x0002,      //!< progress bar is displayed at the right side
    TopSide      = 0x0004,      //!< progress bar is displayed at the top side
    BottomSide   = 0x0008,      //!< progress bar is displayed at the bottom side
    LeftToRight  = 0x0010,      //!< show progress from left to right (from top to bottom)
    RightToLeft  = 0x0020       //!< show progress from right to left (from bottom to top)
  } ProgressBarFlags;
  
  virtual ~QtxSplash();
  
  static QtxSplash* splash( const QPixmap& = QPixmap() );
  
  static void       setStatus( const QString&, const int = -1 );
  static void       setError( const QString&, const QString& = QString(), const int = -1 );
  
  void              setPixmap( const QPixmap& );
  QPixmap           pixmap() const;
  
  void              setHideOnClick( const bool );
  bool              hideOnClick() const;

  void              setProgressVisible( const bool );
  bool              progressVisible() const;

  void              setMessageVisible( const bool );
  bool              messageVisible() const;

  void              setPercentageVisible( const bool );
  bool              percentageVisible() const;

  void              setTotalSteps( const int );
  int               totalSteps() const;
  
  void              setProgress( const int );
  void              setProgress( const int, const int );
  int               progress() const;
  
  void              setMargin( const int );
  int               margin() const;
  
  void              setProgressWidth( const int );
  int               progressWidth() const; 

  void              setProgressFlags( const int );
  int               progressFlags() const;
  
  void              setProgressColors( const QColor&, 
                                       const QColor& = QColor(),
                                       const Qt::Orientation = Qt::Vertical );
  void              setProgressColors( const QGradient& );
  const QGradient*  progressColors() const;
  
  void              setOpacity( const double );
  double            opacity() const;
  
  void              setTextAlignment( const int );
  int               textAlignment() const;
  
  void              setTextColors( const QColor&, const QColor& = QColor() );
  void              textColors( QColor&, QColor& ) const;
  
  void              setConstantInfo( const QString& info );
  QString           constantInfo() const;

  void              setOption( const QString&, const QString& );
  QString           option( const QString& ) const;

  QString           message() const;
  
  int               error() const;
  
  void              finish( QWidget* );
  void              repaint();
  
  void              readSettings( QtxResourceMgr*, const QString& = QString() );

public slots:
  void              setMessage( const QString&, 
                                const int,
                                const QColor& = QColor() );
  void              setMessage( const QString& );
  void              clear();
  
protected:
  virtual void      mousePressEvent( QMouseEvent* );
  virtual void      customEvent( QEvent* );
  virtual void      paintEvent( QPaintEvent* );

  virtual void      drawContents( QPainter* );
  
  virtual void      drawProgressBar( QPainter* );
  virtual void      drawMessage( QPainter* );

private:
  void              drawContents();
  void              setError( const int );
  QString           fullMessage() const;

private:
  typedef QMap<QString, QString> OptMap;
      
private:
  static QtxSplash* mySplash;
  
  QPixmap           myPixmap;           //!< splash pixmap
  QString           myInfo;             //!< constant info
  QString           myMessage;          //!< current status message
  int               myAlignment;        //!< text alignment flags (Qt::Alignment)
  QColor            myColor;            //!< text color
  QColor            myShadowColor;      //!< text shadow color
  bool              myHideOnClick;      //!< 'hide on click' flag
  int               myProgress;         //!< current progress
  int               myTotal;            //!< total progress steps
  QGradient         myGradient;         //!< progress bar custom gradient
  int               myProgressWidth;    //!< progress bar width
  int               myProgressFlags;    //!< progress bar flags (QtxSplash::ProgressBarFlags)
  int               myMargin;           //!< margin (for progress bar and status message)
  double            myOpacity;          //!< progress bar / status message opacity
  int               myError;            //!< error code
  OptMap            myOptions;          //!< constant info options
  bool              myShowProgress;     //!< 'show progress bar' flag
  bool              myShowMessage;      //!< 'show status message' flag
  bool              myShowPercent;      //!< 'show percentage' flag
};

#endif
