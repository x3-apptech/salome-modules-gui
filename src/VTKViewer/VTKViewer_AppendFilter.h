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
  GetPointOutputID(vtkIdType theInputID,
		   vtkIdType theInputDataSetID);

  vtkIdType
  GetCellOutputID(vtkIdType theInputID,
		  vtkIdType theInputDataSetID);

  void 
  GetPointInputID(vtkIdType theOutputID,
		 vtkIdType& theInputID,
		 vtkIdType& theStartID,
		 vtkIdType& theInputDataSetID);

  void
  GetCellInputID(vtkIdType theOutputID,
		 vtkIdType& theInputID,
		 vtkIdType& theStartID,
		 vtkIdType& theInputDataSetID);

  typedef std::vector<vtkIdType> TVectorIds;

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

  bool myDoMappingFlag;
  TVectorIds myNodeRanges;
  TVectorIds myCellRanges;
  vtkSmartPointer<vtkPoints> myPoints;
};

#endif
