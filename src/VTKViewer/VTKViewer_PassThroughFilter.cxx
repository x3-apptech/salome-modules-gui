//  SALOME FILTER : interactive object for VISU entities implementation
//
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SALOME_PassThroughFilter.cxx
//  Author : Laurent CORNABE with help of Nicolas REJNERI
//  Module : SALOME


#include "VTKViewer_PassThroughFilter.h"

#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>

vtkCxxRevisionMacro(VTKViewer_PassThroughFilter, "$Revision$");
vtkStandardNewMacro(VTKViewer_PassThroughFilter);

/*! \class VTKViewer_PassThroughFilter
 * Passive filter take a dataset as input and create a dataset as output.\n
 * The form of the input geometry is not changed in these filters, \n
 * only the point attributes (e.g. scalars, vectors, etc.). 
 */

/*!Execute method.Output calculation.*/
void VTKViewer_PassThroughFilter::Execute()
{
  vtkDataSet *input = static_cast<vtkDataSet*>(this->GetInput());
  vtkDataSet *output = static_cast<vtkDataSet*>(this->GetOutput());

  // This has to be here because it initialized all field datas.
  output->CopyStructure( input );
  
  //! Pass all. (data object's field data is passed by the
  //! superclass after this method)
  output->GetPointData()->PassData( input->GetPointData() );
  output->GetCellData()->PassData( input->GetCellData() );

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
