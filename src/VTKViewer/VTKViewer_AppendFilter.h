#ifndef VTKVIEWER_APPENDFILTER_H
#define VTKVIEWER_APPENDFILTER_H

#include "VTKViewer.h"

#include <vtkAppendFilter.h>
#include <vtkSmartPointer.h>

#include <vector>
#include <map>

class vtkPoints;

/*! \brief This class used same as vtkAppendFilter. See documentation on VTK for more information.
 */
class VTKVIEWER_EXPORT VTKViewer_AppendFilter : public vtkAppendFilter 
{
public:
  /*! \fn static VTKViewer_AppendFilter *New()
   */
  static VTKViewer_AppendFilter *New();
  
  /*! \fn vtkTypeRevisionMacro(VTKViewer_AppendFilter, vtkAppendFilter)
   *  \brief VTK type revision macros.
   */
  vtkTypeRevisionMacro(VTKViewer_AppendFilter, vtkAppendFilter);

  void SetDoMappingFlag(const bool theFlag);

  bool DoMappingFlag() const;

  void
  SetPoints(vtkPoints* thePoints);

  vtkPoints*
  GetPoints();

  vtkIdType
  GetPointOutputID(vtkIdType theInputID);

  vtkIdType
  GetCellOutputID(vtkIdType theInputID);

  vtkIdType 
  GetPointInputID(vtkIdType theOutputID, 
		  vtkIdType& theInputDataSetID);

  vtkIdType
  GetCellInputID(vtkIdType theOutputID, 
		 vtkIdType& theInputDataSetID);

protected:
  /*! \fn VTKViewer_AppendFilter();
   * \brief Constructor
   */
  VTKViewer_AppendFilter();
  /*! \fn ~VTKViewer_AppendFilter();
   * \brief Destructor.
   */
  ~VTKViewer_AppendFilter();
  /*! \fn void Execute();
   * \brief Filter culculation method.
   */
  virtual void Execute();
  //
  void DoMapping();

  void Reset();

  void MakeOutput();

  //
  vtkSmartPointer<vtkPoints> myPoints;

private:
  typedef std::vector<vtkIdType> TVectorId;
  typedef std::vector<int> VectorInt;
  typedef std::map <int,int>                  DataMapOfIntegerInteger;
  typedef DataMapOfIntegerInteger::iterator   IteratorOfDataMapOfIntegerInteger;
  typedef DataMapOfIntegerInteger::value_type PairOfDataMapOfIntegerInteger;
private:
  bool      myDoMappingFlag;
  TVectorId myNodeIds;
  TVectorId myCellIds;
  VectorInt myNodeRanges;
  VectorInt myCellRanges;
  DataMapOfIntegerInteger myNodeMapObjIDVtkID;
  DataMapOfIntegerInteger myCellMapObjIDVtkID;
};

#endif
