//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME FILTER : interactive object for VISU entities implementation
//  File   : SALOME_PassThroughFilter.cxx
//  Author : Laurent CORNABE with help of Nicolas REJNERI
//  Module : SALOME
//
#include "VTKViewer_PassThroughFilter.h"

#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

vtkCxxRevisionMacro(VTKViewer_PassThroughFilter, "$Revision$");
vtkStandardNewMacro(VTKViewer_PassThroughFilter);

/*! \class VTKViewer_PassThroughFilter
 * Passive filter take a dataset as input and create a dataset as output.\n
 * The form of the input geometry is not changed in these filters, \n
 * only the point attributes (e.g. scalars, vectors, etc.). 
 */

/*!Execute method.Output calculation.*/
int VTKViewer_PassThroughFilter::RequestData(
  vtkInformation *,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataSet *output = vtkDataSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // This has to be here because it initialized all field datas.
  output->CopyStructure( input );
  
  //! Pass all. (data object's field data is passed by the
  //! superclass after this method)
  output->GetPointData()->PassData( input->GetPointData() );
  output->GetCellData()->PassData( input->GetCellData() );

  return 1;
}

/*!Methods invoked by print to print information about the object including superclasses.\n
 * Typically not called by the user (use Print() instead) but used in the hierarchical \n
 * print process to combine the output of several classes. 
 *\param os - output stream.
 */
void VTKViewer_PassThroughFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
