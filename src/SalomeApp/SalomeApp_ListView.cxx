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

// File   : SalomeApp_ListView.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "SalomeApp_ListView.h"
#include "SalomeApp_Application.h"

#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"

#include "utilities.h"

#include <QValidator>
#include <QToolButton>
#include <QPixmap>
#include <QHeaderView>
#include <QKeyEvent>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListOfReal.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>

/*!
  Used for resizing editing widget
*/
void computeEditGeometry(SalomeApp_ListViewItem* theItem,
                         SalomeApp_EntityEdit*   theWidget)
{
  if (!theItem)
    return;
  QTreeWidget* aListView = theItem->treeWidget();
  int anEditColumn = theItem->getEditedColumn();
  if (anEditColumn < 0)
    return;

  int aX = 0, aY = 0, aW = 0, aH = 0;

  QRect aRect = aListView->visualItemRect(theItem);
  aX = aListView->header()->sectionViewportPosition(anEditColumn);
  if (aX < 0)
    aX = 0; // THIS CAN BE REMOVED
  QSize aSize = theWidget->getControl()->sizeHint();
  aH = qMax(aSize.height() , aRect.height() );
  aY = aRect.y() - ((aH - aRect.height()) / 2);
  //aW = aListView->columnWidth(anEditColumn); // CAN SUBSTITUTE NEXT 3 ROWS
  aW = aListView->viewport()->width() - aX;
  if (aW < 0)
    aW = 0;
  theWidget->setGeometry(aX, aY, aW, aH);
}

/*!
  Constructor
*/
SalomeApp_ListView::SalomeApp_ListView( QWidget* parent )
  : QTreeWidget/*QtxListView*/( parent )
{
  myMouseEnabled = true;
  myEditingEnabled = false;
  setSelectionMode(QAbstractItemView::SingleSelection);
  setRootIsDecorated(false);
  setAllColumnsShowFocus(false);
//  header()->setClickEnabled(false);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  header()->setMovable(false);
#else
  header()->setSectionsMovable(false);
#endif

  myEditedItem = 0;
  myEdit = 0;

  viewport()->installEventFilter(this);

  connect(this, SIGNAL(itemSelectionChanged()),
          this, SLOT(onSelectionChanged()));
  connect(header(), SIGNAL(sizeChange(int, int, int)),
          this,     SLOT(onHeaderSizeChange(int, int, int)));
}

/*!
  Destructor
*/
SalomeApp_ListView::~SalomeApp_ListView()
{
  if (myEdit) {
    delete myEdit;
  }
  myEdit = 0;
  myEditedItem = 0;
}

/*!
  Updates all data viewer
*/
void SalomeApp_ListView::updateViewer()
{
  // temporary disconnecting selection changed SIGNAL
  blockSignals(true);
  QTreeWidgetItemIterator it( this );
  SalomeApp_ListViewItem* aRoot = (SalomeApp_ListViewItem*)(*it);
  if (aRoot)
    aRoot->updateAllLevels();
  update( contentsRect() );//updateContents();
  // connecting again selection changed SIGNAL
  blockSignals(false);
  emit itemSelectionChanged();
}

/*!
  Updates currently selected item(s)
*/
void SalomeApp_ListView::updateSelected()
{
  // temporary disconnecting selection changed SIGNAL
  blockSignals(true);
  SalomeApp_ListViewItem* aChild = (SalomeApp_ListViewItem*)(selectedItems().first());
  if (aChild)
    aChild->updateAllLevels();
  update( contentsRect() );//updateContents();
  // connecting again selection changed SIGNAL
  blockSignals(false);
  emit itemSelectionChanged();
}

/*!
  Returns popup client type
*/
QString SalomeApp_ListView::popupClientType() const
{
  return "SalomeApp_ListView";
}

/*!
  Fills popup menu with items
*/
void SalomeApp_ListView::contextMenuPopup( QMenu* aPopup )
{
  if (aPopup) {
    // add items here...
  }
}

/*!
  Clears view
*/
void SalomeApp_ListView::clear()
{
  if (myEdit) {
    delete myEdit;
    myEdit = 0;
    myEditedItem = 0;
  }
  QTreeWidget::clear();
}

