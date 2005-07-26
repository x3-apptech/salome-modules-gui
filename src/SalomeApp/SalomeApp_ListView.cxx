//  SALOME SalomeApp
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : SalomeApp_ListView.cxx
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#include "SalomeApp_ListView.h"
#include "SalomeApp_Application.h"

#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"

#include <qheader.h>
#include <qvalidator.h>
#include <qapplication.h>
#include <qtoolbutton.h>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>

#include "utilities.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// SalomeApp_ListView class implementation
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : computeEditGeometry
/*! Purpose  : static function - used for resizing editing widget*/
//================================================================
void computeEditGeometry(SalomeApp_ListViewItem* theItem,
                         SalomeApp_EntityEdit*   theWidget)
{
  if (!theItem)
    return;
  QListView* aListView = theItem->listView();
  int anEditColumn = theItem->getEditedColumn();
  if (anEditColumn < 0)
    return;

  int aX = 0, aY = 0, aW = 0, aH = 0;

  QRect aRect = aListView->itemRect(theItem);
  aListView->contentsToViewport(aListView->header()->sectionPos(anEditColumn), 0, aX, aY);
  if (aX < 0)
    aX = 0; // THIS CAN BE REMOVED
  QSize aSize = theWidget->getControl()->sizeHint();
  aH = QMAX(aSize.height() , aRect.height() );
  aY = aRect.y() - ((aH - aRect.height()) / 2);
  //aW = aListView->columnWidth(anEditColumn); // CAN SUBSTITUTE NEXT 3 ROWS
  aW = aListView->viewport()->width() - aX;
  if (aW < 0)
    aW = 0;
  theWidget->setGeometry(aX, aY, aW, aH);
}

//================================================================
// Function : SalomeApp_ListView::SalomeApp_ListView
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListView::SalomeApp_ListView( QWidget* parent )
: QtxListView( parent )
{
  myMouseEnabled = true;
  myEditingEnabled = false;
  setSelectionMode(Single);
  setSorting(-1);
  setRootIsDecorated(false);
  setAllColumnsShowFocus(false);
//  header()->setClickEnabled(false);
  header()->setMovingEnabled(false);

  myEditedItem = 0;
  myEdit = 0;

  viewport()->installEventFilter(this);

  connect(this, SIGNAL(selectionChanged()),
	  this, SLOT(onSelectionChanged()));
  connect(header(), SIGNAL(sizeChange(int, int, int)),
	  this,     SLOT(onHeaderSizeChange(int, int, int)));
}

//================================================================
// Function : SalomeApp_ListView::~SalomeApp_ListView
/*! Purpose  : destructor*/
//================================================================
SalomeApp_ListView::~SalomeApp_ListView()
{
  if (myEdit) {
    delete myEdit;
  }
  myEdit = 0;
  myEditedItem = 0;
}

//================================================================
// Function : SalomeApp_ListView::updateViewer
/*! Purpose  : updates all data viewer*/
//================================================================
void SalomeApp_ListView::updateViewer()
{
  // temporary disconnecting selection changed SIGNAL
  blockSignals(true);
  SalomeApp_ListViewItem* aRoot = (SalomeApp_ListViewItem*)firstChild();
  if (aRoot)
    aRoot->updateAllLevels();
  updateContents();
  // connecting again selection changed SIGNAL
  blockSignals(false);
  emit selectionChanged();
}

//================================================================
// Function : SalomeApp_ListView::updateSelected
/*! Purpose  : updates currently selected item(s)*/
//================================================================
void SalomeApp_ListView::updateSelected()
{
  // temporary disconnecting selection changed SIGNAL
  blockSignals(true);
  SalomeApp_ListViewItem* aChild = (SalomeApp_ListViewItem*)selectedItem();
  if (aChild)
    aChild->updateAllLevels();
  updateContents();
  // connecting again selection changed SIGNAL
  blockSignals(false);
  emit selectionChanged();
}

//================================================================
// Function : SalomeApp_ListView::popupClientType
/*! Purpose  : returns popup client type*/
//================================================================
QString SalomeApp_ListView::popupClientType() const
{
  return "SalomeApp_ListView";
}

//================================================================
// Function : SalomeApp_ListView::contextMenuPopup
/*! Purpose  : fills popup menu with items*/
//================================================================
void SalomeApp_ListView::contextMenuPopup( QPopupMenu* aPopup )
{
  if (aPopup) {
    // add items here...
  }
}

//================================================================
// Function : SalomeApp_ListView::clear
/*! Purpose  : clears view*/
//================================================================
void SalomeApp_ListView::clear()
{
  if (myEdit) {
    delete myEdit;
    myEdit = 0;
    myEditedItem = 0;
  }
  QListView::clear();
}

//================================================================
// Function : SalomeApp_ListView::isMouseEnabled
/*! Purpose  : returms true if mouse events are enabled*/
//================================================================
bool SalomeApp_ListView::isMouseEnabled()
{
  return myMouseEnabled;
}

//================================================================
// Function : SalomeApp_ListView::enableMouse
// Purpose  : enabled/disables mouse events (excluding MouseMove)
//================================================================
void SalomeApp_ListView::enableMouse(bool enable)
{
  myMouseEnabled = enable;
}

//================================================================
// Function : SalomeApp_ListView::eventFilter
/*! Purpose  : event filter*/
//================================================================
bool SalomeApp_ListView::eventFilter(QObject* object, QEvent* event)
{
  if (object == viewport() &&
       (event->type() == QEvent::MouseButtonPress   ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseButtonDblClick)  &&
      !isMouseEnabled())
    return true;
  else
    return QListView::eventFilter(object, event);
}

