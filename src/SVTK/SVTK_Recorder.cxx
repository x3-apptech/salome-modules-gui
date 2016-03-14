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

#include "SVTK_Recorder.h"

#include "SVTK_ImageWriter.h"
#include "SVTK_ImageWriterMgr.h"

#include <vtkObjectFactory.h>
#include <vtkObject.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindow.h>
#include <vtkTimerLog.h>
#include <vtkWindowToImageFilter.h>
#include <vtkJPEGWriter.h>
#include <vtkImageData.h>

#include <sstream>
#include <iomanip>
#include <iostream>

#ifndef WIN32
#include <unistd.h>
#endif

#include <QApplication>
#include <QFileInfo>
#include <QDir>

//#include "utilities.h"

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif


namespace
{
  //----------------------------------------------------------------------------
  inline
  void
  GetNameJPEG(const std::string& thePreffix,  
              const int theIndex,
              std::string& theName)
  {
    std::ostringstream aStream;
    aStream<<thePreffix<<"_"<<setw(6)<<setfill('0')<<theIndex<<".jpeg";
    theName = aStream.str();
  }
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(SVTK_Recorder);


//----------------------------------------------------------------------------
SVTK_Recorder
::SVTK_Recorder():
  myRenderWindow(NULL),
  myState(SVTK_Recorder_Stop),
  myNbFPS(5.5),
  myQuality(100),
  myProgressiveMode(true),
  myUseSkippedFrames(true),
  myErrorStatus(0),
  myCommand(vtkCallbackCommand::New()),
  myPriority(0.0),
  myTimeStart(0.0),
  myFrameIndex(0),
  myPaused(0),
  myFilter(vtkWindowToImageFilter::New()),
  myWriterMgr(new SVTK_ImageWriterMgr),
  myNbWrittenFrames(0),
  myNameAVIMaker("jpeg2yuv")
{
  myCommand->SetClientData(this); 
  myCommand->SetCallback(SVTK_Recorder::ProcessEvents);
}


//----------------------------------------------------------------------------
SVTK_Recorder
::~SVTK_Recorder()
{
  myCommand->Delete();
  myFilter->Delete();
  delete myWriterMgr;
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::CheckExistAVIMaker()
{
  myErrorStatus = 0;
  std::ostringstream aStream;
#ifndef WIN32
  aStream<<"which "<<myNameAVIMaker<<" 2> /dev/null";
#else
  aStream<<"setlocal & set P2=.;%PATH% & (for %e in (%PATHEXT%) do @for %i in ("<<myNameAVIMaker<<"%e) do @if NOT \"%~$P2:i\"==\"\" exit /b 0) & exit /b 1";
#endif
  std::string anAVIMakeCheck = aStream.str();
  int iErr = system(anAVIMakeCheck.c_str());
  if(iErr != 0)
    myErrorStatus = 127;
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::SetName(const char* theName)
{
  myName = theName;
}

const char* 
SVTK_Recorder::Name() const
{
  return myName.c_str();
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::SetNbFPS(const double theNbFPS)
{
  myNbFPS = theNbFPS;
}

double
SVTK_Recorder
::NbFPS() const
{
  return myNbFPS;
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::SetQuality(int theQuality)
{
  myQuality = theQuality;
}

int
SVTK_Recorder
::GetQuality() const
{
  return myQuality;
}


//----------------------------------------------------------------------------
void 
SVTK_Recorder
::SetRenderWindow(vtkRenderWindow* theRenderWindow)
{
  myRenderWindow = theRenderWindow;
}

vtkRenderWindow* 
SVTK_Recorder
::RenderWindow()
{
  return myRenderWindow;
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::SetProgressiveMode(bool theProgressiveMode)
{
  myProgressiveMode = theProgressiveMode;
}

bool
SVTK_Recorder
::GetProgressiveMode() const
{
  return myProgressiveMode;
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::SetUseSkippedFrames(bool theUseSkippedFrames)
{
  myUseSkippedFrames = theUseSkippedFrames;
}

bool
SVTK_Recorder
::UseSkippedFrames() const
{
  return myUseSkippedFrames;
}


//----------------------------------------------------------------------------
int
SVTK_Recorder
::ErrorStatus() const
{
  return myErrorStatus;
}

int
SVTK_Recorder
::State() const
{
  return myState;
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::ProcessEvents(vtkObject* vtkNotUsed(theObject), 
                unsigned long theEvent,
                void* theClientData, 
                void* vtkNotUsed(theCallData))
{
  if(vtkObject* anObj = reinterpret_cast<vtkObject*>(theClientData)){ 
    if(SVTK_Recorder* aSelf = dynamic_cast<SVTK_Recorder*>(anObj)){
      if(theEvent==vtkCommand::EndEvent){
        if(aSelf->State() == SVTK_Recorder::SVTK_Recorder_Record){
          aSelf->DoRecord();
        }
      }
    }
  }
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::Record()
{
  if(myState == SVTK_Recorder_Stop){
    if(myRenderWindow){
      myState = SVTK_Recorder_Record;
      myFilter->SetInput(myRenderWindow);
      myFrameIndex = -1;
      myNbWrittenFrames = 0;
      myRenderWindow->RemoveObserver(myCommand);
      myRenderWindow->AddObserver(vtkCommand::EndEvent,
                                  myCommand,
                                  myPriority);
      myRenderWindow->Render();
    }
  }
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::Stop()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );

  if(myState == SVTK_Recorder_Record){ 
    if(!myPaused)
      DoRecord();

    myWriterMgr->Stop();

    if(myUseSkippedFrames)
      AddSkippedFrames();

    myFrameIndexes.clear();

    MakeFileAVI();
  }
  myState = SVTK_Recorder_Stop;
  myPaused = 0;

  QApplication::restoreOverrideCursor();
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::Pause()
{
  myPaused = myPaused ? 0 : 1;
  if(myPaused && !myFrameIndexes.empty()){
    myFrameIndexes.back() *= -1;
    if(MYDEBUG) cout<<"SVTK_Recorder::Pause - myFrameIndexes.back() = "<<myFrameIndexes.back()<<endl;
  }
}


//----------------------------------------------------------------------------
inline 
int
GetFrameIndex(double theStartTime,
              double theFPS)
{
  double aTimeNow = vtkTimerLog::GetUniversalTime();
  double aDelta = aTimeNow - theStartTime;
  return int(aDelta*theFPS);
}

void
SVTK_Recorder
::DoRecord()
{
  if(myPaused)
    return;

  if(myFrameIndex < 0){
    myFrameIndex = 0;
    myTimeStart = vtkTimerLog::GetUniversalTime();
  }else{
    int aFrameIndex = GetFrameIndex(myTimeStart,myNbFPS);
    if(aFrameIndex <= myFrameIndex)
      return;

    // If there was a "pause" we correct the myTimeStart
    int aLastFrameIndex = myFrameIndexes.back();
    if(aLastFrameIndex < 0){
      myFrameIndexes.back() = abs(myFrameIndexes.back());
      double aPauseTime = fabs((double)(aFrameIndex - myFrameIndex - 1)) / myNbFPS;
      if(MYDEBUG) 
        cout<<"SVTK_Recorder::DoRecord - aFrameIndex = "<<aFrameIndex<<
          "; aPauseTime = "<<aPauseTime<<endl;
      myTimeStart += aPauseTime;
    }

    aFrameIndex = GetFrameIndex(myTimeStart,myNbFPS);
    if(aFrameIndex <= myFrameIndex)
      return;

    myFrameIndex = aFrameIndex;
  }

  myFrameIndexes.push_back(myFrameIndex);
  if(MYDEBUG) cout<<"SVTK_Recorder::DoRecord - myFrameIndex = "<<myFrameIndex<<endl;

  myRenderWindow->RemoveObserver(myCommand);
  myFilter->Modified();

  std::string aName;
  GetNameJPEG(myName,myFrameIndex,aName);

  PreWrite();

  vtkImageData *anImageData = vtkImageData::New(); 
  anImageData->DeepCopy(myFilter->GetOutput());

  myWriterMgr->StartImageWriter(myFilter,anImageData,aName,myProgressiveMode,myQuality);
  myNbWrittenFrames++;

  myRenderWindow->AddObserver(vtkCommand::EndEvent,
                              myCommand,
                              myPriority);
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::PreWrite()
{
  vtkImageData *anImageData = myFilter->GetOutput();
  //
  if(!anImageData){
    myErrorStatus = 20;
    return;
  }
  myFilter->UpdateInformation();
  myFilter->SetUpdateExtentToWholeExtent();
  myFilter->Update();
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::AddSkippedFrames()
{
  myErrorStatus = 0;

  if(myFrameIndexes.size() < 2)
    return;

  size_t anId = 0, anEnd = myFrameIndexes.size() - 1;
  for(; anId < anEnd; anId++){
    int aStartIndex = myFrameIndexes[anId];
    if(aStartIndex < 0)
      continue;

    int aFinishIndex = abs(myFrameIndexes[anId + 1]);
    if(aStartIndex + 1 == aFinishIndex)
      continue;

    std::string anInitialName;
    std::ostringstream aStream;
    GetNameJPEG(myName,aStartIndex,anInitialName);
    for(int anIndex = aStartIndex + 1; anIndex < aFinishIndex; anIndex++){
      myNbWrittenFrames++;
      std::string anCurrentName;
      GetNameJPEG(myName,anIndex,anCurrentName);
  #ifndef WIN32
      aStream<<"ln -s "<< anInitialName<<" "<<anCurrentName<<";";
  #else
      aStream<<"COPY /Y "<<QString::fromStdString(anInitialName).replace("/","\\\\").toStdString()<<
		  " "<<QString::fromStdString(anCurrentName).replace("/","\\\\").toStdString()<<" > NUL";
  #endif
      if(anIndex + 1 < aFinishIndex)
  #ifndef WIN32
        aStream<<" \\";
        aStream<<endl;
  #else
        aStream<<" & ";
  #endif
    }
    std::string aString(aStream.str());
    system(aString.c_str());
    if(MYDEBUG) cout<<"SVTK_Recorder::AddSkippedFrames - "<<aString<<endl;
  }
}


//----------------------------------------------------------------------------
void
SVTK_Recorder
::MakeFileAVI()
{
  myErrorStatus = 0;
  std::ostringstream aStream;
  aStream<<myNameAVIMaker<<
    " -I p"<<
    " -v 0"<<
    //" -f "<<int(myNbFPS)<<" "<<
    " -f "<<myNbFPS<<" "<<
    " -n "<<myNbWrittenFrames<<" "<<
    " -j \""<<myName<<"_\%06d.jpeg\" "<<
    "| yuv2lav"<<" -o \""<<myName<<"\"";
#ifdef WIN32
  aStream<<" -f aA";   
#endif
  std::string aString(aStream.str());
  myErrorStatus = system(aString.c_str());

  if(MYDEBUG) cout<<"SVTK_Recorder::MakeFileAVI - "<<aString<<endl;

  QFileInfo aFileInfo(myName.c_str());
  QString aDirPath = aFileInfo.absoluteDir().path();
  QString aBaseName = aFileInfo.fileName();
  QString aCommand;
#ifndef WIN32
  aCommand = QString("(cd ") + aDirPath + 
    "; ls " +
    " | egrep '" + aBaseName + "_[0-9]*.jpeg'" +
    " | xargs rm " +
    ")";
#else
  QString tmpFile = QString("_") + aBaseName + "_tempfile";
  QString diskName = aDirPath.split("/")[0];
  aCommand = diskName + " && (cd " + aDirPath.replace("/","\\\\") + 
	" && ((dir /b | findstr " + aBaseName + "_[0-9]*.jpeg > " + tmpFile + 
	") & (for /f %i in (" + tmpFile + ") do (del \"%i\")) & (del " + tmpFile + "))) > NUL";
#endif

  if(MYDEBUG) cout<<"SVTK_Recorder::MakeFileAVI - "<<(const char*)aCommand.toLatin1()<<endl;
  system((const char*)aCommand.toLatin1());
}
