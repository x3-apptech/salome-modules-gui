// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef PVVIEWER_OUTPUTWINDOW_H
#define PVVIEWER_OUTPUTWINDOW_H

#include "PVViewer.h"

#include <QMap>
#include <vtkOutputWindow.h>

/**
  vtkOutputWindow implementation that redirects VTK output messages
  to dedicated sinals which are then handled by SALOME log window.
  
  To use, create an instance of PVViewer_OutputWindow and pass it to the
  vtkOutputWindow::setInstance() static method.
  
  This class is based on pqOutputWindow ParaView class.
 */
class PVVIEWER_EXPORT PVViewer_OutputWindow : public vtkOutputWindow
{
public:
  static PVViewer_OutputWindow* New();
  vtkTypeMacro(PVViewer_OutputWindow, vtkOutputWindow);

  //! Returns the number of text messages received
  const unsigned int getTextCount();
  //! Returns the number of error messages received
  const unsigned int getErrorCount();
  //! Returns the number of warning messages received
  const unsigned int getWarningCount();
  //! Returns the number of generic warning messages received
  const unsigned int getGenericWarningCount();
  //! Returns the number of debug messages received
  const unsigned int getDebugCount();

private:
  PVViewer_OutputWindow();
  PVViewer_OutputWindow(const PVViewer_OutputWindow&);
  PVViewer_OutputWindow& operator=(const PVViewer_OutputWindow&);
  ~PVViewer_OutputWindow();

  QMap<MessageTypes, int> myCounter;

  void DisplayText(const char*);
  int count(const MessageTypes&);
};

#endif // PVVIEWER_OUTPUTWINDOW_H