//================================================================
// Function : SalomeApp_ListView::enableEditing
/*! Purpose  : setting editing of items availbale/not available*/
//================================================================
void SalomeApp_ListView::enableEditing(bool theFlag)
{
  myEditingEnabled = theFlag;
  if (!myEditingEnabled) {
    if (myEdit) {
      delete myEdit;
      myEdit = 0;
      myEditedItem = 0;
    }
  }
}

//================================================================
// Function : SalomeApp_ListView::isEnableEditing
/*! Purpose  : says if editing is enabled*/
//================================================================
bool SalomeApp_ListView::isEnableEditing()
{
  return myEditingEnabled;
}

//================================================================
// Function : SalomeApp_ListView::accept
/*! Purpose  : calls finishEditing(true)...*/
//================================================================
void SalomeApp_ListView::accept()
{
  finishEditing(true);
}

//================================================================
// Function : QAD_ListView::onSelectionChanged
/*! Purpose  : slot, called when selection changed in List Viewer*/
//================================================================
void SalomeApp_ListView::onSelectionChanged()
{
  if (myEdit) {
    finishEditing(true);
    delete myEdit;
    myEdit = 0;
    if (myEditedItem && !myEditedItem->isAccepted()) {
      delete myEditedItem;
      updateContents();
    }
    myEditedItem = 0;
  }
  // editing is allowed in Single Selection Mode only
  if (selectionMode() != Single || !isEnableEditing())
    return;
  SalomeApp_ListViewItem* anItem = (SalomeApp_ListViewItem*)selectedItem();
  if (anItem) {
    if (!anItem->isEditable())
      return;
    myEdit = anItem->startEditing();
    if (myEdit) {
      connect(myEdit, SIGNAL(returnPressed()), this, SLOT(onEditOk()));
      connect(myEdit, SIGNAL(escapePressed()), this, SLOT(onEditCancel()));
      myEditedItem = anItem;
      myEdit->show();
      myEdit->setFocus();
    }
  }
}

//================================================================
// Function : SalomeApp_ListView::resizeEvent
/*! Purpose  : called when Data Viewer is resized*/
//================================================================
void SalomeApp_ListView::resizeEvent( QResizeEvent * e)
{
  QListView::resizeEvent(e);
  int aW = columnWidth(columns()-1);
  int aX = header()->sectionPos(columns()-1);
  if (aW < width() - frameWidth() * 2 - aX - 1)
    setColumnWidth(columns()-1, width() - frameWidth() * 2 - aX - 1);
  updateContents();
}

//================================================================
// Function : SalomeApp_ListView::onHeaderSizeChange
/*! Purpose  : slot, called when columns sizes are changed*/
//================================================================
void SalomeApp_ListView::onHeaderSizeChange(int, int, int)
{
  int aW = columnWidth(columns()-1);
  int aX = header()->sectionPos(columns()-1);
  if (aW < width() - frameWidth() * 2 - aX - 1)
    setColumnWidth(columns()-1, width() - frameWidth() * 2 - aX - 1);
}

//================================================================
// Function : SalomeApp_ListView::viewportPaintEvent
/*! Purpose  : handler for paint event*/
//================================================================
void SalomeApp_ListView::viewportPaintEvent(QPaintEvent* e)
{
  QListView::viewportPaintEvent(e);
  if (myEditedItem && myEdit) {
    computeEditGeometry(myEditedItem, myEdit);
  }
}

//================================================================
// Function : SalomeApp_ListView::onEditOk
/*! Purpose  : called when user finishes in editing of item*/
//================================================================
void SalomeApp_ListView::onEditOk()
{
  finishEditing(true);
}

//================================================================
// Function : SalomeApp_ListView::onEditCancel
/*! Purpose  : called when user cancels item editing*/
//================================================================
void SalomeApp_ListView::onEditCancel()
{
  finishEditing(false);
}

//================================================================
// Function : SalomeApp_ListView::finishEditing
/*! Purpose  : finishes editing of entity*/
//================================================================
UpdateType SalomeApp_ListView::finishEditing(bool ok)
{
  UpdateType aNeedsUpdate = utCancel;
  if (myEditedItem && myEdit)
  {
    disconnect(myEdit, SIGNAL(returnPressed()), this, SLOT(onEditOk()));
    disconnect(myEdit, SIGNAL(escapePressed()), this, SLOT(onEditCancel()));
    myEditedItem->setAccepted(true);
    if (ok) {
      aNeedsUpdate = myEditedItem->finishEditing(myEdit);
      if (aNeedsUpdate == utCancel) {
	// something to do here on Cancel...
      }
      else {
	// something to do here on OK...
      }
      // updating contents
      switch (aNeedsUpdate) {
      case utUpdateItem:
        {
          if (myEditedItem)
            myEditedItem->updateAllLevels();
          break;
        }
      case utUpdateParent:
        {
          if (myEditedItem) {
            SalomeApp_ListViewItem* aParent = (SalomeApp_ListViewItem*)(myEditedItem->parent());
            if (aParent)
              aParent->updateAllLevels();
            else
              myEditedItem->updateAllLevels();
          }
          break;
        }
      case utUpdateViewer:
        {
          updateViewer();
          break;
        }
      case utUpdateAll:
        {
          // doing the same as for utUpdateViewer here
          // descendants can add extra processing
          updateViewer();
          break;
        }
      default:
        break;
      }
    }
  }

  // hide <myEdit> widget
  if (myEdit) {
    myEdit->hide();
  }

  return aNeedsUpdate;
}

