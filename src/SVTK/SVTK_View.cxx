//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$
//
#include "SALOME_Actor.h"

#include "SVTK_View.h"
#include "SVTK_Renderer.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_RenderWindowInteractor.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "VTKViewer_Algorithm.h"
#include "SVTK_Functor.h"

#include <vtkActorCollection.h>
#include <vtkRenderer.h>

/*!
  Constructor
*/
SVTK_SignalHandler
::SVTK_SignalHandler(SVTK_ViewWindow* theMainWindow):
  QObject(theMainWindow),
  myMainWindow(theMainWindow)
{
  SVTK_RenderWindowInteractor* anInteractor = theMainWindow->GetInteractor();

  connect(anInteractor,SIGNAL(KeyPressed(QKeyEvent*)),
	  this,SIGNAL(KeyPressed(QKeyEvent*)) );
  connect(anInteractor,SIGNAL(KeyReleased(QKeyEvent*)),
	  this,SIGNAL(KeyReleased(QKeyEvent*)));
  connect(anInteractor,SIGNAL(MouseButtonPressed(QMouseEvent*)),
	  this,SIGNAL(MouseButtonPressed(QMouseEvent*)));
  connect(anInteractor,SIGNAL(MouseButtonReleased(QMouseEvent*)),
	  this,SIGNAL(MouseButtonReleased(QMouseEvent*)));
  connect(anInteractor,SIGNAL(MouseDoubleClicked(QMouseEvent*)),
	  this,SIGNAL(MouseDoubleClicked(QMouseEvent*)));
  connect(anInteractor,SIGNAL(MouseMove(QMouseEvent*)),
	  this,SIGNAL(MouseMove(QMouseEvent*)));
  connect(anInteractor,SIGNAL(contextMenuRequested(QContextMenuEvent*)),
	  this,SIGNAL(contextMenuRequested(QContextMenuEvent*)));
  connect(anInteractor,SIGNAL(selectionChanged()),
	  this,SIGNAL(selectionChanged()));
}

/*!
  Destructor
*/
SVTK_SignalHandler
::~SVTK_SignalHandler()
{
}

/*!
  \return corresponding svtk main window
*/
SVTK_ViewWindow*
SVTK_SignalHandler
::GetMainWindow()
{
  return myMainWindow;
}


/*!
  Redirect the request to #SVTK_ViewWindow::Repaint (just for flexibility)
*/
void
SVTK_SignalHandler
::Repaint(bool theUpdateTrihedron)
{
  myMainWindow->Repaint(theUpdateTrihedron);
}

/*!
  Redirect the request to #SVTK_ViewWindow::GetRenderer (just for flexibility)
*/
SVTK_Renderer* 
SVTK_SignalHandler
::GetRenderer()
{
  return myMainWindow->GetRenderer();
}

/*!
  Redirect the request to #SVTK_ViewWindow::getRenderer (just for flexibility)
*/
vtkRenderer* 
SVTK_SignalHandler
::getRenderer()
{
  return myMainWindow->getRenderer();
}

namespace SVTK
{
  struct THighlightAction
  {
    bool myIsHighlight;
    THighlightAction( bool theIsHighlight ):
      myIsHighlight( theIsHighlight )
    {}
    
    void
    operator()( SALOME_Actor* theActor) 
    {
      if(theActor->GetMapper() && theActor->hasIO()){
	theActor->Highlight( myIsHighlight );
      }
    }
  };
}

/*!
  SLOT: called on selection change
*/
void
SVTK_SignalHandler
::onSelectionChanged()
{
  vtkActorCollection* anActors = myMainWindow->getRenderer()->GetActors();

  using namespace SVTK;
  ForEach<SALOME_Actor>(anActors,
			THighlightAction( false ));
  SVTK_Selector* aSelector = myMainWindow->GetSelector();
  const SALOME_ListIO& aListIO = aSelector->StoredIObjects();
  SALOME_ListIteratorOfListIO anIter(aListIO);
  for(; anIter.More(); anIter.Next()){
    ForEachIf<SALOME_Actor>(anActors,
			    TIsSameIObject<SALOME_Actor>(anIter.Value()),
			    THighlightAction(true));
  }

  myMainWindow->Repaint(false);
}

/*!
  Constructor
*/
SVTK_View
::SVTK_View(SVTK_ViewWindow* theMainWindow) :
  SVTK_SignalHandler(theMainWindow)
{
}

