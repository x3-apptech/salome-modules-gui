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
