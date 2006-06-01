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
// File:      QtxColorScale.h
// Author:    Sergey TELKOV

#ifndef QTXCOLORSCALE_H
#define QTXCOLORSCALE_H

#include "Qtx.h"

#include <qframe.h>
#include <qvaluelist.h>

#if QT_VER == 3
#include <qdockwindow.h>
#endif

class QStyleSheet;
class QSimpleRichText;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class QtxColorScale
  Color Scale widget.
*/
class QTX_EXPORT QtxColorScale : public QFrame
{
	  Q_OBJECT

public:
	  typedef enum { Auto, User } Mode;
	  typedef enum { None, Left, Right, Center } Position;
	  typedef enum { NoDump, TitleDump, ScaleDump, FullDump } DumpMode;
	  typedef enum { AtBorder = 0x001, Reverse = 0x002, Integer = 0x004,
				           WrapTitle = 0x008, PreciseFormat = 0x010, Transparent = 0x020 } Flags;

#if QT_VER == 3
	  class Dock : public QDockWindow
	  {
      public:
          Dock( Place = InDock, QWidget* = 0, const char* = 0, WFlags = 0 );
          virtual ~Dock();
          
          QtxColorScale* colorScale() const;
          
          void           activate();
          void           deactivate();
          
          bool           isActive() const;
          
          virtual void   show();
          virtual void   hide();
          
          virtual void   resize( int, int );
          virtual void   setOrientation( Orientation );
          
      private:
          QtxColorScale* myScale;
          bool           myBlockShow;
          bool           myBlockResize;
      };

private:
	  QtxColorScale( Dock*, const char* = 0, WFlags = 0 );
#endif

public:
	  QtxColorScale( QWidget* = 0, const char* = 0, WFlags = 0 );
	  QtxColorScale( const int, QWidget* = 0, const char* = 0, WFlags = 0 );
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
	  void                  colors( QValueList<QColor>& ) const;

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
	  void                  setColors( const QValueList<QColor>& );

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
	  virtual void          drawContents( QPainter* );

private:
	  void                  updateScale();
	  QString               getFormat() const;
	  QString               getLabel( const int ) const;
	  QColor                getColor( const int ) const;
	  double                getNumber( const int ) const;
	  QSimpleRichText*      simpleRichText( const int ) const;
	  void                  drawScale( QPainter*, const bool, const int, const int,
									                   const int, const int, const bool, const bool, const bool ) const;
	  void                  drawScale( QPainter*, const QColor&, const bool,
									                   const int, const int, const int, const int,
									                   const bool, const bool, const bool ) const;
	  QSize                 calculateSize( const bool, const int,
										                     const bool, const bool, const bool ) const;

#if QT_VER == 3
	  friend class QtxColorScale::Dock;
#endif

private:
	  double                myMin;
	  double                myMax;
	  QString               myTitle;
	  QString               myFormat;
	  QString               myPrecise;
	  int                   myInterval;
	  int                   myDumpMode;
	  int                   myColorMode;
	  int                   myLabelMode;

	  QValueList<QColor>    myColors;
	  QValueList<QString>   myLabels;

	  Dock*                 myDock;
	  int                   myFlags;
	  int                   myLabelPos;
	  int                   myTitlePos;
	  QStyleSheet*          myStyleSheet;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