/*!
  Destructor
*/
SVTK_View
::~SVTK_View()
{
}

/*!
  Unhilights all objects in viewer
*/
void 
SVTK_View
::unHighlightAll() 
{
  using namespace SVTK;
  ForEach<SALOME_Actor>(getRenderer()->GetActors(),
			THighlightAction( false ));
  Repaint();
}

/*!
  Hilights/unhilights object in viewer
  \param theIO - object to be updated
  \param theIsHighlight - if it is true, object will be hilighted, otherwise it will be unhilighted
  \param theIsUpdate - update current viewer
*/
void
SVTK_View
::highlight( const Handle(SALOME_InteractiveObject)& theIO, 
	     bool theIsHighlight, 
	     bool theIsUpdate ) 
{
  using namespace SVTK;
  ForEachIf<SALOME_Actor>(getRenderer()->GetActors(),
			  TIsSameIObject<SALOME_Actor>( theIO ),
			  THighlightAction(theIsHighlight));
  Repaint();
}

/*!
  Display object
  \param theEntry - entry that corresponds to intractive objects
*/
Handle(SALOME_InteractiveObject)
SVTK_View
::FindIObject(const char* theEntry) 
{
  using namespace SVTK;
  SALOME_Actor* anActor = 
    Find<SALOME_Actor>(getRenderer()->GetActors(),
		       TIsSameEntry<SALOME_Actor>(theEntry));
  if(anActor != NULL)
    return anActor->getIO();
 
  return NULL;
}

/*!
   Redirect the request to #SVTK_Renderer::SetPreselectionProp
*/
void
SVTK_View
::SetSelectionProp(const double& theRed, 
		   const double& theGreen, 
		   const double& theBlue, 
		   const int& theWidth) 
{
  GetRenderer()->SetSelectionProp(theRed,theGreen,theBlue,theWidth);
}

/*!
  Redirect the request to #SVTK_Renderer::SetPreselectionProp
*/
void
SVTK_View
::SetPreselectionProp(const double& theRed, 
		      const double& theGreen, 
		      const double& theBlue, 
		      const int& theWidth) 
{
  GetRenderer()->SetPreselectionProp(theRed,theGreen,theBlue,theWidth);
}

/*!
  Redirect the request to #SVTK_Renderer::SetPreselectionProp
*/
void
SVTK_View
::SetSelectionTolerance(const double& theTolNodes, 
			const double& theTolCell,
			const double& theTolObjects)
{
  GetRenderer()->SetSelectionTolerance(theTolNodes, theTolCell, theTolObjects);
}

/*!
  \return true if object is in viewer or in collector
  \param theIO - object to be checked
*/
bool
SVTK_View
::isInViewer(const Handle(SALOME_InteractiveObject)& theIObject)
{
  using namespace SVTK;
  SALOME_Actor* anActor = 
    Find<SALOME_Actor>(getRenderer()->GetActors(),
		       TIsSameIObject<SALOME_Actor>(theIObject));
  return anActor != NULL;
}

/*!
  \return true if object is displayed in viewer
  \param theIO - object to be checked
*/
bool
SVTK_View
::isVisible(const Handle(SALOME_InteractiveObject)& theIObject)
{
  using namespace SVTK;
  SALOME_Actor* anActor = 
    Find<SALOME_Actor>(getRenderer()->GetActors(),
		       TIsSameIObject<SALOME_Actor>(theIObject));
  return anActor != NULL && anActor->GetVisibility();
}

/*!
  Changes name of object
  \param theIObject - object to be renamed
  \param theName - new name
*/
void
SVTK_View
::rename(const Handle(SALOME_InteractiveObject)& theIObject, 
	 const QString& theName)
{
  using namespace SVTK;
  ForEachIf<SALOME_Actor>(getRenderer()->GetActors(),
			  TIsSameIObject<SALOME_Actor>(theIObject),
			  TSetFunction<SALOME_Actor,const char*,const char*>
			  (&SALOME_Actor::setName,theName.toLatin1().data()));
}

/*!
  \return current display mode (obsolete)
*/
int
SVTK_View
::GetDisplayMode() 
{
  return myDisplayMode; 
}

/*!
  Set current display mode
  \param theMode - new display mode
*/
void
SVTK_View
::SetDisplayMode(int theMode)
{
  if(theMode == 0) 
    ChangeRepresentationToWireframe();
  else 
    ChangeRepresentationToSurface();
  myDisplayMode = theMode;
}