//================================================================
// Function : SalomeApp_ListView::tip
/*! Purpose  : gets current tooltip for list view
 * \retval valid rect in success
 */
//================================================================
QRect SalomeApp_ListView::tip(QPoint aPos,
			QString& aText,
			QRect& dspRect,
			QFont& dspFnt) const
{
  QRect result( -1, -1, -1, -1 );
  SalomeApp_ListViewItem* aItem = (SalomeApp_ListViewItem*)itemAt( aPos );
  if ( aItem ) {
    for (int i = 0; i < columns(); i++) {
      QRect aItemRect = aItem->itemRect(i);
      QRect aTextRect = aItem->textRect(i);
      if ( !aItem->text(i).isEmpty() &&
	   ( aItemRect.width()  > header()->sectionSize(i) ||
	     aTextRect.left()   < 0 ||
             aTextRect.top()    < 0 ||
             aTextRect.right()  > viewport()->width() ||
             aTextRect.bottom() > viewport()->height() ) ) {
        // calculating tip data
        aText   = aItem->tipText();
	dspRect = aItem->tipRect();
	dspFnt  = font();
        if (dspRect.isValid()) {
          result  = QRect(QPoint(0, aItemRect.top()),
                          QSize(viewport()->width(), aItemRect.height()));
        }
      }
    }
  }
  return result;
}

//////////////////////////////////////////////////////////////////////
// SalomeApp_ListViewItem Class Implementation
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView* parent) :
QListViewItem( parent )
{
  init();
}

//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
				   SalomeApp_ListViewItem* after) :
QListViewItem( parent, after )
{
  init();
}

//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
				   const QString&    theName,
				   const bool        theEditable) :
QListViewItem(parent, theName)
{
  init();
  setEditable(theEditable);
}

//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
				   const QString&    theName,
				   const QString&    theValue,
				   const bool        theEditable) :
QListViewItem(parent, theName, theValue)
{
  init();
  setEditable(theEditable);
}

//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListViewItem* parent,
				   const QString&    theName,
				   const bool        theEditable) :
QListViewItem(parent, theName)
{
  init();
  setEditable(theEditable);
}

//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListViewItem* parent,
				   SalomeApp_ListViewItem* after,
				   const QString&    theName,
				   const bool        theEditable) :
QListViewItem(parent, after, theName)
{
  init();
  setEditable(theEditable);
}

//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
				   SalomeApp_ListViewItem* after,
				   const QString&    theName,
				   const bool        theEditable) :
QListViewItem(parent, after, theName)
{
  init();
  setEditable(theEditable);
}


//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListViewItem* parent,
				   const QString&    theName,
				   const QString&    theValue,
				   const bool        theEditable) :
QListViewItem(parent, theName, theValue)
{
  init();
  setEditable(theEditable);
}


//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListViewItem* parent,
				   SalomeApp_ListViewItem* after,
				   const QString&    theName,
				   const QString&    theValue,
				   const bool        theEditable) :
QListViewItem(parent, after, theName, theValue)
{
  init();
  setEditable(theEditable);
}

//================================================================
// Function : SalomeApp_ListViewItem::SalomeApp_ListViewItem
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
				   SalomeApp_ListViewItem* after,
				   const QString&    theName,
				   const QString&    theValue,
				   const bool        theEditable) :
QListViewItem(parent, after, theName, theValue)
{
  init();
  setEditable(theEditable);
}

//================================================================
// Function : SalomeApp_ListViewItem::~SalomeApp_ListViewItem
/*! Purpose  : destructor*/
//================================================================
SalomeApp_ListViewItem::~SalomeApp_ListViewItem()
{
}

//================================================================
// Function : SalomeApp_ListViewItem::init
/*! Purpose  : initialization*/
//================================================================
void SalomeApp_ListViewItem::init()
{
  myEditable    = false;
  myAccepted    = true;
  myEditingType = (int)SalomeApp_EntityEdit::etLineEdit;
  myValueType   = (int)SalomeApp_EntityEdit::vtString;
  myButtons     = 0;
  myUserType    = -1;
}

//================================================================
// Function : SalomeApp_ListViewItem::getName
/*! Purpose  : as default returns text in the first column*/
//================================================================
QString SalomeApp_ListViewItem::getName() const
{
  return ( listView()->columns() > 0 ) ? text(0) : QString("");
}

//================================================================
// Function : SalomeApp_ListViewItem::setName
/*! Purpose  : as default sets text in the first column*/
//================================================================
UpdateType SalomeApp_ListViewItem::setName(const QString& theName)
{
  UpdateType aNeedsUpdate = utCancel;
  if (listView()->columns() > 0) {
    setText(0, theName);
    aNeedsUpdate = utNone;
  }
  return aNeedsUpdate;
}

//================================================================
// Function : SalomeApp_ListViewItem::getValue
/*! Purpose  : as default returns text in the second column*/
//================================================================
QString SalomeApp_ListViewItem::getValue() const
{
  return ( listView()->columns() > 1 ) ? text(1) : QString("");
}

//================================================================
// Function : SalomeApp_ListViewItem::setValue
/*! Purpose  : as default sets text in the second column*/
//================================================================
UpdateType SalomeApp_ListViewItem::setValue(const QString& theValue)
{
  UpdateType aNeedsUpdate = utCancel;
  if (listView()->columns() > 1) {
    setText(1, theValue);
    aNeedsUpdate = utNone;
  }
  return aNeedsUpdate;
}

