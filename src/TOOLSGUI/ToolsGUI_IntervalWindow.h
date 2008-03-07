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
//  File   : IntervalWindow.hxx
//  Author : Oksana TCHEBANOVA
//  Module : SALOME

#ifndef IntervalWindow_HeaderFile
#define IntervalWindow_HeaderFile

# include <qwidget.h>
# include <qdialog.h>
# include <qpushbutton.h>
# include <qspinbox.h>

#ifndef WNT
using namespace std;
#endif

class ToolsGUI_IntervalWindow : public QDialog
{
  Q_OBJECT

public:
  ToolsGUI_IntervalWindow( QWidget* parent = 0 );
  ~ToolsGUI_IntervalWindow();
  
  QPushButton* Ok();
  QPushButton* Cancel();

  int getValue();
  void setValue( int );

private:
  QSpinBox* mySpinBox;
  QPushButton* myButtonOk;
  QPushButton* myButtonCancel;
};

#endif