/*!
  Set current display mode
  \param theIObject - object
  \param theMode - new display mode
*/
void
SVTK_View
::SetDisplayMode(const Handle(SALOME_InteractiveObject)& theIObject, 
		 int theMode)
{
  using namespace SVTK;
  ForEachIf<SALOME_Actor>(getRenderer()->GetActors(),
			  TIsSameIObject<SALOME_Actor>(theIObject),
			  TSetFunction<SALOME_Actor,int>
			  (&SALOME_Actor::setDisplayMode,theMode));
}

/*!
  Change all actors to wireframe
*/
void
SVTK_View
::ChangeRepresentationToWireframe()
{
  ChangeRepresentationToWireframe(getRenderer()->GetActors());
}

/*!
  Change all actors to shading
*/
void
SVTK_View
::ChangeRepresentationToSurface()
{
  ChangeRepresentationToSurface(getRenderer()->GetActors());
}

/*!
  Change to wireframe a list of vtkactor
  theCollection - list of vtkactor
*/
void
SVTK_View
::ChangeRepresentationToWireframe(vtkActorCollection* theCollection)
{
  using namespace SVTK;
  ForEach<SALOME_Actor>(theCollection,
			TSetFunction<SALOME_Actor,int>
			(&SALOME_Actor::setDisplayMode,0));
  Repaint();
}

/*!
  Change to shading a list of vtkactor
  theCollection - list of vtkactor
*/
void
SVTK_View
::ChangeRepresentationToSurface(vtkActorCollection* theCollection)
{
  using namespace SVTK;
  ForEach<SALOME_Actor>(theCollection,
			TSetFunction<SALOME_Actor,int>
			(&SALOME_Actor::setDisplayMode,1));
  Repaint();
}

namespace SVTK
{
  struct TErase
  {
    VTK::TSetFunction<vtkActor,int> mySetFunction;
    TErase():
      mySetFunction(&vtkActor::SetVisibility,false)
    {}
    void
    operator()(SALOME_Actor* theActor)
    {
      theActor->SetVisibility(false);
      // Erase dependent actors
      vtkActorCollection* aCollection = vtkActorCollection::New(); 
      theActor->GetChildActors(aCollection);
      VTK::ForEach<vtkActor>(aCollection,mySetFunction);
      aCollection->Delete();
    }
  };
}

/*!
  To erase all existing VTK presentations
*/
void
SVTK_View
::EraseAll()
{   
  using namespace SVTK;
  ForEach<SALOME_Actor>(getRenderer()->GetActors(),
			TErase());
  Repaint();
}

/*!
  To display all existing VTK presentations
*/
void
SVTK_View
::DisplayAll()
{ 
  using namespace SVTK;
  ForEach<SALOME_Actor>(getRenderer()->GetActors(),
			TSetVisibility<SALOME_Actor>(true));
  Repaint();
}

/*!
  To erase VTK presentation
  \param theActor - actor
  \param theIsUpdate - updates current viewer
*/
void
SVTK_View
::Erase(SALOME_Actor* theActor, 
	bool theIsUpdate)
{
  SVTK::TErase()(theActor);

  if(theIsUpdate)
    Repaint();
}


/*!
  To erase VTK presentation
  \param theIObject - object
  \param theIsUpdate - updates current viewer
*/
void
SVTK_View
::Erase(const Handle(SALOME_InteractiveObject)& theIObject, 
	bool theIsUpdate)
{
  using namespace SVTK;
  ForEachIf<SALOME_Actor>(getRenderer()->GetActors(),
			  TIsSameIObject<SALOME_Actor>(theIObject),
			  TErase());
  if(theIsUpdate)
    Repaint();
}

/*!
  To display the VTK presentation
*/
void
SVTK_View
::Display(SALOME_Actor* theActor, 
	  bool theIsUpdate)
{
  GetRenderer()->AddActor(theActor);
  theActor->SetVisibility(true);

  if(theIsUpdate)
    Repaint();
}

/*!
  To display the VTK presentation
*/
void
SVTK_View
::Display(const Handle(SALOME_InteractiveObject)& theIObject, 
	  bool theIsUpdate)
{
  using namespace SVTK;
  ForEachIf<SALOME_Actor>(getRenderer()->GetActors(),
			  TIsSameIObject<SALOME_Actor>(theIObject),
			  TSetVisibility<SALOME_Actor>(true));

  if(theIsUpdate)
    Repaint();
}

