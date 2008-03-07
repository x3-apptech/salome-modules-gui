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
// File:      Plot2d_ToolTip.h
// Author:    Alexandre SOLOVYOV

#ifndef PLOT2D_TOOLTIP_H
#define PLOT2D_TOOLTIP_H

#include <Plot2d.h>
#include <QtxToolTip.h>

class Plot2d_ViewFrame;
class Plot2d_Plot2d;

class PLOT2D_EXPORT Plot2d_ToolTip : public QtxToolTip
{
  Q_OBJECT

public:
  Plot2d_ToolTip( Plot2d_ViewFrame*, Plot2d_Plot2d* );
  virtual ~Plot2d_ToolTip();

public slots:
  void onToolTip( QPoint, QString&, QFont&, QRect&, QRect& );

protected:
  virtual bool eventFilter( QObject*, QEvent* );

private:
  Plot2d_ViewFrame* myFrame;
  Plot2d_Plot2d*    myPlot;
};

#endif