//================================================================
// Function : SalomeApp_ListViewItem::fullName
/*! Purpose  : returns full path to the entity from the root*/
//================================================================
QString SalomeApp_ListViewItem::fullName()
{
  QString aFullName = getName();
  SalomeApp_ListViewItem* aParent = (SalomeApp_ListViewItem*)parent();
  while(aParent != NULL) {
    aFullName = aParent->getName() + QString(".") + aFullName;
    aParent = (SalomeApp_ListViewItem*)(aParent->parent());
  }
  return aFullName;
}

//================================================================
// Function : SalomeApp_ListViewItem::openAllLevels
/*! Purpose  : expands all entities beginning from this level*/
//================================================================
void SalomeApp_ListViewItem::openAllLevels()
{
  setOpen(true);
  SalomeApp_ListViewItem* aChild = (SalomeApp_ListViewItem*)firstChild();
  while( aChild ) {
    aChild->openAllLevels();
    aChild = (SalomeApp_ListViewItem*)(aChild->nextSibling());
  }
}

//================================================================
// Function : SalomeApp_ListViewItem::updateAllLevels
/*! Purpose  : update all entites beginning from this level*/
//================================================================
void SalomeApp_ListViewItem::updateAllLevels()
{
  SalomeApp_ListViewItem* aChild = (SalomeApp_ListViewItem*)firstChild();
  while( aChild ) {
    aChild->updateAllLevels();
    aChild = (SalomeApp_ListViewItem*)(aChild->nextSibling());
  }
}

//================================================================
// Function : SalomeApp_EditBox::isEditable
/*! Purpose  : return true if entity is editable*/
//================================================================
bool SalomeApp_ListViewItem::isEditable() const
{
  return myEditable;
}

//================================================================
// Function : SalomeApp_ListViewItem::setEditable
/*! Purpose  : sets editable flag fo the entity*/
//================================================================
void SalomeApp_ListViewItem::setEditable(bool theEditable)
{
  myEditable = theEditable;
}

//================================================================
// Function : SalomeApp_ListViewItem::isAccepted
/*! Purpose  : returns true if entitiy is accepted after editing*/
//================================================================
bool SalomeApp_ListViewItem::isAccepted() const
{
  return myAccepted;
}

//================================================================
// Function : SalomeApp_ListViewItem::setAccepted
/*! Purpose  : set entitiy accepted or not after editing*/
//================================================================
void SalomeApp_ListViewItem::setAccepted(bool theAccepted)
{
  myAccepted = theAccepted;
}

//================================================================
// Function : SalomeApp_ListViewItem::getEditingType
/*! Purpose  : 
 * \retval type of edit control (default is edit box)
 *     \li 0 - edit box
 *     \li 1 - combo box
 *     \li 2 - editable combo box
 */
//================================================================
int SalomeApp_ListViewItem::getEditingType()
{
  return myEditingType;
}

//================================================================
// Function : SalomeApp_ListViewItem::setEditingType
/*! Purpose  : 
 * \retval type of edit control (negative value means none)
 *     \li 0 - edit box
 *     \li 1 - combo box
 *     \li 2 - editable combo box
 */
//================================================================
void SalomeApp_ListViewItem::setEditingType(const int type)
{
  myEditingType = type;
}

//================================================================
// Function : SalomeApp_ListViewItem::getEditedColumn
// Purpose  : 
/*! \retval edited column, default is last column
 *   negative value means there are no editable columns
 */
//================================================================
int SalomeApp_ListViewItem::getEditedColumn()
{
  return listView()->columns()-1;
}

//================================================================
// Function : SalomeApp_ListViewItem::getValueType
// Purpose  : 
/*!\retval type of edited value (string, int, double)
 * default is string
 */
//================================================================
int SalomeApp_ListViewItem::getValueType()
{
  return myValueType;
}

//================================================================
// Function : SalomeApp_ListViewItem::setValueType
/*! Purpose  : sets type of edited value*/
//================================================================
void SalomeApp_ListViewItem::setValueType(const int valueType)
{
  myValueType = valueType;
}

//================================================================
// Function : SalomeApp_ListViewItem::getUserType
/*! Purpose  : sets type of edited value*/
//================================================================
int SalomeApp_ListViewItem::getUserType()
{
  return myUserType;
}

//================================================================
// Function : SalomeApp_ListViewItem::setUserType
/*! Purpose  : sets type of edited value*/
//================================================================
void SalomeApp_ListViewItem::setUserType(const int userType)
{
  myUserType = userType;
}

//================================================================
// Function : SalomeApp_ListViewItem::getButtons
/*! Purpose  : returns buttons for editing widget (Apply (V), Cancel (X))
 * default is both buttons
 */
//================================================================
int SalomeApp_ListViewItem::getButtons()
{
  return myButtons;
}

//================================================================
// Function : SalomeApp_ListViewItem::getButtons
/*! Purpose  : sets buttons for editing widget (Apply (V), Cancel (X))*/
//================================================================
void SalomeApp_ListViewItem::setButtons(const int buttons)
{
  myButtons = buttons;
}