/*!
  \return true if mouse events are enabled
*/
bool SalomeApp_ListView::isMouseEnabled()
{
  return myMouseEnabled;
}

/*!
  Enables/disables mouse events (excluding MouseMove)
*/
void SalomeApp_ListView::enableMouse(bool enable)
{
  myMouseEnabled = enable;
}

/*!
  Event filter
*/
bool SalomeApp_ListView::eventFilter(QObject* object, QEvent* event)
{
  if (object == viewport() &&
       (event->type() == QEvent::MouseButtonPress   ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseButtonDblClick)  &&
      !isMouseEnabled())
    return true;
  else
    return QTreeWidget::eventFilter(object, event);
}

/*!
  Setting editing of items availbale/not available
*/
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

/*!
  Says if editing is enabled
*/
bool SalomeApp_ListView::isEnableEditing()
{
  return myEditingEnabled;
}

/*!
  Calls finishEditing(true)...
*/
void SalomeApp_ListView::accept()
{
  finishEditing(true);
}

/*!
  Slot, called when selection changed in List Viewer
*/
void SalomeApp_ListView::onSelectionChanged()
{
  if (myEdit) {
    finishEditing(true);
    delete myEdit;
    myEdit = 0;
    if (myEditedItem && !myEditedItem->isAccepted()) {
      delete myEditedItem;
      update( contentsRect() );//updateContents();
    }
    myEditedItem = 0;
  }
  // editing is allowed in Single Selection Mode only
  if (selectionMode() != QAbstractItemView::SingleSelection || !isEnableEditing())
    return;
  SalomeApp_ListViewItem* anItem = (SalomeApp_ListViewItem*)(selectedItems().first());
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

/*!
  Called when Data Viewer is resized
*/
void SalomeApp_ListView::resizeEvent( QResizeEvent * e)
{
  QTreeWidget::resizeEvent(e);
  int aW = columnWidth(columnCount()-1);
  int aX = header()->sectionPosition(columnCount()-1);
  if (aW < width() - frameWidth() * 2 - aX - 1)
    setColumnWidth(columnCount()-1, width() - frameWidth() * 2 - aX - 1);
  update( contentsRect() );//updateContents();
}

/*!
  Slot, called when columns sizes are changed
*/
void SalomeApp_ListView::onHeaderSizeChange(int, int, int)
{
  int aW = columnWidth(columnCount()-1);
  int aX = header()->sectionPosition(columnCount()-1);
  if (aW < width() - frameWidth() * 2 - aX - 1)
    setColumnWidth(columnCount()-1, width() - frameWidth() * 2 - aX - 1);
}

/*!
  Handler for paint event
*/
void SalomeApp_ListView::viewportPaintEvent(QPaintEvent* e)
{
  QTreeWidget::paintEvent(e);
  if (myEditedItem && myEdit) {
    computeEditGeometry(myEditedItem, myEdit);
  }
}

/*!
  Called when user finishes in editing of item
*/
void SalomeApp_ListView::onEditOk()
{
  finishEditing(true);
}

/*!
  Called when user cancels item editing
*/
void SalomeApp_ListView::onEditCancel()
{
  finishEditing(false);
}

/*!
  Finishes editing of entity
*/
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

/*!
  \return current tooltip for list view
  \retval valid rect in success
*/
QRect SalomeApp_ListView::tip(QPoint aPos,
                              QString& aText,
                              QRect& dspRect,
                              QFont& dspFnt) const
{
  QRect result( -1, -1, -1, -1 );
  SalomeApp_ListViewItem* aItem = (SalomeApp_ListViewItem*)itemAt( aPos );
  if ( aItem ) {
    for (int i = 0; i < columnCount(); i++) {
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

/*!
  Constructor
*/
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView* parent) :
QTreeWidgetItem( parent )
{
  init();
}

/*!
  Constructor
*/
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
                                               SalomeApp_ListViewItem* after) :
QTreeWidgetItem( parent, after )
{
  init();
}

/*!
  Constructor
*/
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
                                               const QStringList&    theStrings,
                                               const bool        theEditable) :
QTreeWidgetItem(parent, theStrings)
{
  init();
  setEditable(theEditable);
}

/*!
  Constructor
*/
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListViewItem* parent,
                                               const QStringList&    theString,
                                               const bool        theEditable) :
