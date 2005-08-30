//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 


#include <stdexcept>
#include <qapplication.h>

#include <CASCatch_CatchSignals.hxx>
#include <CASCatch_ErrorHandler.hxx>
#include <CASCatch_Failure.hxx>

extern "C" int HandleSignals(QApplication *theQApplication)
{
  int aRet = -1; 
  CASCatch_CatchSignals aCatchSignals;
  aCatchSignals.Activate();
        
  CASCatch_TRY {       
    aRet = theQApplication->exec();
  }
  CASCatch_CATCH(CASCatch_Failure) {
    aCatchSignals.Deactivate();    
    Handle(CASCatch_Failure) aFail = CASCatch_Failure::Caught();
    throw std::runtime_error( aFail->GetError() );   
  }
  
  aCatchSignals.Deactivate(); 
  return aRet;
}
