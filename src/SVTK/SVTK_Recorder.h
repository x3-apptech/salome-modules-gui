// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef SVTK_RECORDER_H
#define SVTK_RECORDER_H

#include <list>
#include <string>
#include <vector>

#include <vtkObject.h>

class vtkRenderWindow;
class vtkCallbackCommand;
class vtkWindowToImageFilter;
class SVTK_ImageWriterMgr;

class SVTK_Recorder : public vtkObject 
{
 protected:
  enum State {
    SVTK_Recorder_Unknown=0,
    SVTK_Recorder_Record,
    SVTK_Recorder_Stop
  };
  
 public:
  static SVTK_Recorder *New();
  vtkTypeMacro(SVTK_Recorder,vtkObject);

  void
  SetRenderWindow(vtkRenderWindow* theRenderWindow);

  vtkRenderWindow* 
  RenderWindow();

  void 
  SetName(const char *theName);

  const char* 
  Name() const;

  void
  SetNbFPS(const double theNbFPS);

  double
  NbFPS() const;

  void
  SetQuality(int theQuality);

  int
  GetQuality() const;

  void
  SetProgressiveMode(bool theProgressiveMode);

  bool
  GetProgressiveMode() const;

  void
  SetUseSkippedFrames(bool theUseSkippedFrames);

  bool
  UseSkippedFrames() const;

  void
  Record();

  void
  Pause();

  void
  Stop();

  int
  State() const;

  int  
  ErrorStatus() const;

  void
  CheckExistAVIMaker();

protected :
  SVTK_Recorder();

  ~SVTK_Recorder();

  void
  DoRecord();

  void
  MakeFileAVI();

  void
  AddSkippedFrames();

  void
  PreWrite();

  static
  void
  ProcessEvents(vtkObject* theObject, 
                unsigned long theEvent,
                void* theClientData, 
                void* theCallData);

protected :
  int myState;
  int myPaused;
  int myErrorStatus;

  float myPriority;
  double myTimeStart;

  int myFrameIndex;
  int myNbWrittenFrames;

  double myNbFPS;
  int myQuality;
  bool myProgressiveMode;

  typedef std::vector<int> TFrameIndexes;
  TFrameIndexes myFrameIndexes;
  bool myUseSkippedFrames;

  std::string myName;
  std::string myNameAVIMaker;

  vtkCallbackCommand *myCommand;
  vtkRenderWindow *myRenderWindow;
  vtkWindowToImageFilter *myFilter;
  SVTK_ImageWriterMgr *myWriterMgr;

private:
  SVTK_Recorder(const SVTK_Recorder&);  //Not implemented
  void operator=(const SVTK_Recorder&); //Not implemented
};

#endif