QTreeWidgetItem(parent, theString)
{
  init();
  setEditable(theEditable);
}

/*!
  Constructor
*/
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListViewItem* parent,
                                               SalomeApp_ListViewItem* after,
                                               const QString&    theName,
                                               const bool        theEditable) :
QTreeWidgetItem(parent, after)
{
  setData(0,Qt::DisplayRole,QVariant(theName));
  init();
  setEditable(theEditable);
}

/*!
  Constructor
*/
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
                                               SalomeApp_ListViewItem* after,
                                               const QString&    theName,
                                               const bool        theEditable) :
QTreeWidgetItem(parent, after)
{
  setData(0,Qt::DisplayRole,QVariant(theName));
  init();
  setEditable(theEditable);
}

/*!
  Constructor
*/
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListViewItem* parent,
                                               SalomeApp_ListViewItem* after,
                                               const QString&    theName,
                                               const QString&    theValue,
                                               const bool        theEditable) :
QTreeWidgetItem(parent, after)
{
  setData(0,Qt::DisplayRole,QVariant(theName));
  setData(1,Qt::DisplayRole,QVariant(theValue));
  init();
  setEditable(theEditable);
}

/*!
  Constructor
*/
SalomeApp_ListViewItem::SalomeApp_ListViewItem(SalomeApp_ListView*     parent,
                                               SalomeApp_ListViewItem* after,
                                               const QString&    theName,
                                               const QString&    theValue,
                                               const bool        theEditable) :
QTreeWidgetItem(parent, after)
{
  setData(0,Qt::DisplayRole,QVariant(theName));
  setData(1,Qt::DisplayRole,QVariant(theValue));
  init();
  setEditable(theEditable);
}

/*!
  Destructor
*/
SalomeApp_ListViewItem::~SalomeApp_ListViewItem()
{
}

/*!
  Initialization
*/
void SalomeApp_ListViewItem::init()
{
  myEditable    = false;
  myAccepted    = true;
  myEditingType = (int)SalomeApp_EntityEdit::etLineEdit;
  myValueType   = (int)SalomeApp_EntityEdit::vtString;
  myButtons     = 0;
  myUserType    = -1;
}

/*!
  Returns the depth of this item
*/
int SalomeApp_ListViewItem::depth() const
{
  int aDepth = 0;
  QTreeWidgetItem* aParent = parent();
  while ( aParent ) {
    aParent = aParent->parent();
    aDepth++;
  }
  return aDepth;
}

/*!
  \return text in the first column
*/
QString SalomeApp_ListViewItem::getName() const
{
  return ( treeWidget()->columnCount() > 0 ) ? text(0) : QString("");
}

/*!
  Sets text in the first column
*/
UpdateType SalomeApp_ListViewItem::setName(const QString& theName)
{
  UpdateType aNeedsUpdate = utCancel;
  if (treeWidget()->columnCount() > 0) {
    setText(0, theName);
    aNeedsUpdate = utNone;
  }
  return aNeedsUpdate;
}

/*!
  \return text in the second column
*/
QString SalomeApp_ListViewItem::getValue() const
{
  return ( treeWidget()->columnCount() > 1 ) ? text(1) : QString("");
}

/*!
  Sets text in the second column
*/
UpdateType SalomeApp_ListViewItem::setValue(const QString& theValue)
{
  UpdateType aNeedsUpdate = utCancel;
  if (treeWidget()->columnCount() > 1) {
    setText(1, theValue);
    aNeedsUpdate = utNone;
  }
  return aNeedsUpdate;
}

/*!
  \return full path to the entity from the root
*/
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

/*!
  expands all entities beginning from this level
*/
void SalomeApp_ListViewItem::openAllLevels()
{
  setExpanded(true);
  QTreeWidgetItemIterator it( this );
  SalomeApp_ListViewItem* aChild = (SalomeApp_ListViewItem*)(*it);
  while( aChild ) {
    aChild->openAllLevels();
    ++it;
    aChild = (SalomeApp_ListViewItem*)(*it);
  }
}

