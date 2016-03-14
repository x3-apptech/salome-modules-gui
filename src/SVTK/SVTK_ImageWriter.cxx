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

#include "SVTK_ImageWriter.h"

#include <QSemaphore>

#include <vtkAlgorithm.h>
#include <vtkImageData.h>
#include <vtkImageClip.h>
#include <vtkJPEGWriter.h>
#include <vtkSmartPointer.h>

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif


//----------------------------------------------------------------------------
SVTK_ImageWriter
::SVTK_ImageWriter(QSemaphore* theSemaphore,
                   vtkAlgorithm* theAlgorithm,
                   vtkImageData* theImageData,
                   const std::string& theName,
                   int theProgressive,
                   int theQuality):
  mySemaphore(theSemaphore),
  myAlgorithm(theAlgorithm),
  myImageData(theImageData),
  myName(theName),
  myProgressive(theProgressive),
  myQuality(theQuality),
  myConstraint16Flag(true)
{}

//----------------------------------------------------------------------------
SVTK_ImageWriter
::~SVTK_ImageWriter()
{
  if(MYDEBUG) cout<<"SVTK_ImageWriter::~SVTK_ImageWriter - this = "<<this<<endl;
}


//----------------------------------------------------------------------------
void
SVTK_ImageWriter
::run()
{
  vtkJPEGWriter *aWriter = vtkJPEGWriter::New();
  vtkAlgorithmOutput *anImageData = 0;
  vtkSmartPointer<vtkImageClip> anImageClip;
  //
  if(myConstraint16Flag){ 
    int uExtent[6];
    myAlgorithm->UpdateInformation();
    myAlgorithm->GetUpdateExtent(uExtent);
    unsigned int width = uExtent[1] - uExtent[0] + 1;
    unsigned int height = uExtent[3] - uExtent[2] + 1;
    width = (width / 16) * 16;
    height= (height / 16) * 16;
    uExtent[1] = uExtent[0] + width - 1;
    uExtent[3] = uExtent[2] + height - 1;
    //
    anImageClip = vtkImageClip::New();
    anImageClip->Delete();

    anImageClip->SetInputData(myImageData);
    anImageClip->SetOutputWholeExtent(uExtent);
    anImageClip->ClipDataOn();
    anImageData = anImageClip->GetOutputPort();
  }
  //
  aWriter->WriteToMemoryOff();
  aWriter->SetFileName(myName.c_str());
  aWriter->SetQuality(myQuality);
  aWriter->SetProgressive(myProgressive);
  if(myConstraint16Flag)
    aWriter->SetInputConnection(anImageData);
  else
    aWriter->SetInputData(myImageData);
  aWriter->Write();

  aWriter->Delete();
  myImageData->Delete();

  if(MYDEBUG) cout<<"SVTK_ImageWriter::run "<<
                "- this = "<<this<<
                //"; total = "<<mySemaphore->total()<<
                "; available = "<<mySemaphore->available()<<endl;
  mySemaphore->release();
}

