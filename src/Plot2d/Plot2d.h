//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : Plot2d.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef PLOT2D_H
#define PLOT2D_H

#ifdef WIN32
#  ifdef PLOT2D_EXPORTS
#    define PLOT2D_EXPORT __declspec(dllexport)
#  else
#    define PLOT2D_EXPORT __declspec(dllimport)
#  endif
#else
#  define PLOT2D_EXPORT
#endif

#include <qwt_symbol.h>
#include <Qt>

class QPainter;

namespace Plot2d
{
  typedef enum { None, 
		 Circle,  
		 Rectangle,
		 Diamond,
		 DTriangle,
		 UTriangle,
		 LTriangle,
		 RTriangle,
		 Cross,
		 XCross
  } MarkerType;

  typedef enum {
    NoPen,
    Solid, 
    Dash, 
    Dot, 
    DashDot, 
    DashDotDot
  } LineType;

  QwtSymbol::Style         plot2qwtMarker( MarkerType );
  MarkerType qwt2plotMarker( QwtSymbol::Style );

  Qt::PenStyle             plot2qwtLine( LineType );
  LineType                 qwt2plotLine( Qt::PenStyle );

  void                     drawLine( QPainter*, const QPoint&, const QPoint&, 
				     Qt::PenStyle = Qt::SolidLine, 
				     const QColor& = Qt::black, int = 0 );
  void                     drawLine( QPainter*, const QPoint&, const QPoint&, 
				     LineType = Solid, 
				     const QColor& = Qt::black, int = 0 );
  void                     drawLine( QPainter*, int, int, int, int,
				     Qt::PenStyle = Qt::SolidLine, 
				     const QColor& = Qt::black, int = 0 );
  void                     drawLine( QPainter*, int, int, int, int,
				     LineType = Solid, 
				     const QColor& = Qt::black, int = 0 );

  void                     drawMarker( QPainter*, const QPoint&, const QRect&,
				       QwtSymbol::Style = QwtSymbol::Ellipse,
				       const QColor& = Qt::black );
  void                     drawMarker( QPainter*, const QPoint&, const QRect&,
				       MarkerType = Circle,
				       const QColor& = Qt::black );
  void                     drawMarker( QPainter*, int, int, int, int,
				       QwtSymbol::Style = QwtSymbol::Ellipse,
				       const QColor& = Qt::black );
  void                     drawMarker( QPainter*, int, int, int, int,
				       MarkerType = Circle,
				       const QColor& = Qt::black );
}

#if defined WIN32
#  pragma warning ( disable: 4251 )
#endif

#endif // PLOT2D_H
