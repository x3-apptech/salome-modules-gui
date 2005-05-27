#include "SalomeApp_Filter.h"

SalomeApp_Filter::SalomeApp_Filter( SalomeApp_Study* study )
{
  myStudy = study;
}

SalomeApp_Filter::~SalomeApp_Filter()
{
}

SalomeApp_Study* SalomeApp_Filter::getStudy() const
{
  return myStudy;
}