//================================================================
// Function : SalomeApp_ListViewItem::startEditing
/*! Purpose  : creates control for editing and fills it with values*/
//================================================================
SalomeApp_EntityEdit* SalomeApp_ListViewItem::startEditing()
{
  SalomeApp_EntityEdit* aWidget = 0;
  QListView* aListView = listView();
  if (aListView) {
    if (!isEditable())
      return 0;
    int anEditType   = getEditingType();
    int aValueType   = getValueType();
    int aButtons     = getButtons();
    int anEditColumn = getEditedColumn();
    if (anEditColumn < 0 || anEditType < 0)
      return 0;
    aWidget = new SalomeApp_EntityEdit(aListView->viewport(),
                                 anEditType,
				 aValueType,
				 aButtons & SalomeApp_EntityEdit::btApply,
				 aButtons & SalomeApp_EntityEdit::btCancel);
    computeEditGeometry(this, aWidget);

    fillWidgetWithValues(aWidget);
  }
  return aWidget;
}

//================================================================
// Function : SalomeApp_ListViewItem::fillWidgetWithValues
/*! Purpose  : fills widget with initial values (list or single value)*/
//================================================================
void SalomeApp_ListViewItem::fillWidgetWithValues(SalomeApp_EntityEdit* theWidget)
{
  int anEditColumn = getEditedColumn();
  if (theWidget && anEditColumn >= 0 && !text(anEditColumn).isEmpty())
    theWidget->insertItem(text(anEditColumn), true);
}

//================================================================
// Function : SalomeApp_ListViewItem::finishEditing
/*! Purpose  : finishes editing of entity*/
//================================================================
UpdateType SalomeApp_ListViewItem::finishEditing(SalomeApp_EntityEdit* theWidget)
{
  UpdateType aNeedsUpdate = utCancel;
  try {
    if (theWidget) {
      int anEditColumn = getEditedColumn();
      switch (anEditColumn) {
      case 0:
        aNeedsUpdate = setName(theWidget->getText());
        break;
      case 1:
        aNeedsUpdate = setValue(theWidget->getText());
        break;
      default:
        break;
      }
    }
  }
  catch (...) {
    MESSAGE( "System error has been caught - SalomeApp_ListViewItem::finishEditing" )
  }
  return aNeedsUpdate;
}

//================================================================
// Function : SalomeApp_ListViewItem::tipRect
/*! Purpose  : calculates rectangle which should contain item's tip*/
//================================================================
QRect SalomeApp_ListViewItem::tipRect()
{
  QRect aRect = QRect(-1, -1, -1, -1);
  QRect aItemRect = listView()->itemRect(this);
  if ( !aItemRect.isValid() )
    return aItemRect;

  QString aTip = tipText();
  if (!aTip.isEmpty()) {
    QRect aRect0 = textRect(0);
    QFont aFont(listView()->font());
    QFontMetrics fm(aFont);
    int iw = fm.width(aTip);
    aRect = QRect(QPoint(aRect0.x() < 0 ? 0 : aRect0.x(),
                         aRect0.y()),
                  QSize (iw,
                         aRect0.height()));
  }
  return aRect;
}

//================================================================
// Function : SalomeApp_ListViewItem::tipText
/*! Purpose  : returns text for tooltip*/
//================================================================
QString SalomeApp_ListViewItem::tipText()
{
  QString aText = getName();
  if (!getValue().isEmpty())
    aText += QString(" : ") + getValue();
  return aText;
}

//================================================================
// Function : SalomeApp_ListViewItem::textRect
/*! Purpose  : calculates rect of item text in viewport coordinates*/
//================================================================
QRect SalomeApp_ListViewItem::textRect(const int column) const
{
  QRect aItemRect = listView()->itemRect( this );
  if ( !aItemRect.isValid() )
    return aItemRect;

  QFont aFont(listView()->font());
  QFontMetrics fm(aFont);

  int decorWidth  = ( listView()->rootIsDecorated() ) ?
                    ( listView()->treeStepSize() * (depth() + 1) ) :
                    ( listView()->treeStepSize() *  depth() );
  int pixmapWidth = ( pixmap(column) ) ?
                      pixmap(column)->width() +  listView()->itemMargin() * 2 :
                      listView()->itemMargin();
  int prevWidth = 0;
  for (int i = 0; i < column; i++)
    prevWidth += listView()->header()->sectionSize(i);
  int ix = prevWidth   +
           pixmapWidth +
           ((column == 0) ? decorWidth : 0);
  int iy = aItemRect.y();
  int iw = fm.width(text(column));
  int ih = aItemRect.height();
  if (pixmap(column)) {
    iy += listView()->itemMargin();
    ih -= listView()->itemMargin() * 2;
  }
  ix -= listView()->contentsX();

  QRect theResult(QPoint(ix, iy), QSize(iw, ih));
  return theResult;
}

//================================================================
// Function : SalomeApp_ListViewItem::itemRect
/*! Purpose  : calculates rect of item data in viewport coordinates*/
//================================================================
QRect SalomeApp_ListViewItem::itemRect(const int column) const
{
  QRect aItemRect = listView()->itemRect( this );
  if ( !aItemRect.isValid() )
    return aItemRect;

  QFont aFont(listView()->font());
  QFontMetrics fm(aFont);

  int decorWidth  = ( listView()->rootIsDecorated() ) ?
                    ( listView()->treeStepSize() * (depth() + 1) ) :
                    ( listView()->treeStepSize() *  depth() );
  int pixmapWidth = ( pixmap(column) ) ?
                      pixmap(column)->width() +  listView()->itemMargin() * 2 :
                      0;
  int prevWidth = 0;
  for (int i = 0; i < column; i++)
    prevWidth += listView()->header()->sectionSize(i);
  int ix = prevWidth;
  int iy = aItemRect.y();
  int iw = pixmapWidth +
           listView()->itemMargin() * 2 +
           ((column == 0) ? decorWidth : 0) +
           fm.width(text(column));
  int ih = aItemRect.height();
  ix -= listView()->contentsX();

  QRect theResult(QPoint(ix, iy), QSize(iw, ih));
  return theResult;
}