/*!
  update all entites beginning from this level
*/
void SalomeApp_ListViewItem::updateAllLevels()
{
  QTreeWidgetItemIterator it( this );
  SalomeApp_ListViewItem* aChild = (SalomeApp_ListViewItem*)(*it);
  while( aChild ) {
    aChild->updateAllLevels();
    ++it;
    aChild = (SalomeApp_ListViewItem*)(*it);
  }
}

/*!
  \return true if entity is editable
*/
bool SalomeApp_ListViewItem::isEditable() const
{
  return myEditable;
}

/*!
  Sets editable flag fo the entity
*/
void SalomeApp_ListViewItem::setEditable(bool theEditable)
{
  myEditable = theEditable;
}

/*!
  \return true if entitiy is accepted after editing
*/
bool SalomeApp_ListViewItem::isAccepted() const
{
  return myAccepted;
}

/*!
  Sets entitiy accepted or not after editing
*/
void SalomeApp_ListViewItem::setAccepted(bool theAccepted)
{
  myAccepted = theAccepted;
}

/*!
 \retval type of edit control (default is edit box)
      \li 0 - edit box
      \li 1 - combo box
      \li 2 - editable combo box
*/
int SalomeApp_ListViewItem::getEditingType()
{
  return myEditingType;
}

/*!
 \retval type of edit control (negative value means none)
     \li 0 - edit box
     \li 1 - combo box
     \li 2 - editable combo box
*/
void SalomeApp_ListViewItem::setEditingType(const int type)
{
  myEditingType = type;
}

/*! \retval edited column, default is last column
    negative value means there are no editable columns
*/
int SalomeApp_ListViewItem::getEditedColumn()
{
  return treeWidget()->columnCount()-1;
}

/*!
  \retval type of edited value (string, int, double)
   default is string
*/
int SalomeApp_ListViewItem::getValueType()
{
  return myValueType;
}

/*!
  Sets type of edited value
*/
void SalomeApp_ListViewItem::setValueType(const int valueType)
{
  myValueType = valueType;
}

/*!
  Sets type of edited value
*/
int SalomeApp_ListViewItem::getUserType()
{
  return myUserType;
}

/*!
  Sets type of edited value
*/
void SalomeApp_ListViewItem::setUserType(const int userType)
{
  myUserType = userType;
}

/*!
  \return buttons for editing widget (Apply (V), Cancel (X))
   default is both buttons
*/
int SalomeApp_ListViewItem::getButtons()
{
  return myButtons;
}

/*!
  Sets buttons for editing widget (Apply (V), Cancel (X))
*/
void SalomeApp_ListViewItem::setButtons(const int buttons)
{
  myButtons = buttons;
}

