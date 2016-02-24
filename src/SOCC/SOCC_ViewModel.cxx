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

#include "SOCC_ViewModel.h"

#include "SOCC_Prs.h"
#include "SOCC_ViewWindow.h"

#include "SUIT_Session.h"
#include "SUIT_ResourceMgr.h"
//#include "SUIT_Application.h"

//#include "ToolsGUI.h"

// Temporarily commented to avoid awful dependecy on SALOMEDS
// TODO: better mechanism of storing display/erse status in a study
// should be provided...
//#include <SALOMEconfig.h>
//#include CORBA_CLIENT_HEADER(SALOMEDS)

#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <SALOME_AISShape.hxx>
#include <SALOME_AISObject.hxx>
#include <SALOME_InteractiveObject.hxx>
#include <SALOME_ListIO.hxx>

// Temporarily commented to avoid awful dependecy on SALOMEDS
// TODO: better mechanism of storing display/erse status in a study
// should be provided...
//#include <Utils_ORB_INIT.hxx>
//#include <Utils_SINGLETON.hxx>
//#include <SALOME_ModuleCatalog_impl.hxx>
//#include <SALOME_NamingService.hxx>

//#include "SALOMEDSClient.hxx"
//#include "SALOMEDS_StudyManager.hxx"

#include <AIS_TypeOfIso.hxx>
#include <Precision.hxx>

#include <algorithm>

// in order NOT TO link with SalomeApp, here the code returns SALOMEDS_Study.
// SalomeApp_Study::studyDS() does it as well, but -- here it is retrieved from 
// SALOMEDS::StudyManager - no linkage with SalomeApp. 

// Temporarily commented to avoid awful dependecy on SALOMEDS
// TODO: better mechanism of storing display/erse status in a study
// should be provided...
//static _PTR(Study) getStudyDS() 
//{
//  SALOMEDSClient_Study* aStudy = NULL;
//  _PTR(StudyManager) aMgr( new SALOMEDS_StudyManager() );

  // get id of SUIT_Study, if it's a SalomeApp_Study, it will return
  //    id of its underlying SALOMEDS::Study
//  SUIT_Application* app = SUIT_Session::session()->activeApplication();
//  if ( !app )  return _PTR(Study)(aStudy);
//  SUIT_Study* stud = app->activeStudy();
//  if ( !stud ) return _PTR(Study)(aStudy);  
//  const int id = stud->id(); // virtual method, must return SALOMEDS_Study id
  // get SALOMEDS_Study with this id from StudyMgr
//  return aMgr->GetStudyByID( id );
//}

/*!
  Constructor
  \param DisplayTrihedron - is trihedron displayed
*/
SOCC_Viewer::SOCC_Viewer( bool DisplayTrihedron )
: OCCViewer_Viewer( DisplayTrihedron )
{
}

/*!
  Destructor
*/
SOCC_Viewer::~SOCC_Viewer()
{
}

/*!
  Hilights/unhilights object in viewer
  \param obj - object to be updated
  \param hilight - if it is true, object will be hilighted, otherwise it will be unhilighted
  \param update - update current viewer
*/
bool SOCC_Viewer::highlight( const Handle(SALOME_InteractiveObject)& obj,
                             bool hilight, bool upd )
{
  bool isInLocal = getAISContext()->HasOpenedContext();
  //SUIT_Study* ActiveStudy = SUIT_Application::getDesktop()->getActiveStudy();
  //SALOME_Selection* Sel = SALOME_Selection::Selection( ActiveStudy->getSelection() );

  AIS_ListOfInteractive List;
  getAISContext()->DisplayedObjects(List);
  
  AIS_ListIteratorOfListOfInteractive ite(List);
  for ( ; ite.More(); ite.Next() )
  {
    Handle(SALOME_InteractiveObject) anObj =
      Handle(SALOME_InteractiveObject)::DownCast( ite.Value()->GetOwner() );

    if ( !anObj.IsNull() && anObj->hasEntry() && anObj->isSame( obj ) )
    {
      if ( !isInLocal )
          OCCViewer_Viewer::highlight( ite.Value(), hilight, false );
      // highlight sub-shapes only when local selection is active
      else
      {
        /*if ( ite.Value()->IsKind( STANDARD_TYPE( SALOME_AISShape ) ) )
        {
          Handle(SALOME_AISShape) aSh = Handle(SALOME_AISShape)::DownCast( ite.Value() );
          TColStd_IndexedMapOfInteger MapIndex;
          Sel->GetIndex( IObject, MapIndex );
          aSh->highlightSubShapes( MapIndex, highlight );
        }*/
      }
      break;
    }
  }
    
  if( upd )
    update();
    
  return false;
}