//////////////////////////////////////////////////////////////////////
// SalomeApp_EditBox class implementation
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : SalomeApp_EditBox::SalomeApp_EditBox
/*! Purpose  : constructor*/
//================================================================
SalomeApp_EditBox::SalomeApp_EditBox(QWidget* parent) :
QLineEdit(parent)
{
}

//================================================================
// Function : SalomeApp_EditBox::keyPressEvent
/*! Purpose  : event filter for key pressing*/
//================================================================
void SalomeApp_EditBox::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Key_Escape )
    emit escapePressed();
  else
    QLineEdit::keyPressEvent( e );
  e->accept();
}

//////////////////////////////////////////////////////////////////////
// SalomeApp_ComboBox class implementation
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : SalomeApp_ComboBox::SalomeApp_ComboBox
/*! Purpose  : constructor*/
//================================================================
SalomeApp_ComboBox::SalomeApp_ComboBox(bool rw, QWidget* parent, const char* name) :
QComboBox(rw, parent, name)
{
}

//================================================================
// Function : SalomeApp_ComboBox::findItem
/*! Purpose  : searches item in list and returns its index*/
//================================================================
int SalomeApp_ComboBox::findItem(const QString& theText)
{
  for (int i = 0; i < count(); i++)
    if (text(i) == theText)
      return i;
  return -1;
}

//================================================================
// Function : SalomeApp_ComboBox::insertItem
/*! Purpose  : adds item in combo box*/
//================================================================
void SalomeApp_ComboBox::insertItem(const QString& theValue,
			      int            theIndex)
{
  if (duplicatesEnabled() || findItem(theValue) < 0)
    QComboBox::insertItem(theValue, theIndex);
}

//================================================================
// Function : SalomeApp_ComboBox::insertList
/*! Purpose  : adds list of items in combo box*/
//================================================================
void SalomeApp_ComboBox::insertList(const QStringList& theList)
{
  for (unsigned i = 0; i < theList.count(); i++)
    insertItem(theList[i]);
}

//================================================================
// Function : SalomeApp_ComboBox::insertItem
/*! Purpose  : adds item in combo box*/
//================================================================
void SalomeApp_ComboBox::insertItem(const int theValue)
{
  int aNum;
  bool bOk;
  for (int i = 0; i < count(); i++) {
    aNum = text(i).toInt(&bOk);
    if (bOk) {
      if (aNum > theValue || (aNum == theValue && duplicatesEnabled())) {
        insertItem(QString::number(theValue), i);
        return;
      }
    }
  }
  insertItem(QString::number(theValue));
}

//================================================================
// Function : SalomeApp_ComboBox::insertList
/*! Purpose  : adds list of items in combo box*/
//================================================================
void SalomeApp_ComboBox::insertList(const TColStd_ListOfInteger& theList)
{
  for (TColStd_ListIteratorOfListOfInteger aIter(theList); aIter.More(); aIter.Next())
    insertItem(aIter.Value());
}

//================================================================
// Function : SalomeApp_ComboBox::insertItem
/*! Purpose  : adds item in combo box*/
//================================================================
void SalomeApp_ComboBox::insertItem(const double theValue)
{
  double aNum;
  bool bOk;
  for (int i = 0; i < count(); i++) {
    aNum = text(i).toDouble(&bOk);
    if (bOk) {
      if (aNum > theValue || (aNum == theValue && duplicatesEnabled())) {
        insertItem(QString::number(theValue), i);
        return;
      }
    }
  }
  insertItem(QString::number(theValue));
}

//================================================================
// Function : SalomeApp_ComboBox::insertList
/*! Purpose  : adds list of items in combo box*/
//================================================================
void SalomeApp_ComboBox::insertList(const TColStd_ListOfReal& theList)
{
  for (TColStd_ListIteratorOfListOfReal aIter(theList); aIter.More(); aIter.Next())
    insertItem(aIter.Value());
}

//////////////////////////////////////////////////////////////////////
// SalomeApp_EntityEdit class implementation
//////////////////////////////////////////////////////////////////////

#include <qlayout.h>

#define MIN_COMBO_WIDTH     1
#define MIN_EDIT_WIDTH      1

//================================================================
// Function : SalomeApp_EntityEdit::SalomeApp_EntityEdit
/*! Purpose  : constructor*/
//================================================================
SalomeApp_EntityEdit::SalomeApp_EntityEdit(QWidget* parent,
			       int      controlType,
			       int      valueType,
			       bool     butApply,
			       bool     butCancel) :