/*!
  Creates control for editing and fills it with values
*/
SalomeApp_EntityEdit* SalomeApp_ListViewItem::startEditing()
{
  SalomeApp_EntityEdit* aWidget = 0;
  QTreeWidget* aListView = treeWidget();
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

/*!
  Fills widget with initial values (list or single value)
*/
void SalomeApp_ListViewItem::fillWidgetWithValues(SalomeApp_EntityEdit* theWidget)
{
  int anEditColumn = getEditedColumn();
  if (theWidget && anEditColumn >= 0 && !text(anEditColumn).isEmpty())
    theWidget->insertItem(text(anEditColumn), true);
}

/*!
  Finishes editing of entity
*/
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

/*!
  Calculates rectangle which should contain item's tip
*/
QRect SalomeApp_ListViewItem::tipRect()
{
  QRect aRect = QRect(-1, -1, -1, -1);
  QRect aItemRect = treeWidget()->visualItemRect(this);
  if ( !aItemRect.isValid() )
    return aItemRect;

  QString aTip = tipText();
  if (!aTip.isEmpty()) {
    QRect aRect0 = textRect(0);
    QFont aFont(treeWidget()->font());
    QFontMetrics fm(aFont);
    int iw = fm.width(aTip);
    aRect = QRect(QPoint(aRect0.x() < 0 ? 0 : aRect0.x(),
                         aRect0.y()),
                  QSize (iw,
                         aRect0.height()));
  }
  return aRect;
}

/*!
  \return text for tooltip
*/
QString SalomeApp_ListViewItem::tipText()
{
  QString aText = getName();
  if (!getValue().isEmpty())
    aText += QString(" : ") + getValue();
  return aText;
}

/*!
  Calculates rect of item text in viewport coordinates
*/
QRect SalomeApp_ListViewItem::textRect(const int column) const
{
  QRect aItemRect = treeWidget()->visualItemRect( this );
  if ( !aItemRect.isValid() )
    return aItemRect;

  QFont aFont(treeWidget()->font());
  QFontMetrics fm(aFont);

  int decorWidth  = ( treeWidget()->rootIsDecorated() ) ?
                    ( treeWidget()->indentation() * (depth() + 1) ) :
                    ( treeWidget()->indentation() *  depth() );
  int pixmapWidth = ( !icon(column).isNull() ) ?
                      treeWidget()->iconSize().width() + 2 :
                      1;
  int prevWidth = 0;
  for (int i = 0; i < column; i++)
    prevWidth += treeWidget()->header()->sectionSize(i);
  int ix = prevWidth   +
           pixmapWidth +
           ((column == 0) ? decorWidth : 0);
  int iy = aItemRect.y();
  int iw = fm.width(text(column));
  int ih = aItemRect.height();
  if (!icon(column).isNull()) {
    iy += 1;
    ih -= 2;
  }
  ix -= treeWidget()->contentsRect().left();

  QRect theResult(QPoint(ix, iy), QSize(iw, ih));
  return theResult;
}

/*!
  Calculates rect of item data in viewport coordinates
*/
QRect SalomeApp_ListViewItem::itemRect(const int column) const
{
  QRect aItemRect = treeWidget()->visualItemRect( this );
  if ( !aItemRect.isValid() )
    return aItemRect;

  QFont aFont(treeWidget()->font());
  QFontMetrics fm(aFont);

  int decorWidth  = ( treeWidget()->rootIsDecorated() ) ?
                    ( treeWidget()->indentation() * (depth() + 1) ) :
                    ( treeWidget()->indentation() *  depth() );
  int pixmapWidth = ( !icon(column).isNull() ) ?
                      treeWidget()->iconSize().width() + 2 :
                      0;
  int prevWidth = 0;
  for (int i = 0; i < column; i++)
    prevWidth += treeWidget()->header()->sectionSize(i);
  int ix = prevWidth;
  int iy = aItemRect.y();
  int iw = pixmapWidth +
           2 +
           ((column == 0) ? decorWidth : 0) +
           fm.width(text(column));
  int ih = aItemRect.height();
  ix -= treeWidget()->contentsRect().left();

  QRect theResult(QPoint(ix, iy), QSize(iw, ih));
  return theResult;
}

/*!
  Constructor
*/
SalomeApp_EditBox::SalomeApp_EditBox(QWidget* parent) :
QLineEdit(parent)
{
}

/*!
  Event filter for key pressing
*/
void SalomeApp_EditBox::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Qt::Key_Escape )
    emit escapePressed();
  else
    QLineEdit::keyPressEvent( e );
  e->accept();
}


/*!
  Constructor
*/
SalomeApp_ComboBox::SalomeApp_ComboBox(bool rw, QWidget* parent, const char* name) :
QComboBox(parent)
{
  setEditable( rw );
  setObjectName( name );
}

/*!
  Searches item in list and returns its index
*/
int SalomeApp_ComboBox::findItem(const QString& theText)
{
  for (int i = 0; i < count(); i++)
    if (itemText(i) == theText)
      return i;
  return -1;
}

/*!
  Adds item in combo box
*/
void SalomeApp_ComboBox::insertItem(const QString& theValue,
                                    int            theIndex)
{
  if (duplicatesEnabled() || findItem(theValue) < 0)
    QComboBox::insertItem(theIndex, theValue);
}

/*!
  Adds list of items in combo box
*/
void SalomeApp_ComboBox::insertList(const QStringList& theList)
{
  for (unsigned i = 0; i < theList.count(); i++)
    insertItem(theList[i]);
}

