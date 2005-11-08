// SUIT_Convertor.h: interface for the SUIT_Convertor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUIT_CONVERTOR_H__4C27F4C7_DC7C_4BEF_9DC1_EFB97B387EBF__INCLUDED_)
#define AFX_SUIT_CONVERTOR_H__4C27F4C7_DC7C_4BEF_9DC1_EFB97B387EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SUIT.h"
#include "SUIT_DataObject.h"
#include "SUIT_ViewModel.h"

/*! Provides convertation of selected object of specified viewer into SUIT_DataObject.
 *  Instance of Convertor class is created by Application according to data type and viewers used.
 */
class SUIT_Convertor  
{
public:
  virtual DataObjectList getSelectedObjects(const SUIT_ViewModel* theViewer) = 0;
  virtual void highlight(const SUIT_ViewModel* theViewer, const DataObjectList& theObjList) = 0;
};

#endif // !defined(AFX_SUIT_CONVERTOR_H__4C27F4C7_DC7C_4BEF_9DC1_EFB97B387EBF__INCLUDED_)
