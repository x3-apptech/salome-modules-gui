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

#ifndef _PVViewer_LogWindowAdapter_h
#define _PVViewer_LogWindowAdapter_h

#include "PVViewer.h"

#include <vtkOutputWindow.h>

class LogWindow;

/*!
vtkOutputWindow implementation that puts VTK output messages to SALOME log window.

To use, create an instance of PVViewer_LogWindowAdapter and pass it to the
vtkOutputWindow::setInstance() static method.

This class is based on pqOutputWindow ParaView class.
*/
class PVVIEWER_EXPORT PVViewer_LogWindowAdapter : public vtkOutputWindow
{
public:
  static PVViewer_LogWindowAdapter *New();
  vtkTypeMacro(PVViewer_LogWindowAdapter, vtkOutputWindow);

  //! Returns the number of text messages received
  const unsigned int getTextCount();
  //! Returns the number of error messages received
  const unsigned int getErrorCount();
  //! Returns the number of warning messages received
  const unsigned int getWarningCount();
  //! Returns the number of generic warning messages received
  const unsigned int getGenericWarningCount();

  void setLogWindow( LogWindow* w) { logWindow = w; }
  LogWindow* getLogWindow() { return logWindow; }

private:
  PVViewer_LogWindowAdapter();
  PVViewer_LogWindowAdapter(const PVViewer_LogWindowAdapter&);
  PVViewer_LogWindowAdapter& operator=(const PVViewer_LogWindowAdapter&);
  ~PVViewer_LogWindowAdapter();

  unsigned int TextCount;
  unsigned int ErrorCount;
  unsigned int WarningCount;
  unsigned int GenericWarningCount;
  LogWindow * logWindow;

  virtual void DisplayText(const char*);
  virtual void DisplayErrorText(const char*);
  virtual void DisplayWarningText(const char*);
  virtual void DisplayGenericWarningText(const char*);
};

#endif // !_PVViewer_LogWindowAdapter_h