/*!
  Adds item in combo box
*/
void SalomeApp_ComboBox::insertItem(const int theValue)
{
  int aNum;
  bool bOk;
  for (int i = 0; i < count(); i++) {
    aNum = itemText(i).toInt(&bOk);
    if (bOk) {
      if (aNum > theValue || (aNum == theValue && duplicatesEnabled())) {
        insertItem(QString::number(theValue),i);
        return;
      }
    }
  }
  insertItem(QString::number(theValue));
}

/*!
  Adds list of items in combo box
*/
void SalomeApp_ComboBox::insertList(const TColStd_ListOfInteger& theList)
{
  for (TColStd_ListIteratorOfListOfInteger aIter(theList); aIter.More(); aIter.Next())
    insertItem(aIter.Value());
}

/*!
  Adds item in combo box
*/
void SalomeApp_ComboBox::insertItem(const double theValue)
{
  double aNum;
  bool bOk;
  for (int i = 0; i < count(); i++) {
    aNum = itemText(i).toDouble(&bOk);
    if (bOk) {
      if (aNum > theValue || (aNum == theValue && duplicatesEnabled())) {
        insertItem(QString::number(theValue), i);
        return;
      }
    }
  }
  insertItem(QString::number(theValue));
}

/*!
  Adds list of items in combo box
*/
void SalomeApp_ComboBox::insertList(const TColStd_ListOfReal& theList)
{
  for (TColStd_ListIteratorOfListOfReal aIter(theList); aIter.More(); aIter.Next())
    insertItem(aIter.Value());
}

#include <qlayout.h>

#define MIN_COMBO_WIDTH     1
#define MIN_EDIT_WIDTH      1

/*!
  Constructor
*/
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
    myCombo->setInsertPolicy(QComboBox::NoInsert);
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
      anIcon = mgr->loadPixmap( "SalomeApp", tr( "ICON_APPLY" ), false );

    myApplyBtn->setIcon(anIcon);
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
      anIcon = mgr->loadPixmap( "SalomeApp", tr( "ICON_CANCEL" ), false );
    myCancelBtn->setIcon(anIcon);
    myCancelBtn->setEnabled(false);
    myCancelBtn->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    myCancelBtn->setMinimumSize(16, 16);
    myCancelBtn->setMaximumSize(16, 20);
    aTopLayout->addWidget(myCancelBtn);
    connect(myCancelBtn, SIGNAL(clicked()), this, SLOT(onCancel()));
  }
}

/*!
  Destructor
*/
SalomeApp_EntityEdit::~SalomeApp_EntityEdit()
{
}

/*!
  Clears edit/combo box
*/
void SalomeApp_EntityEdit::clear()
{
  if (myEdit)
    myEdit->clear();
  if (myCombo)
    myCombo->clear();
}

/*!
  \return current text in edit box or combo box
*/
QString SalomeApp_EntityEdit::getText()
{
  if (myEdit)
    return myEdit->text();
  else if (myCombo)
    return myCombo->currentText();
  else
    return "";
}

/*!
  Sets text
*/
void SalomeApp_EntityEdit::setText(const QString& theText)
{
  myString = theText;
  if (myEdit)
    myEdit->setText(theText);
  if (myCombo) {
    int aFound = myCombo->findItem(theText);
    if (aFound >= 0) {
      myCombo->setCurrentIndex(aFound);
      onTextChanged(theText);
    }
  }
}

/*!
  Adds item in combo box, sets it current if theSetCurrent is true
*/
void SalomeApp_EntityEdit::insertItem(const QString& theValue,
                                bool           theSetCurrent,
                                int            theOrder)
{
  if (myCombo) {
    int aIndexAt = -1;
    if (theOrder == atTop)
      aIndexAt = 0;
    else if (theOrder == atBeforeCurrent && myCombo->count() > 0)
      aIndexAt = myCombo->currentIndex();
    else if (theOrder == atAfterCurrent &&
             myCombo->count() > 0 &&
             myCombo->currentIndex() < myCombo->count()-1)
      aIndexAt = myCombo->currentIndex() + 1;
    myCombo->insertItem(theValue, aIndexAt);
  }
  if (theSetCurrent)
    setText(theValue);
}

