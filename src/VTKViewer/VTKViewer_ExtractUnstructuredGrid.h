#ifndef VTKVIEWER_EXTRACTUNSTRUCTUREDGRID_H
#define VTKVIEWER_EXTRACTUNSTRUCTUREDGRID_H

#include "VTKViewer.h"

#include <vtkUnstructuredGridToUnstructuredGridFilter.h>

#include <set>
#include <map>
#include <vector>
/*! \class vtkUnstructuredGridToUnstructuredGridFilter
 * \brief For more information see <a href="http://www.vtk.org/">VTK documentation
 */
/*! \class vtkUnstructuredGridToUnstructuredGridFilter
 * \brief For more information see VTK documentation.
 */
class VTKVIEWER_EXPORT VTKViewer_ExtractUnstructuredGrid : public vtkUnstructuredGridToUnstructuredGridFilter
{
public:
  //! VTK type macros.
  vtkTypeMacro( VTKViewer_ExtractUnstructuredGrid, vtkUnstructuredGridToUnstructuredGridFilter );

  //! \brief Construct with all types of clipping turned off.
  static VTKViewer_ExtractUnstructuredGrid *New();

  enum EExtraction{ eCells, ePoints};
  //! Sets mode of extraction to \a theExtractionMode
  void SetModeOfExtraction(EExtraction theExtractionMode){
    myExtractionMode = theExtractionMode; Modified();
  }
  //! Get Extraction mode (Return: \a myExtractionMode field)
  EExtraction GetModeOfExtraction(){ return myExtractionMode;}

  enum EChanging{ ePassAll, eAdding, eRemoving};
  //! Sets mode of changing to \a theChangeMode
  void SetModeOfChanging(EChanging theChangeMode){
    myChangeMode = theChangeMode; 
    Modified();
  }
  //! Return \a myChangeMode field
  EChanging GetModeOfChanging(){ return myChangeMode;}

  //! Add cell id to \a myCellIds std::set
  void RegisterCell(vtkIdType theCellId);
  //! Check if myCellIds is empty.
  int IsCellsRegistered() { return !myCellIds.empty();}
  //! Remove the cell from the output
  void ClearRegisteredCells() { 
    myCellIds.clear();
    Modified();
  }
  
  //! Add cell type to \a myCellTypes std::set
  void RegisterCellsWithType(vtkIdType theCellType);
  //! Check if myCellTypes is empty.
  int IsCellsWithTypeRegistered() { return !myCellTypes.empty();}
  //! Remove every cells with the type from the output
  void ClearRegisteredCellsWithType() { 
    myCellTypes.clear();
    Modified();
  }

  //! \brief Do the filter do some real work
  int IsChanging() { return IsCellsRegistered() || IsCellsWithTypeRegistered();}

  //! \brief Do it keep the mapping between input's and output's UnstructuredGrid
  void SetStoreMapping(int theStoreMapping);
  //! Get \a myStoreMapping
  int GetStoreMapping(){ return myStoreMapping;}

  //! Gets the input id by output id.
  vtkIdType GetInputId(int theOutId) const;
  //! Gets the output id by input id.
  vtkIdType GetOutputId(int theInId) const;

  typedef std::vector<vtkIdType> TVectorId;
  typedef std::map<vtkIdType,vtkIdType> TMapId;

protected:
  VTKViewer_ExtractUnstructuredGrid();
  ~VTKViewer_ExtractUnstructuredGrid();

  //! Main method, which calculate output
  void Execute();

  EExtraction myExtractionMode;
  
  EChanging myChangeMode;
  typedef std::set<vtkIdType> TSetId;
  TSetId myCellIds;
  TSetId myCellTypes;

  bool myStoreMapping;
  TVectorId myOut2InId;
  TMapId myIn2OutId;

private:
  //! Not implemented.
  VTKViewer_ExtractUnstructuredGrid(const VTKViewer_ExtractUnstructuredGrid&);
  //! Not implemented.
  void operator=(const VTKViewer_ExtractUnstructuredGrid&);
};

#endif
