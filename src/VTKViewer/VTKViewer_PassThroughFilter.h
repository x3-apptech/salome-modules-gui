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
#ifndef VTKVIEWER_PASSTHROUGHFILTER_H
#define VTKVIEWER_PASSTHROUGHFILTER_H

#include "VTKViewer.h"

#include <vtkDataSetToDataSetFilter.h>

class VTKVIEWER_EXPORT VTKViewer_PassThroughFilter : public vtkDataSetToDataSetFilter
{
public:
  vtkTypeRevisionMacro( VTKViewer_PassThroughFilter, vtkDataSetToDataSetFilter );
  void PrintSelf( ostream& os, vtkIndent indent );

  /*!Create a new VTKViewer_PassThroughFilter.*/
  static VTKViewer_PassThroughFilter *New();

protected:
  VTKViewer_PassThroughFilter() {};//!< Null body.
  virtual ~VTKViewer_PassThroughFilter() {};//!< Null body.

  void Execute();

private:
  VTKViewer_PassThroughFilter( const VTKViewer_PassThroughFilter& );  //!< Not implemented.
  void operator=( const VTKViewer_PassThroughFilter& );               //!< Not implemented.
};

#endif