/*!
  Adds items in combo box, sets item theCurrent as current
*/
void SalomeApp_EntityEdit::insertList(const QStringList& theList,
                                const int          theCurrent)
{
  if (myCombo)
    myCombo->insertList(theList);
  if (theCurrent >= 0 && theCurrent < (int)theList.count())
    setText(theList[theCurrent]);
}

/*!
  Adds item in combo box, sets it current if theSetCurrent is true
*/
void SalomeApp_EntityEdit::insertItem(const int theValue,
                                bool      theSetCurrent)
{
  if (myCombo) {
    myCombo->insertItem(theValue);
  }
  if (theSetCurrent)
    setText(QString::number(theValue));
}

/*!
  Adds items in combo box, sets item theCurrent as current
*/
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

/*!
  Adds item in combo box, sets it current if theSetCurrent is true
*/
void SalomeApp_EntityEdit::insertItem(const double theValue,
                                bool         theSetCurrent)
{
  if (myCombo) {
    myCombo->insertItem(theValue);
  }
  if (theSetCurrent)
    setText(QString::number(theValue));
}

/*!
  Adds items in combo box, sets item theCurrent as current
*/
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

/*! 
  \return actual widget
*/
QWidget* SalomeApp_EntityEdit::getControl()
{
  if (myEdit)
    return myEdit;
  else if (myCombo)
    return myCombo;
  else
    return 0;
}

/*!
  redirect focus to corresponding widget
*/
void SalomeApp_EntityEdit::setFocus()
{
  if (myEdit) {
    myEdit->setFocus();
    //myEdit->selectAll();
  }
  else if (myCombo && myCombo->isEditable()) {
    myCombo->setFocus();
    //myCombo->lineEdit()->selectAll();
  }
}

/*!
  Sets validator for the control
*/
void SalomeApp_EntityEdit::setValidator(const QValidator* theValidator)
{
  if (myEdit)
    myEdit->setValidator(theValidator);
  if (myCombo)
    myCombo->setValidator(theValidator);
}

/*!
  Event filter for KeyPress event
*/
void SalomeApp_EntityEdit::keyPressEvent( QKeyEvent * e)
{
  if ( (e->key() == Qt::Key_Enter ||
        e->key() == Qt::Key_Return ) )
    onApply();
  else if (e->key() == Qt::Key_Escape)
    onCancel();
}

/*!
  Called when item activated in combo box
*/
void SalomeApp_EntityEdit::onComboActivated(const QString& theText)
{
  onTextChanged(theText);
}

/*!
  Slot, called when text changed in line edit
*/
void SalomeApp_EntityEdit::onTextChanged(const QString& theText)
{
  if (myApplyBtn)
    myApplyBtn->setEnabled(!(theText == myString));
  if (myCancelBtn)
    myCancelBtn->setEnabled(!(theText == myString));
}

/*!
  Slot, called when user presses Cancel button
*/
void SalomeApp_EntityEdit::onCancel()
{
  setText(myString);
  if (myApplyBtn)
    myApplyBtn->setEnabled(false);
  if (myCancelBtn)
    myCancelBtn->setEnabled(false);
  emit escapePressed();
}

/*!
  Slot, called when user presses Apply button
*/
void SalomeApp_EntityEdit::onApply()
{
  myString = getText();
  if (myApplyBtn)
    myApplyBtn->setEnabled(false);
  if (myCancelBtn)
    myCancelBtn->setEnabled(false);
  emit returnPressed();
}

/*!
  Shows/hides buttons
*/
void SalomeApp_EntityEdit::showButtons(bool show)
{
  if (myApplyBtn)
    show ? myApplyBtn->show()  : myApplyBtn->hide();
  if (myCancelBtn)
    show ? myCancelBtn->show() : myCancelBtn->hide();
}

/*!
  Enables/disables data duplication (for combo box)
*/
void SalomeApp_EntityEdit::setDuplicatesEnabled(bool enabled)
{
  if (myCombo)
    myCombo->setDuplicatesEnabled(enabled);
}