QWidget(parent),
myEdit(0),
myCombo(0),
myApplyBtn(0),
myCancelBtn(0)
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  SUIT_ResourceMgr* mgr = app ? app->resourceMgr() : NULL;

  QHBoxLayout* aTopLayout = new QHBoxLayout(this);
  aTopLayout->setAlignment( Qt::AlignTop );
  aTopLayout->setSpacing( 0 );
  aTopLayout->setMargin( 1 );
  if (controlType != etLineEdit &&
      controlType != etComboBox &&
      controlType != etComboEdit)
    controlType = etLineEdit;
  if (controlType == etComboBox || controlType == etComboEdit) {
    // this is an editable combo box
    myCombo = new SalomeApp_ComboBox(controlType == etComboEdit, this);
    myCombo->setMinimumSize(MIN_COMBO_WIDTH, 0);
    myCombo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed));
    // no insertions
    myCombo->setInsertionPolicy(QComboBox::NoInsertion);
    // no duplicates enabled by default
    myCombo->setDuplicatesEnabled(false);
    aTopLayout->addWidget(myCombo);
    // connect signals
    connect(myCombo, SIGNAL(activated(const QString&)), this, SLOT(onComboActivated(const QString&)));
    connect(myCombo, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
  }
  else {
    // and this is an edit box
    myEdit = new SalomeApp_EditBox(this);
    myEdit->setMinimumSize(MIN_EDIT_WIDTH, 0);
    myEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Fixed));
    aTopLayout->addWidget(myEdit);
    connect(myEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
    connect(myEdit, SIGNAL(returnPressed()), this, SLOT(onApply()));
    connect(myEdit, SIGNAL(escapePressed()), this, SLOT(onCancel()));
  }
  if (valueType != vtString &&
      valueType != vtInteger &&
      valueType != vtDouble)
    valueType = vtString;
  if (valueType == vtInteger)
    setValidator(new QIntValidator(this));
  else if (valueType == vtDouble)
    setValidator(new QDoubleValidator(this));
  if (butApply) {
    // Apply button (V)
    myApplyBtn = new QToolButton(this);

    QPixmap anIcon;
    if( mgr )
      anIcon = mgr->loadPixmap( "STD", tr( "ICON_APPLY" ), false );

    myApplyBtn->setPixmap(anIcon);
    myApplyBtn->setEnabled(false);
    myApplyBtn->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    myApplyBtn->setMinimumSize(16, 16);
    myApplyBtn->setMaximumSize(16, 20);
    aTopLayout->addWidget(myApplyBtn);
    connect(myApplyBtn, SIGNAL(clicked()), this, SLOT(onApply()));
  }
  if (butCancel) {
    // Cancel button (X)
    myCancelBtn = new QToolButton(this);
    QPixmap anIcon;
    if( mgr )
      anIcon = mgr->loadPixmap( "STD", tr( "ICON_CANCEL" ), false );
    myCancelBtn->setPixmap(anIcon);
    myCancelBtn->setEnabled(false);
    myCancelBtn->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    myCancelBtn->setMinimumSize(16, 16);
    myCancelBtn->setMaximumSize(16, 20);
    aTopLayout->addWidget(myCancelBtn);
    connect(myCancelBtn, SIGNAL(clicked()), this, SLOT(onCancel()));
  }
}

//================================================================
// Function : SalomeApp_EntityEdit::~SalomeApp_EntityEdit
/*! Purpose  : destructor*/
//================================================================
SalomeApp_EntityEdit::~SalomeApp_EntityEdit()
{
}

//================================================================
// Function : SalomeApp_EntityEdit::clear
/*! Purpose  : clears edit/combo box*/
//================================================================
void SalomeApp_EntityEdit::clear()
{
  if (myEdit)
    myEdit->clear();
  if (myCombo)
    myCombo->clear();
}

//================================================================
// Function : SalomeApp_EntityEdit::getText
/*! Purpose  : returns current text in edit box or combo box*/
//================================================================
QString SalomeApp_EntityEdit::getText()
{
  if (myEdit)
    return myEdit->text();
  else if (myCombo)
    return myCombo->currentText();
  else
    return "";
}

//================================================================
// Function : SalomeApp_EntityEdit::setText
/*! Purpose  : sets text*/
//================================================================
void SalomeApp_EntityEdit::setText(const QString& theText)
{
  myString = theText;
  if (myEdit)
    myEdit->setText(theText);
  if (myCombo) {
    int aFound = myCombo->findItem(theText);
    if (aFound >= 0) {
      myCombo->setCurrentItem(aFound);
      onTextChanged(theText);
    }
  }
}

//================================================================
// Function : SalomeApp_EntityEdit::insertItem
/*! Purpose  : adds item in combo box,
 *            sets it current if theSetCurrent is true
 */
//================================================================
void SalomeApp_EntityEdit::insertItem(const QString& theValue,
				bool           theSetCurrent,
				int            theOrder)
{
  if (myCombo) {
    int aIndexAt = -1;
    if (theOrder == atTop)
      aIndexAt = 0;
    else if (theOrder == atBeforeCurrent && myCombo->count() > 0)
      aIndexAt = myCombo->currentItem();
    else if (theOrder == atAfterCurrent &&
             myCombo->count() > 0 &&
             myCombo->currentItem() < myCombo->count()-1)
      aIndexAt = myCombo->currentItem() + 1;
    myCombo->insertItem(theValue, aIndexAt);
  }
  if (theSetCurrent)
    setText(theValue);
}

//================================================================
// Function : SalomeApp_EntityEdit::insertList
/*! Purpose  : adds items in combo box,
 *            sets item theCurrent as current
 */
//================================================================
void SalomeApp_EntityEdit::insertList(const QStringList& theList,
				const int          theCurrent)
{
  if (myCombo)
    myCombo->insertList(theList);
  if (theCurrent >= 0 && theCurrent < (int)theList.count())
    setText(theList[theCurrent]);
}

//================================================================
// Function : SalomeApp_EntityEdit::insertItem
/*! Purpose  : adds item in combo box,
 *            sets it current if theSetCurrent is true
 */
//================================================================
void SalomeApp_EntityEdit::insertItem(const int theValue,
				bool      theSetCurrent)
{
  if (myCombo) {
    myCombo->insertItem(theValue);
  }
  if (theSetCurrent)
    setText(QString::number(theValue));
}

