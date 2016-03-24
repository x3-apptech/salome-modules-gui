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

#include "SVTK_ImageWriterMgr.h"
#include "SVTK_ImageWriter.h"

#include <vtkAlgorithm.h>
#include <vtkImageData.h>

#include <QSemaphore>

#include <limits>


#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif

//----------------------------------------------------------------------------
SVTK_ImageWriterMgr
::SVTK_ImageWriterMgr()
{
  int aMax = std::numeric_limits<int>::max() / 2;
  mySemaphore = new QSemaphore(aMax);
  mySemaphore->acquire( aMax );
  if(MYDEBUG) cout<<"SVTK_ImageWriterMgr::SVTK_ImageWriterMgr "<<
                //"- total = "<<mySemaphore->total()<<
                "; available = "<<mySemaphore->available()<<endl;
}


//----------------------------------------------------------------------------
SVTK_ImageWriterMgr
::~SVTK_ImageWriterMgr()
{
  Stop();
  delete mySemaphore;
}


//----------------------------------------------------------------------------
void
SVTK_ImageWriterMgr
::StartImageWriter(vtkAlgorithm *theAlgorithm,
                   vtkImageData *theImageData,
                   const std::string& theName,
                   const int theProgressive,
                   const int theQuality)
{
  SVTK_ImageWriter *anImageWriter = 
    new SVTK_ImageWriter(mySemaphore,
                         theAlgorithm,
                         theImageData,
                         theName,
                         theProgressive,
                         theQuality);
  myThreads.push_back(anImageWriter);

  anImageWriter->start();

}


//----------------------------------------------------------------------------
void
SVTK_ImageWriterMgr
::Stop()
{
  if(MYDEBUG) cout<<"SVTK_ImageWriterMgr::Stop "<<
                //"- total = "<<mySemaphore->total()<<
                "; available = "<<mySemaphore->available()<<endl;
  if(MYDEBUG) cout<<"SVTK_ImageWriterMgr::Stop - *mySemaphore += "<<myThreads.size()<<endl;
  mySemaphore->acquire( myThreads.size() );

  for(size_t anId = 0, anEnd = myThreads.size(); anId < anEnd; anId++){
    SVTK_ImageWriter* anImageWriter = myThreads[anId];
    anImageWriter->wait();
    delete anImageWriter;
  }
  myThreads.clear();
}
