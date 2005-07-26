#include "SalomeApp_Filter.h"

/*!Constructor.Initialize by study.*/
SalomeApp_Filter::SalomeApp_Filter( SalomeApp_Study* study )
{
  myStudy = study;
}

/*!Destructor. Do nothing.*/
SalomeApp_Filter::~SalomeApp_Filter()
{
}

/*!Gets study.*/
SalomeApp_Study* SalomeApp_Filter::getStudy() const
{
  return myStudy;
}