//================================================================
// Function : SalomeApp_EntityEdit::insertList
/*! Purpose  : adds items in combo box,
 *            sets item theCurrent as current
 */
//================================================================
void SalomeApp_EntityEdit::insertList(const TColStd_ListOfInteger& theList,
				const int                    theCurrent)
{
  if (myCombo)
    myCombo->insertList(theList);

  TColStd_ListIteratorOfListOfInteger aIter(theList);
  for (unsigned i = 0; aIter.More(); aIter.Next(), i++) {
    if (theCurrent == i) {
      setText(QString::number(aIter.Value()));
      break;
    }
  }
}

//================================================================
// Function : SalomeApp_EntityEdit::insertItem
// Purpose  : adds item in combo box,
//            sets it current if theSetCurrent is true
//================================================================
void SalomeApp_EntityEdit::insertItem(const double theValue,
				bool         theSetCurrent)
{
  if (myCombo) {
    myCombo->insertItem(theValue);
  }
  if (theSetCurrent)
    setText(QString::number(theValue));
}

//================================================================
// Function : SalomeApp_EntityEdit::insertList
/*! Purpose  : adds items in combo box,
 *            sets item theCurrent as current
 */
//================================================================
void SalomeApp_EntityEdit::insertList(const TColStd_ListOfReal& theList,
				const int                 theCurrent)
{
  if (myCombo)
    myCombo->insertList(theList);

  TColStd_ListIteratorOfListOfReal aIter(theList);
  for (unsigned i = 0; aIter.More(); aIter.Next(), i++) {
    if (theCurrent == i) {
      setText(QString::number(aIter.Value()));
      break;
    }
  }
}

//================================================================
// Function : SalomeApp_EntityEdit::getControl
/*! Purpose  : gets actual widget*/
//================================================================
QWidget* SalomeApp_EntityEdit::getControl()
{
  if (myEdit)
    return myEdit;
  else if (myCombo)
    return myCombo;
  else
    return 0;
}

//================================================================
// Function : SalomeApp_EntityEdit::setFocus
/*! Purpose  : redirect focus to corresponding widget*/
//================================================================
void SalomeApp_EntityEdit::setFocus()
{
  if (myEdit) {
    myEdit->setFocus();
    //myEdit->selectAll();
  }
  else if (myCombo && myCombo->editable()) {
    myCombo->setFocus();
    //myCombo->lineEdit()->selectAll();
  }
}

//================================================================
// Function : SalomeApp_EntityEdit::setValidator
/*! Purpose  : sets validator for the control*/
//================================================================
void SalomeApp_EntityEdit::setValidator(const QValidator* theValidator)
{
  if (myEdit)
    myEdit->setValidator(theValidator);
  if (myCombo)
    myCombo->setValidator(theValidator);
}

//================================================================
// Function : SalomeApp_EntityEdit::keyPressEvent
/*! Purpose  : event filter for KeyPress event*/
//================================================================
void SalomeApp_EntityEdit::keyPressEvent( QKeyEvent * e)
{
  if ( (e->key() == Key_Enter ||
        e->key() == Key_Return ) )
    onApply();
  else if (e->key() == Key_Escape)
    onCancel();
}

//================================================================
// Function : SalomeApp_EntityEdit::onComboActivated
/*! Purpose  : called when item activated in combo box*/
//================================================================
void SalomeApp_EntityEdit::onComboActivated(const QString& theText)
{
  onTextChanged(theText);
}

//================================================================
// Function : SalomeApp_EntityEdit::onTextChanged
/*! Purpose  : slot, called when text changed in line edit*/
//================================================================
void SalomeApp_EntityEdit::onTextChanged(const QString& theText)
{
  if (myApplyBtn)
    myApplyBtn->setEnabled(!(theText == myString));
  if (myCancelBtn)
    myCancelBtn->setEnabled(!(theText == myString));
}

//================================================================
// Function : SalomeApp_EntityEdit::onCancel
/*! Purpose  : slot, called when user presses Cancel button*/
//================================================================
void SalomeApp_EntityEdit::onCancel()
{
  setText(myString);
  if (myApplyBtn)
    myApplyBtn->setEnabled(false);
  if (myCancelBtn)
    myCancelBtn->setEnabled(false);
  emit escapePressed();
}

//================================================================
// Function : SalomeApp_EntityEdit::onApply
/*! Purpose  : slot, called when user presses Apply button*/
//================================================================
void SalomeApp_EntityEdit::onApply()
{
  myString = getText();
  if (myApplyBtn)
    myApplyBtn->setEnabled(false);
  if (myCancelBtn)
    myCancelBtn->setEnabled(false);
  emit returnPressed();
}

//================================================================
// Function : SalomeApp_EntityEdit::showButtons
/*! Purpose  : shows/hides buttons*/
//================================================================
void SalomeApp_EntityEdit::showButtons(bool show)
{
  if (myApplyBtn)
    show ? myApplyBtn->show()  : myApplyBtn->hide();
  if (myCancelBtn)
    show ? myCancelBtn->show() : myCancelBtn->hide();
}

//================================================================
// Function : SalomeApp_EntityEdit::setDuplicatesEnabled
/*! Purpose  : enables/disables data duplication (for combo box)*/
//================================================================
void SalomeApp_EntityEdit::setDuplicatesEnabled(bool enabled)
{
  if (myCombo)
    myCombo->setDuplicatesEnabled(enabled);
}
