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

// File:      QtxColorScale.h
// Author:    Sergey TELKOV
//
#ifndef QTXCOLORSCALE_H
#define QTXCOLORSCALE_H

#include "Qtx.h"

#include <QFrame>
#include <QList>

class QTextDocument;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxColorScale : public QFrame
{
  Q_OBJECT

public:
  //! Color scale color/label mode.
  typedef enum { 
    Auto,            //!< auto
    User             //!< user defined
  } Mode;
  //! Color scale title, label position.
  typedef enum { 
    None,            //!< do not draw
    Left,            //!< draw at the left
    Right,           //!< draw at the right
    Center           //!< draw at the center
  } Position;
  //! Dump mode.
  typedef enum { 
    NoDump,          //!< do not dump
    TitleDump,       //!< dump title
    ScaleDump,       //!< dump scale
    FullDump         //!< dump all
  } DumpMode;
  //! Color scale flags (bitwise).
  typedef enum { 
    AtBorder      = 0x001,   //!< diplay values at colors boundaries
    Reverse       = 0x002,   //!< display color scale in reverse order
    Integer       = 0x004,   //!< round numbers to integer values
    WrapTitle     = 0x008,   //!< wrap title to several lines
    PreciseFormat = 0x010,   //!< autodetect decimal point precision for color scale values
    Transparent   = 0x020    //!< transparent background (not implemented yet!)
  } Flags;

public:
  QtxColorScale( QWidget* = 0, Qt::WindowFlags = 0 );
  QtxColorScale( const int, QWidget* = 0, Qt::WindowFlags = 0 );
  virtual ~QtxColorScale();

  double                minimum() const;
  double                maximum() const;
  void                  range( double&, double& ) const;
  int                   dumpMode() const;
  int                   labelMode() const;
  int                   colorMode() const;
  int                   intervalsNumber() const;

  QString               title() const;
  QString               format() const;
  QString               label( const int ) const;
  QColor                color( const int ) const;
  void                  labels( QStringList& ) const;
  void                  colors( QList<QColor>& ) const;

  int                   labelPosition() const;
  int                   titlePosition() const;

  void                  setMinimum( const double );
  void                  setMaximum( const double );
  void                  setRange( const double, const double );
  void                  setDumpMode( const int );
  void                  setColorMode( const int );
  void                  setLabelMode( const int );
  void                  setIntervalsNumber( const int );

  void                  setTitle( const QString& );
  void                  setFormat( const QString& );
  void                  setLabel( const QString&, const int = -1 );
  void                  setColor( const QColor&, const int = -1 );
  void                  setLabels( const QStringList& );
  void                  setColors( const QList<QColor>& );

  void                  setLabelPosition( const int );
  void                  setTitlePosition( const int );

  void                  setFlags( const int );
  bool                  testFlags( const int ) const;
  void                  clearFlags( const int );

  QPixmap               dump() const;
  QPixmap               dump( const int = -1, const int = -1 ) const;
  QPixmap               dump( const QColor&, const int = -1, const int = -1 ) const;

  virtual QSize         minimumSizeHint() const;
  virtual QSize         sizeHint() const;

  virtual void          show();
  virtual void          hide();

protected:
  virtual void          paintEvent( QPaintEvent* );
  virtual void          drawContents( QPainter* );

private:
  void                  updateScale();
  QString               getFormat() const;
  QString               getLabel( const int ) const;
  QColor                getColor( const int ) const;
  double                getNumber( const int ) const;
  QTextDocument*        textDocument( const int ) const;
  void                  drawScale( QPainter*, const bool, const int, const int,
                                   const int, const int, const bool, const bool, const bool ) const;
  void                  drawScale( QPainter*, const QColor&, const bool,
                                   const int, const int, const int, const int,
                                   const bool, const bool, const bool ) const;
  QSize                 calculateSize( const bool, const int,
                                       const bool, const bool, const bool ) const;

private:
  double                myMin;             //!< lower limit
  double                myMax;             //!< upper limit
  QString               myTitle;           //!< title
  QString               myFormat;          //!< number presentation format
  QString               myPrecise;         //!< double values precision format
  int                   myInterval;        //!< number of color scale intervals
  int                   myDumpMode;        //!< dump mode (QtxColorScale::DumpMode)
  int                   myColorMode;       //!< color mode (QtxColorScale::Mode)
  int                   myLabelMode;       //!< label mode (QtxColorScale::Mode)

  QList<QColor>         myColors;          //!< list of colors
  QList<QString>        myLabels;          //!< list of labels

  int                   myFlags;           //!< color scale flags (QtxColorScale::Flags)
  int                   myLabelPos;        //!< label position (QtxColorScale::Position)
  int                   myTitlePos;        //!< title position (QtxColorScale::Position)
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif  // QTXCOLORSCALE_H