/*!
  \return true if object is in viewer or in collector
  \param obj - object to be checked
  \param onlyInViewer - search object only in viewer (so object must be displayed)
*/
bool SOCC_Viewer::isInViewer( const Handle(SALOME_InteractiveObject)& obj,
                              bool onlyInViewer )
{
  AIS_ListOfInteractive List;
  getAISContext()->DisplayedObjects(List);
  AIS_ListIteratorOfListOfInteractive ite(List);
  for ( ; ite.More(); ite.Next() )
  {
    Handle(SALOME_InteractiveObject) anObj =
        Handle(SALOME_InteractiveObject)::DownCast( ite.Value()->GetOwner() );

    if ( !anObj.IsNull() && anObj->hasEntry() && anObj->isSame( obj ) )
      return true;
  }
  return false;
}

/*!
  \return true if object is displayed in viewer
  \param obj - object to be checked
*/
bool SOCC_Viewer::isVisible( const Handle(SALOME_InteractiveObject)& obj )
{

  std::map< std::string , std::vector<Handle(AIS_InteractiveObject)> >::iterator it=entry2aisobjects.find(obj->getEntry());
  if(it != entry2aisobjects.end())
    {
      // get context
      Handle (AIS_InteractiveContext) ic = getAISContext();
      std::vector<Handle(AIS_InteractiveObject)>& List = it->second;
      for( unsigned int ind = 0; ind < List.size(); ind++ )
        {
          Handle(AIS_InteractiveObject) anAIS=List[ind];
          if(ic->IsDisplayed(anAIS))
            return true;
        }
  }
  
  return false;
}

/*!
  Sets color of object
  \param obj - object to be updated
  \param color - new color
  \param update - update current viewer
*/
void SOCC_Viewer::setColor( const Handle(SALOME_InteractiveObject)& obj,
                            const QColor& color, bool update )
{
  if(obj.IsNull() || !obj->hasEntry() )
    return;

  if(entry2aisobjects.count(obj->getEntry())>0)
    {
      // get context
      Handle (AIS_InteractiveContext) ic = getAISContext();
      std::vector<Handle(AIS_InteractiveObject)>& List = entry2aisobjects[obj->getEntry()];
      for( unsigned int ind = 0; ind < List.size(); ind++ )
        {
          Handle(AIS_InteractiveObject) anAIS=List[ind];
          if( !anAIS.IsNull() && ic->IsDisplayed(anAIS))
            OCCViewer_Viewer::setColor( anAIS, color, update );
        }
    }
}

/*!
  Changes display mode of object
  \param obj - object to be processed
  \param mode - new display mode
  \param update - update current viewer
*/
void SOCC_Viewer::switchRepresentation( const Handle(SALOME_InteractiveObject)& obj,
                                        int mode, bool update )
{
  if(obj.IsNull() || !obj->hasEntry() )
    return;

  if(entry2aisobjects.count(obj->getEntry())>0)
    {
      // get context
      Handle (AIS_InteractiveContext) ic = getAISContext();
      std::vector<Handle(AIS_InteractiveObject)>& List = entry2aisobjects[obj->getEntry()];
      for( unsigned int ind = 0; ind < List.size(); ind++ )
        {
          Handle(AIS_InteractiveObject) anAIS=List[ind];
          if( !anAIS.IsNull() && ic->IsDisplayed(anAIS))
            OCCViewer_Viewer::switchRepresentation( anAIS, mode, update );
        }
    }
}

/*!
  Changes transparency of object
  \param obj - object to be processed
  \param trans - new transparency
  \param update - update current viewer
*/
void SOCC_Viewer::setTransparency( const Handle(SALOME_InteractiveObject)& obj,
                                   float trans, bool update )
{
  if(obj.IsNull() || !obj->hasEntry() )
    return;

  if(entry2aisobjects.count(obj->getEntry())>0)
    {
      // get context
      Handle (AIS_InteractiveContext) ic = getAISContext();
      std::vector<Handle(AIS_InteractiveObject)>& List = entry2aisobjects[obj->getEntry()];
      for( unsigned int ind = 0; ind < List.size(); ind++ )
        {
          Handle(AIS_InteractiveObject) anAIS=List[ind];
          if( !anAIS.IsNull() && ic->IsDisplayed(anAIS))
            OCCViewer_Viewer::setTransparency( anAIS, trans, update );
        }
    }
}