/*!
  To display VTK presentation with defined #SALOME_InteractiveObject and erase all others
*/
void
SVTK_View
::DisplayOnly(const Handle(SALOME_InteractiveObject)& theIObject)
{
  EraseAll();
  Display(theIObject);
}


namespace SVTK
{
  struct TRemoveAction
  {
    SVTK_Renderer* myRenderer;
    TRemoveAction(SVTK_Renderer* theRenderer): 
      myRenderer(theRenderer)
    {}
    void
    operator()(SALOME_Actor* theActor)
    {
      myRenderer->RemoveActor(theActor);
    }
  };
}

/*!
  To remove the VTK presentation
*/
void
SVTK_View
::Remove(const Handle(SALOME_InteractiveObject)& theIObject, 
	 bool theIsUpdate)
{
  using namespace SVTK;
  ForEachIf<SALOME_Actor>(getRenderer()->GetActors(),
			  TIsSameIObject<SALOME_Actor>(theIObject),
			  TRemoveAction(GetRenderer()));
  if(theIsUpdate)
    Repaint();
}

/*!
  To remove the VTK presentation
*/
void
SVTK_View
::Remove(SALOME_Actor* theActor, 
	 bool theIsUpdate)
{
  GetRenderer()->RemoveActor(theActor);
  if(theIsUpdate)
    Repaint();
}

/*!
  To remove all VTK presentations
*/
void
SVTK_View
::RemoveAll(bool theIsUpdate)
{
  vtkRenderer* aRenderer = getRenderer();
  if(vtkActorCollection* anActors = aRenderer->GetActors()){
    anActors->InitTraversal();
    while(vtkActor *anAct = anActors->GetNextActor()){
      if(SALOME_Actor* aSAct = SALOME_Actor::SafeDownCast(anAct)){
        if(aSAct->hasIO() && aSAct->getIO()->hasEntry())
          aRenderer->RemoveActor( anAct );
      }
    }

    if(theIsUpdate)
      Repaint();
  }
}

/*!
  \return current transparency
  \param theIObject - object
*/
float
SVTK_View
::GetTransparency(const Handle(SALOME_InteractiveObject)& theIObject) 
{
  using namespace SVTK;
  SALOME_Actor* anActor = 
    Find<SALOME_Actor>(getRenderer()->GetActors(),
		       TIsSameIObject<SALOME_Actor>(theIObject));
  if(anActor)
    return 1.0 - anActor->GetOpacity();
  return -1.0;
}


/*!
  Sets current transparency
  \param theIObject - object
  \param theTrans - new transparency
*/
void
SVTK_View
::SetTransparency(const Handle(SALOME_InteractiveObject)& theIObject, 
		  float theTrans)
{
  vtkFloatingPointType anOpacity = 1.0 - theTrans;
  using namespace SVTK;
  ForEachIf<SALOME_Actor>(getRenderer()->GetActors(),
			  TIsSameIObject<SALOME_Actor>(theIObject),
			  TSetFunction<SALOME_Actor,vtkFloatingPointType>
			  (&SALOME_Actor::SetOpacity,anOpacity));
}

/*!
  Change color
  \param theIObject - object
  \param theColor - new color
*/
void
SVTK_View
::SetColor(const Handle(SALOME_InteractiveObject)& theIObject,
	   const QColor& theColor) 
{
  vtkFloatingPointType aColor[3] = {theColor.red()/255., theColor.green()/255., theColor.blue()/255.};

  using namespace SVTK;
  ForEachIf<SALOME_Actor>(getRenderer()->GetActors(),
			  TIsSameIObject<SALOME_Actor>(theIObject),
			  TSetFunction<SALOME_Actor,const vtkFloatingPointType*>
			  (&SALOME_Actor::SetColor,aColor));
}


/*!
  \return object color
  \param theIObject - object
*/
QColor
SVTK_View
::GetColor(const Handle(SALOME_InteractiveObject)& theIObject) 
{
  using namespace SVTK;
  SALOME_Actor* anActor = 
    Find<SALOME_Actor>(getRenderer()->GetActors(),
		       TIsSameIObject<SALOME_Actor>(theIObject));
  if(anActor){
    vtkFloatingPointType r,g,b;
    anActor->GetColor(r,g,b);
    return QColor(int(r*255),int(g*255),int(b*255));
  }

  return QColor(0,0,0);
}

