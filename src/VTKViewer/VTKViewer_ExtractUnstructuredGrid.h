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

#ifndef VTKVIEWER_EXTRACTUNSTRUCTUREDGRID_H
#define VTKVIEWER_EXTRACTUNSTRUCTUREDGRID_H

#include "VTKViewer.h"

#include <vtkUnstructuredGridAlgorithm.h>

#include <set>
#include <map>
#include <vector>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*! \class vtkUnstructuredGridAlgorithm
 * \brief For more information see <a href="http://www.vtk.org/">VTK documentation</a>
 */
/*! \class vtkUnstructuredGridAlgorithm
 * \brief For more information see VTK documentation.
 */
class VTKVIEWER_EXPORT VTKViewer_ExtractUnstructuredGrid : public vtkUnstructuredGridAlgorithm
{
public:
  //! VTK type macros.
  vtkTypeMacro( VTKViewer_ExtractUnstructuredGrid, vtkUnstructuredGridAlgorithm );

  //! \brief Construct with all types of clipping turned off.
  static VTKViewer_ExtractUnstructuredGrid *New();

  enum EExtraction{ eCells, ePoints};
  //! Sets mode of extraction to \a theExtractionMode
  void SetModeOfExtraction(EExtraction theExtractionMode){
    if ( myExtractionMode != theExtractionMode ) { myExtractionMode = theExtractionMode; Modified(); }
  }
  //! Get Extraction mode (Return: \a myExtractionMode field)
  EExtraction GetModeOfExtraction(){ return myExtractionMode;}

  enum EChanging{ ePassAll, eAdding, eRemoving};
  //! Sets mode of changing to \a theChangeMode
  void SetModeOfChanging(EChanging theChangeMode){
    if ( myChangeMode != theChangeMode ) { myChangeMode = theChangeMode; Modified(); }
  }
  //! Return \a myChangeMode field
  EChanging GetModeOfChanging(){ return myChangeMode; }

  //! Add cell id to \a myCellIds std::set
  void RegisterCell(vtkIdType theCellId);
  //! Check if myCellIds is empty.
  int IsCellsRegistered() { return !myCellIds.empty(); }
  //! Remove the cell from the output
  void ClearRegisteredCells() {
    if ( !myCellIds.empty() ) { myCellIds.clear(); Modified(); }
  }

  //! Add cell type to \a myCellTypes std::set
  void RegisterCellsWithType(vtkIdType theCellType);
  //! Check if myCellTypes is empty.
  int IsCellsWithTypeRegistered() { return !myCellTypes.empty(); }
  //! Remove every cells with the type from the output
  void ClearRegisteredCellsWithType() {
    if ( !myCellTypes.empty() ) { myCellTypes.clear(); Modified(); }
  }

  //! \brief Do the filter do some real work
  int IsChanging() { return IsCellsRegistered() || IsCellsWithTypeRegistered(); }

  //! \brief Do it keep the mapping between input's and output's UnstructuredGrid
  void SetStoreMapping(int theStoreMapping);
  //! Get \a myStoreMapping
  int GetStoreMapping(){ return myStoreMapping; }

  //! Computes a map out IDs to in IDs. Call it before GetInputId()!!!
  void BuildOut2InMap();
  //! Gets the input id by output id. Call BuildOut2InMap() before
  vtkIdType GetInputId(int theOutId) const;
  //! Gets the output id by input id.
  //vtkIdType GetOutputId(int theInId) const;

  typedef std::vector<vtkIdType>        TVectorId;
  typedef std::map<vtkIdType,vtkIdType> TMapId;
  typedef std::set<vtkIdType>           TSetId;

protected:
  VTKViewer_ExtractUnstructuredGrid();
  ~VTKViewer_ExtractUnstructuredGrid();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  EExtraction myExtractionMode;
  
  EChanging myChangeMode;
  TSetId myCellIds;
  TSetId myCellTypes;

  bool myStoreMapping, myPassAll;
  TVectorId myOut2InId;
  //TMapId myIn2OutId;

private:
  //! Not implemented.
  VTKViewer_ExtractUnstructuredGrid(const VTKViewer_ExtractUnstructuredGrid&);
  //! Not implemented.
  void operator=(const VTKViewer_ExtractUnstructuredGrid&);
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