/*!
  Changes name of object
  \param obj - object to be renamed
  \param name - new name
*/
void SOCC_Viewer::rename( const Handle(SALOME_InteractiveObject)& obj,
                          const QString& name )
{
  AIS_ListOfInteractive List;
  getAISContext()->DisplayedObjects(List);
  
  AIS_ListIteratorOfListOfInteractive ite(List);
  while (ite.More())
  {
    if (ite.Value()->IsKind(STANDARD_TYPE(SALOME_AISShape)))
    {
      Handle(SALOME_AISShape) aSh = Handle(SALOME_AISShape)::DownCast(ite.Value());
      
      if ( aSh->hasIO() )
      {
        Handle(SALOME_InteractiveObject) IO = aSh->getIO();
        if ( IO->isSame( obj ) )
        {
          aSh->setName( name.toLatin1().data() );
          break;
        }
      }
    }
    else if ( ite.Value()->IsKind( STANDARD_TYPE( SALOME_AISObject ) ) )
    {
      Handle(SALOME_AISObject) aSh = Handle(SALOME_AISObject)::DownCast( ite.Value() );

      // Add code here, if someone create a MODULE_AISObject.
    }
    ite.Next();
  }
}


/*!
  Display presentation
  \param prs - presentation
*/
void SOCC_Viewer::Display( const SALOME_OCCPrs* prs )
{
  // try do downcast object
  const SOCC_Prs* anOCCPrs = dynamic_cast<const SOCC_Prs*>( prs );
  if ( !anOCCPrs || anOCCPrs->IsNull() )
    return;

  // get SALOMEDS Study
  // Temporarily commented to avoid awful dependecy on SALOMEDS
  // TODO: better mechanism of storing display/erse status in a study
  // should be provided...
  //  _PTR(Study) study(getStudyDS());

  // get context
  Handle (AIS_InteractiveContext) ic = getAISContext();

  // get objects to be displayed
  AIS_ListOfInteractive anAISObjects;
  anOCCPrs->GetObjects( anAISObjects );

  AIS_ListIteratorOfListOfInteractive aIter( anAISObjects );
  for ( ; aIter.More(); aIter.Next() )
  {
    Handle(AIS_InteractiveObject) anAIS = aIter.Value();
    if ( !anAIS.IsNull() )
    {
      // try to find presentation in the viewer

      // if the object is already displayed - nothing to do more
      if(ic->IsDisplayed(anAIS))
        {
          // Deactivate object if necessary
          if ( !anOCCPrs->ToActivate() )
            ic->Deactivate( anAIS );
          continue;
        }

      // if object is not displayed and not found in the collector - display it
      if ( anAIS->IsKind( STANDARD_TYPE(AIS_Trihedron) ) )
      {
        Handle(AIS_Trihedron) aTrh = Handle(AIS_Trihedron)::DownCast( anAIS );
        double aNewSize = 100, aSize = 100;
        computeTrihedronSize( aNewSize, aSize );
        aTrh->SetSize( aTrh == getTrihedron() ? aNewSize : 0.5 * aNewSize );
      }

      ic->Display( anAIS, false );
      Handle(SALOME_AISShape) aSh = Handle(SALOME_AISShape)::DownCast (anAIS);
      if (!aSh.IsNull())
      {
        aSh->SetClippable (prs->IsClippable());
        applyExistingClipPlanesToObject (anAIS);
        bool top = (aSh->isTopLevel() && aSh->switchTopLevel());
	      ic->SetZLayer( aSh, top ? getTopLayerId() : 0 );
		    if(!aSh->toActivate())
        {
			    ic->Deactivate( aSh );
		    }
      }

      //Register anAIS (if it has an entry) in entry2aisobjects map
      Handle(SALOME_InteractiveObject) anObj = Handle(SALOME_InteractiveObject)::DownCast( anAIS->GetOwner() );
      if ( !anObj.IsNull() && anObj->hasEntry())
        {
          std::vector<Handle(AIS_InteractiveObject)>& List = entry2aisobjects[anObj->getEntry()];
          int found=0;
          for ( unsigned int ind = 0; ind < List.size(); ind++ )
          {
            if(List[ind] == anAIS)
              {
                found=1;
                break;
              }
          }
          if(!found)
            {
              List.push_back(anAIS);
            }
        }

      // Set visibility flag
      // Temporarily commented to avoid awful dependecy on SALOMEDS
      // TODO: better mechanism of storing display/erse status in a study
      // should be provided...
      //Handle(SALOME_InteractiveObject) anObj =
      //  Handle(SALOME_InteractiveObject)::DownCast( anAIS->GetOwner() );
      //if ( !anObj.IsNull() && anObj->hasEntry() )
      //{
      //  if ( study  )
      //    ToolsGUI::SetVisibility( study, anObj->getEntry(), true, this );
      //}

      // Deactivate object if necessary
      if ( !anOCCPrs->ToActivate() )
        ic->Deactivate( anAIS );
    }
  }
  updateTrihedron();
}


