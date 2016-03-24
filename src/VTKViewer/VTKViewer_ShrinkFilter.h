// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef VTKVIEWER_SHRINKFILTER_H
#define VTKVIEWER_SHRINKFILTER_H

#include "VTKViewer.h"

#include <vtkShrinkFilter.h>

#include <vector>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*!Shrink cells composing an arbitrary data set.
 *\warning It is possible to turn cells inside out or cause self intersection in special cases.
 */
class VTKVIEWER_EXPORT VTKViewer_ShrinkFilter : public vtkShrinkFilter 
{
public:
  /*!Create new instance of VTKViewer_ShrinkFilter.*/
  static VTKViewer_ShrinkFilter *New();
  vtkTypeMacro(VTKViewer_ShrinkFilter, vtkShrinkFilter);

  void SetStoreMapping(int theStoreMapping);
  /*!Gets store mapping flag.*/
  int GetStoreMapping(){ return myStoreMapping;}

  virtual vtkIdType GetNodeObjId(int theVtkID);
  /*!Return element id by vtk id.*/
  virtual vtkIdType GetElemObjId(int theVtkID) { return theVtkID;}

protected:
  VTKViewer_ShrinkFilter();
  ~VTKViewer_ShrinkFilter();
  
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  /*!Not implemented.*/
  void UnstructuredGridExecute();
    
private:
  int myStoreMapping;   
  typedef std::vector<vtkIdType> TVectorId;
  TVectorId myVTK2ObjIds;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
