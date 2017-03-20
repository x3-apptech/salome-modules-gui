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

// File   : SALOME_PYQT_Selector.cxx
// Author :
//
#include "SALOME_PYQT_Selector.h"

#include "LightApp_DataOwner.h"
#include "LightApp_DataObject.h"
#include "LightApp_Application.h"
#include "SALOME_PYQT_ModuleLight.h"
#include "SALOME_PYQT_BorrowedDataObjectLight.h"
#include <SUIT_Session.h>
#include <SUIT_DataObjectIterator.h>
#include <QTime>
#include <time.h>

#include <utilities.h>

/*!
 \class SALOME_PYQT_Selector
 \brief Object browser selection handler class.
 */

/*!
 \brief Constructor.
 \param pymod Python module interface instance in charge of calling Python module methods from C++
 \param mgr selection manager
 */
SALOME_PYQT_Selector::SALOME_PYQT_Selector(SALOME_PYQT_ModuleLight* pymod, SUIT_SelectionMgr* mgr) :
    SUIT_Selector(mgr, pymod), myPyModule(pymod)
{
  MESSAGE("constructor");
  myLocalEntries.clear();
  mySelectedList.clear();
  myEntries.clear();
  myModifiedTime = 0;
  if (myPyModule)
    {
      connect(myPyModule, SIGNAL(localSelectionChanged()), this, SLOT(onSelectionChanged()));
    }
  setModified();
}

/*!
 \brief Destructor.
 */
SALOME_PYQT_Selector::~SALOME_PYQT_Selector()
{
  MESSAGE("destructor");
}

/*!
 \brief Reset selection internally. Used on close study.
 */
void SALOME_PYQT_Selector::clear()
{
  MESSAGE("clear");
  myLocalEntries.clear();
  mySelectedList.clear();
  myEntries.clear();
}

/*!
 \brief Get Python module interface instance.
 \return a pointer to the Python module interface instance.
 */
SALOME_PYQT_ModuleLight* SALOME_PYQT_Selector::pyModule() const
{
  MESSAGE("pyModule");
  return myPyModule;
}

/*!
 \brief Get selector type.
 \return selector type
 */
QString SALOME_PYQT_Selector::type() const
{
  MESSAGE("type");
  return "PYQT_Module";
}

/*!
 \brief Get the time of the last selection changing.
 \return latest selection changing time
 */
unsigned long SALOME_PYQT_Selector::getModifiedTime() const
{
  MESSAGE("getModifiedTime");
  return myModifiedTime;
}

/*!
 \brief Update the time of the latest selection changing.
 */
void SALOME_PYQT_Selector::setModified()
{
  MESSAGE("setModified");
  myModifiedTime = clock();
}

/*!
 \brief Called when the Object browser selection is changed.
 */
void SALOME_PYQT_Selector::onSelectionChanged()
{
  MESSAGE("onSelectionChanged");
  QTime t1 = QTime::currentTime();
  mySelectedList.clear();
  selectionChanged();
  QTime t2 = QTime::currentTime();
  qDebug( QString( "selection time = %1 msecs" ).arg( t1.msecsTo( t2 ) ).toLatin1().constData());
}

/*!
 \brief Get list of currently selected objects.
 \param theList list to be filled with the selected objects owners
 */
void SALOME_PYQT_Selector::getSelection(SUIT_DataOwnerPtrList& theList) const
{
  MESSAGE("getSelection");
  if (mySelectedList.count() == 0)
    {
      MESSAGE("mySelectedList.count(): " << mySelectedList.count() << " myLocalEntries.size(): "<< myLocalEntries.size());
      SALOME_PYQT_Selector* that = (SALOME_PYQT_Selector*) this; // because of const...
      for (int i = 0; i < myLocalEntries.size(); i++)
        {
          SALOME_PYQT_BorrowedDataObjectLight *obj = new SALOME_PYQT_BorrowedDataObjectLight(myLocalEntries[i]);
#ifndef DISABLE_SALOMEOBJECT
          Handle(SALOME_InteractiveObject)aSObj = new SALOME_InteractiveObject
          ( obj->entry().toLatin1().constData(),
              obj->componentDataType().toLatin1().constData(),
              obj->name().toLatin1().constData() );
          LightApp_DataOwner* owner = new LightApp_DataOwner(aSObj);
#else
          LightApp_DataOwner* owner = new LightApp_DataOwner( obj->entry() );
#endif
          that->mySelectedList.append(SUIT_DataOwnerPtr(owner));
        }
    }
  theList = mySelectedList;
}

/*!
 \brief Set selection.
 \param theList list of the object owners to be set selected
 */
void SALOME_PYQT_Selector::setSelection(const SUIT_DataOwnerPtrList& theList)
{
  MESSAGE("setSelection");
  if (!myPyModule)
    return;

  myEntries.clear();
  for (SUIT_DataOwnerPtrList::const_iterator it = theList.begin(); it != theList.end(); ++it)
    {
      const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>((*it).operator->());
      if (owner)
        {
          QString entry = owner->entry();
          myEntries.append(entry);
          MESSAGE("Selected: "<< entry.toStdString());
        }
    }
  fillEntries(myEntries);
  mySelectedList.clear();
}

/*!
 \brief Fill map of the data objects currently shown in the Object Browser.
 \param entries map to be filled
 */
void SALOME_PYQT_Selector::fillEntries(QStringList& entries)
{
  MESSAGE("fillEntries");
  if (!myPyModule)
    return;
  myPyModule->setSelected(entries);
  setModified();
}