/*!
  Erase presentation
  \param prs - presentation
  \param forced - removes object from context
*/
void SOCC_Viewer::Erase( const SALOME_OCCPrs* prs, const bool forced )
{
  // try do downcast object
  const SOCC_Prs* anOCCPrs = dynamic_cast<const SOCC_Prs*>( prs );
  if ( !anOCCPrs || anOCCPrs->IsNull() )
    return;

  // get SALOMEDS Study
  // Temporarily commented to avoid awful dependecy on SALOMEDS
  // TODO: better mechanism of storing display/erse status in a study
  // should be provided...
  //  _PTR(Study) study(getStudyDS());

  // get context
  Handle(AIS_InteractiveContext) ic = getAISContext();

  // get objects to be erased
  AIS_ListOfInteractive anAISObjects;
  anOCCPrs->GetObjects( anAISObjects );

  AIS_ListIteratorOfListOfInteractive aIter( anAISObjects );
  for ( ; aIter.More(); aIter.Next() ) {
    Handle(AIS_InteractiveObject) anAIS = aIter.Value();
    if ( !anAIS.IsNull() ) {
      // erase the object from context : move it to collector
      ic->Erase( anAIS, false );
      // Set visibility flag if necessary
      // Temporarily commented to avoid awful dependecy on SALOMEDS
      // TODO: better mechanism of storing display/erse status in a study
      // should be provided...
      //if ( !forced )
      //{
      //  Handle(SALOME_InteractiveObject) anObj =
      //    Handle(SALOME_InteractiveObject)::DownCast( anAIS->GetOwner() );
      //  if ( !anObj.IsNull() && anObj->hasEntry() )
      //  {
      //  if ( study )
      //    ToolsGUI::SetVisibility( study, anObj->getEntry(), true, this );
      //  }
      //}
    }
  }
  updateTrihedron();
}


/*!
  Erase all presentations
  \param forced - removes all objects from context
*/
void SOCC_Viewer::EraseAll( SALOME_Displayer* d, const bool forced )
{
  // get SALOMEDS Study
  // Temporarily commented to avoid awful dependecy on SALOMEDS
  // TODO: better mechanism of storing display/erse status in a study
  // should be provided...
  //  _PTR(Study) study(getStudyDS());

  // get context
  Handle(AIS_InteractiveContext) ic = getAISContext();

  // check if trihedron is displayed
  Standard_Boolean isTrihedronDisplayed = ic->IsDisplayed( getTrihedron() );

  // get objects to be erased (all currently displayed objects)
  AIS_ListOfInteractive aList;
  ic->DisplayedObjects( aList );
  AIS_ListIteratorOfListOfInteractive anIter( aList );
  for ( ; anIter.More(); anIter.Next() ) {
    if ( (isTrihedronDisplayed && anIter.Value()->DynamicType() == STANDARD_TYPE( AIS_Trihedron ) ) )
      continue;

    // erase an object
    Handle(AIS_InteractiveObject) anIO = anIter.Value();
    ic->Erase( anIO, false );
    
    // Set visibility flag if necessary
    // Temporarily commented to avoid awful dependecy on SALOMEDS
    // TODO: better mechanism of storing display/erse status in a study
    // should be provided...
    //if ( !forced ) {
    //  Handle(SALOME_InteractiveObject) anObj =
    //  Handle(SALOME_InteractiveObject)::DownCast( anIO->GetOwner() );

    //  if ( !anObj.IsNull() && anObj->hasEntry() ) {
    //  if ( study )
    //    ToolsGUI::SetVisibility( study, anObj->getEntry(), true, this );
    //  }
    //}
  }

  SALOME_View::EraseAll( d, forced );

  Repaint();
  updateTrihedron();
}

/*!
  Create presentation corresponding to the entry
  \param entry - entry
*/
SALOME_Prs* SOCC_Viewer::CreatePrs( const char* entry )
{
  SOCC_Prs* prs = new SOCC_Prs(entry);
  if ( entry )
  {
    if(entry2aisobjects.count(entry)>0)
      {
        //ais object exists
        std::vector<Handle(AIS_InteractiveObject)> List = entry2aisobjects[entry];
        // get context
        Handle(AIS_InteractiveContext) ic = getAISContext();
        //add all ais
        for ( unsigned int ind = 0; ind < List.size(); ind++ )
          {
            Handle(AIS_InteractiveObject) anAIS=List[ind];
            if(ic->IsDisplayed(anAIS))
              {
                prs->AddObject( anAIS );
              }
          }
      }
  }
  return prs;
}

