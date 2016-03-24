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

#include "PVViewer_LogWindowAdapter.h"

#include <vtkObjectFactory.h>

#include <LogWindow.h>
#include <LogWindow.h>
#include <SUIT_Session.h>
#include <SALOME_Event.h>

vtkStandardNewMacro(PVViewer_LogWindowAdapter);

/*!
 * Put the message in the log window. 
 */
class TEvent: public SALOME_Event {
  LogWindow* myWindow;
  QString    myMsg;
  QColor     myColor;
  int        myFlags;
  public:
  TEvent( LogWindow* theWindow,  const QString theMsg, const QColor theColor, const int flags) :
    myWindow ( theWindow ),
    myMsg ( theMsg ),
    myColor ( theColor ),
    myFlags (flags)
  {}

  virtual void Execute() {
    if(myWindow)
      myWindow->putMessage(myMsg, myColor, myFlags);
  }
};


PVViewer_LogWindowAdapter::PVViewer_LogWindowAdapter() :
  TextCount(0),
  ErrorCount(0),
  WarningCount(0),
  GenericWarningCount(0),
  logWindow(0)
{
}

PVViewer_LogWindowAdapter::~PVViewer_LogWindowAdapter()
{
}

const unsigned int PVViewer_LogWindowAdapter::getTextCount()
{
  return this->TextCount;
}

const unsigned int PVViewer_LogWindowAdapter::getErrorCount()
{
  return this->ErrorCount;
}

const unsigned int PVViewer_LogWindowAdapter::getWarningCount()
{
  return this->WarningCount;
}

const unsigned int PVViewer_LogWindowAdapter::getGenericWarningCount()
{
  return this->GenericWarningCount;
}

void PVViewer_LogWindowAdapter::DisplayText(const char* text)
{
  ++this->TextCount;
  ProcessVoidEvent( new TEvent( getLogWindow(), text, Qt::darkGreen, LogWindow::DisplayNormal ));
}

void PVViewer_LogWindowAdapter::DisplayErrorText(const char* text)
{
  ++this->ErrorCount;
  ProcessVoidEvent( new TEvent( getLogWindow(), text, Qt::darkRed, LogWindow::DisplayNormal ));
}

void PVViewer_LogWindowAdapter::DisplayWarningText(const char* text)
{
  ++this->WarningCount;
  ProcessVoidEvent( new TEvent( getLogWindow(), text, Qt::black, LogWindow::DisplayNormal ));
}

void PVViewer_LogWindowAdapter::DisplayGenericWarningText(const char* text)
{
  ++this->GenericWarningCount;
  ProcessVoidEvent( new TEvent( getLogWindow() , text, Qt::black, LogWindow::DisplayNormal ));
}