/*!
  Activates selection of sub-shapes
*/
void SOCC_Viewer::LocalSelection( const SALOME_OCCPrs* thePrs, const std::list<int> modes )
{
  Handle(AIS_InteractiveContext) ic = getAISContext();
  
  const SOCC_Prs* anOCCPrs = dynamic_cast<const SOCC_Prs*>( thePrs );
  if ( ic.IsNull() )
    return;
  
  // Open local context if there is no one
  bool allObjects = thePrs == 0 || thePrs->IsNull();
  if ( !ic->HasOpenedContext() ) {
    ic->ClearCurrents( false );
    ic->OpenLocalContext( Standard_False, Standard_True, Standard_True );
  }

  AIS_ListOfInteractive anObjs;
  // Get objects to be activated
  if ( allObjects ) 
    ic->DisplayedObjects( anObjs );
  else
    anOCCPrs->GetObjects( anObjs );

  std::list<int> sel_modes;
  for ( int i = TopAbs_COMPOUND; i < TopAbs_SHAPE; i++ )
    if ( std::find(modes.begin(), modes.end(), (int)TopAbs_SHAPE) != modes.end() || std::find(modes.begin(), modes.end(), i) != modes.end())
      sel_modes.push_back(i);

  // Activate selection of objects from prs
  AIS_ListIteratorOfListOfInteractive aIter( anObjs );
  for ( ; aIter.More(); aIter.Next() ) {
    Handle(AIS_InteractiveObject) anAIS = aIter.Value();
    if ( !anAIS.IsNull() )
    {
      std::list<int>::const_iterator it;
      if ( anAIS->IsKind( STANDARD_TYPE( AIS_Shape ) ) )
      {
        ic->Load( anAIS, -1, false );
        for( it = sel_modes.begin(); it != sel_modes.end(); ++it )
          ic->Activate( anAIS, AIS_Shape::SelectionMode( (TopAbs_ShapeEnum)*it ) );
      }
      else if ( anAIS->DynamicType() != STANDARD_TYPE(AIS_Trihedron) )
      {
        ic->Load( anAIS, -1, false );
        for( it = sel_modes.begin(); it != sel_modes.end(); ++it )
          ic->Activate( anAIS, *it );
      }
    }
  }
}

/*!
  Activates selection of sub-shapes
*/
void SOCC_Viewer::LocalSelection( const SALOME_OCCPrs* thePrs, const int theMode )
{
  std::list<int> modes;
  modes.push_back( theMode );
  LocalSelection( thePrs, modes );
}

/*!
  Deactivates selection of sub-shapes
*/
void SOCC_Viewer::GlobalSelection( const bool update ) const
{
  Handle(AIS_InteractiveContext) ic = getAISContext();
  if ( !ic.IsNull() )
  {
    ic->CloseAllContexts( false );
    if ( update )
      ic->CurrentViewer()->Redraw();
  }
}


/*!
  \Collect objects visible in viewer
  \param theList - visible objects collection
*/
void SOCC_Viewer::GetVisible( SALOME_ListIO& theList )
{
  AIS_ListOfInteractive List;
  getAISContext()->DisplayedObjects(List);
  
  AIS_ListIteratorOfListOfInteractive ite(List);
  for ( ; ite.More(); ite.Next() )
  {
    Handle(SALOME_InteractiveObject) anObj =
        Handle(SALOME_InteractiveObject)::DownCast( ite.Value()->GetOwner() );

    if ( !anObj.IsNull() && anObj->hasEntry() )
      theList.Append( anObj );
  }
}

/*!
  Updates current viewer
*/
void SOCC_Viewer::Repaint()
{
//  onAdjustTrihedron();
  getViewer3d()->Update();
}


/*!
  create SOCC_ViewWindow
*/
/*SUIT_ViewWindow* SOCC_Viewer::createView( SUIT_Desktop* theDesktop )
{
  SOCC_ViewWindow* view = new SOCC_ViewWindow(theDesktop, this);
  //initView( view );
  initView( view->getView(OCCViewer_ViewFrame::MAIN_VIEW) );
  return view;
  }*/

/* 
 * Returns a new OCCViewer_ViewWindow instance which will be placed as a sub window in ViewFrame
 */
OCCViewer_ViewWindow* SOCC_Viewer::createSubWindow()
{
  return new SOCC_ViewWindow( 0,  this);
}
