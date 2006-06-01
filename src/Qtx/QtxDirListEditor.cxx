// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "QtxDirListEditor.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qapplication.h>

#define MARGIN_SIZE    11
#define SPACING_SIZE    6
#define SPACER_SIZE     5

static const char* delete_icon[] = {
"16 16 3 1",
"` c #810000",
"  c none",
"# c #ffffff",
"                ",
"                ",
" ``#        ``# ",
" ````#     ``#  ",
"  ````#   ``#   ",
"    ```# `#     ",
"     `````#     ",
"      ```#      ",
"     `````#     ",
"    ```# ``#    ",
"   ```#   ``#   ",
"  ```#     `#   ",
"  ```#      `#  ",
"   `#        `# ",
"                ",
"                "
};

static const char* insert_icon[] = {
"16 16 5 1",
"` c #000000",
". c #ffff00",
"# c #9d9da1",
"  c none",
"b c #ffffff",
"                ",
"                ",
" #  #b #.       ",
"  # #.#.` ` `   ",
"  .#.b####   `  ",
" ### ..         ",
"  . # .#     `  ",
" #` #.          ",
"    #        `  ",
"  `             ",
"             `  ",
"  `             ",
"             `  ",
"  ` ` ` ` ` `   ",
"                ",
"                "
};

static const char* movedown_icon[] = {
"16 16 2 1",
"` c #000000",
"  c none",
"                ",
"                ",
"         ```    ",
"        ```     ",
"       ```      ",
"       ```      ",
"       ```      ",
"       ```      ",
"   ```````````  ",
"    `````````   ",
"     ```````    ",
"      `````     ",
"       ```      ",
"        `       ",
"                ",
"                "
};

static const char* moveup_icon[] = {
"16 16 2 1",
"` c #000000",
"  c none",
"                ",
"                ",
"        `       ",
"       ```      ",
"      `````     ",
"     ```````    ",
"    `````````   ",
"   ```````````  ",
"       ```      ",
"       ```      ",
"       ```      ",
"       ```      ",
"      ```       ",
"     ```        ",
"                ",
"                "
};

/*!
  Constructor
*/
QtxDirListEditor::QtxDirListEditor( QWidget* parent )
: QWidget( parent )
{
  myEdited       = false;
  myLastSelected = 0;
  myEdit         = 0; 
  myBtn          = 0;

  QGridLayout* topLayout = new QGridLayout(this);
  topLayout->setMargin(0);
  topLayout->setSpacing(0);
  
  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
  
  myDirList = new QListBox(this);
  myDirList->setSelectionMode(QListBox::Single);
  myDirList->setHScrollBarMode(QListBox::AlwaysOff);
  myDirList->horizontalScrollBar()->installEventFilter(this);
  myDirList->verticalScrollBar()->installEventFilter(this);
  myDirList->insertItem(tr("<empty>"));
  myDirList->installEventFilter(this);

  QHBoxLayout* ctrlLayout = new QHBoxLayout;
  ctrlLayout->setMargin(0);
  ctrlLayout->setSpacing(0);

  // QLabel* lab = new QLabel(myDirList, tr("DIRECTORIES_LBL"), this);

  QToolButton* insertBtn = new QToolButton(this);
  insertBtn->setIconSet(QPixmap( insert_icon ));
  insertBtn->setAutoRaise(true);

  QToolButton* deleteBtn = new QToolButton(this);
  deleteBtn->setIconSet(QPixmap( delete_icon ));
  deleteBtn->setAutoRaise(true);

  QToolButton* upBtn = new QToolButton(this);
  upBtn->setIconSet(QPixmap( moveup_icon ));
  upBtn->setAutoRaise(true);

  QToolButton* downBtn = new QToolButton(this);
  downBtn->setIconSet(QPixmap( movedown_icon ));
  downBtn->setAutoRaise(true);
  
  // ctrlLayout->addWidget(lab);
  ctrlLayout->addItem(new QSpacerItem(SPACER_SIZE, SPACER_SIZE, QSizePolicy::Expanding, QSizePolicy::Minimum));
  ctrlLayout->addWidget(insertBtn);
  ctrlLayout->addWidget(deleteBtn);
  ctrlLayout->addWidget(upBtn);
  ctrlLayout->addWidget(downBtn);

  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->setMargin(0);
  btnLayout->setSpacing(6);
  
  topLayout->addLayout(ctrlLayout, 0, 0);
  topLayout->addWidget(myDirList,  1, 0);
  topLayout->addLayout(btnLayout,  2, 0);

  connect(myDirList, SIGNAL(mouseButtonClicked(int, QListBoxItem*, const QPoint&)), 
          this, SLOT(onMouseButtonClicked(int, QListBoxItem*, const QPoint&)));
  connect(myDirList, SIGNAL(doubleClicked(QListBoxItem*)), 
          this, SLOT(onDblClicked(QListBoxItem*)));
  
  connect(insertBtn, SIGNAL(clicked()), this, SLOT(onInsert()));
  connect(deleteBtn, SIGNAL(clicked()), this, SLOT(onDelete()));
  connect(upBtn,     SIGNAL(clicked()), this, SLOT(onUp()));
  connect(downBtn,   SIGNAL(clicked()), this, SLOT(onDown()));
}

/*!
  Destructor
*/
QtxDirListEditor::~QtxDirListEditor()
{
}

/*!
  Gets list of paths
*/
void QtxDirListEditor::getPathList(QStringList& list)
{
  // Finish the path editing
  if (myEdit) {
    validate(true);
    
    myEdit->deleteLater();
    myBtn->deleteLater();
    myEdit = 0;
    myBtn  = 0;
    myEdited = false;
    myDirList->setFocus();
  }

  list.clear();
  for (unsigned i = 0; i < myDirList->count()-1; i++)
    list.append(myDirList->text(i));
}

/*!
  Sets list of paths
*/
void QtxDirListEditor::setPathList(const QStringList& list) {
  myDirList->clear();
  myDirList->insertItem(tr("<empty>"));
  for (unsigned i = 0; i < list.count(); i++)
    myDirList->insertItem(list[i], myDirList->count()-1);
}

/*!
  Validates entered path, returns true if OK
*/
bool QtxDirListEditor::validate( const bool quietMode )
{
  if ( myEdited )
  {
    QString dirPath = QFileInfo( myEdit->text().stripWhiteSpace() ).filePath();
/*
#ifndef WNT
    if ( dirPath.startsWith( "~") ) {
      dirPath = dirPath.remove(0,1);
      QString user;
      int slashPos = dirPath.find("/");
      if ( slashPos >= 0 ) {
	user = dirPath.left(slashPos);
	dirPath = dirPath.mid(slashPos);
      }
      else {
	user = dirPath;
	dirPath = "";
      }
      if ( user.isEmpty() )
	user = getenv( "USER" );

      struct passwd* user_data = getpwnam( user.latin1() );
      if ( user_data == NULL ) {
	// unknown user or something another error
	QMessageBox::critical(this, 
			      tr("Error"),
			      tr("Unknown user %1").arg(user), 
			      tr("Ok"));
	myEdit->setFocus();
        return false;
      }
      dirPath = user_data->pw_dir + dirPath;
    }
#endif
*/
    QDir dir(dirPath);
    QListBoxItem* found = 0;
    for (unsigned i = 0; i < myDirList->count()-1; i++) {
      QDir aDir(myDirList->text(i));
      if ( aDir.canonicalPath().isNull() && myDirList->text(i) == dir.absPath() ||
          !aDir.canonicalPath().isNull() && aDir.exists() && aDir.canonicalPath() == dir.canonicalPath()) {
          found = myDirList->item(i);
        break;
      }
    }
    if (dirPath.isEmpty()) {
      if (found) {
        // it should be last (empty) item in the list - nothing to do
        return true;
      }
      else {
        // delete directory from the list
        removeDir(myLastSelected);
        return true;
      }
    }
    else {
      if (found) {
        if (found != myLastSelected) {
          // it is forbidden to add directory more then once
	  if ( !quietMode )
	    QMessageBox::critical(this, 
				  tr("Error"),
				  tr("Directory already specified."), 
				  tr("Ok"));
	  myEdit->setFocus();
          return false;
        }
      }
      else {
        if (!dir.exists()) {
	  if ( !quietMode && QMessageBox::information(this, 
						      tr("Warning"),
						      tr("%1\n\nThe directory doesn't exist.\nAdd directory anyway?").arg(dir.absPath()),
						      tr("Yes"), tr("No"), QString::null, 1, 1) == 1) {
	    myEdit->setFocus();
            return false;
	  }
        }
        // append
        appendDir(myLastSelected, dir.absPath());
      }
    }
  }
  return true;
}

/*!
  Appends/changes directory
*/
void QtxDirListEditor::appendDir(QListBoxItem* item, const QString& dir) {
  int index = myDirList->index(item);
  if (index >= 0 && index < (int)myDirList->count()) {
    if (index == (int)myDirList->count()-1) {
      // it is the last item (new), well, insert it before the last (empty)
      myDirList->insertItem(dir, myDirList->count()-1);
    }
    else {
      // change item
      myDirList->changeItem(dir, index);
    }
  }
}

/*!
  Removes directory from list
*/
void QtxDirListEditor::removeDir(QListBoxItem* item) {
  // do not remove last item (empty)
  int index = myDirList->index(item);
  if (index >= 0 && index < (int)myDirList->count()-1) {
    delete item;
    myLastSelected = myDirList->item(index);
    myDirList->setSelected(myLastSelected, true);
  }
}

/*!
  Resize event
*/
void QtxDirListEditor::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  if ( myEdited ) {
    myEdit->resize(myDirList->viewport()->width()-myBtn->sizeHint().width(), myEdit->height());
    myBtn->move(myEdit->width(), myEdit->y());
  }
}

/*!
  Called when user clicks inside directories list box
*/
void QtxDirListEditor::onMouseButtonClicked(int           button, 
					    QListBoxItem* item, 
					    const QPoint& point) {
  if (myEdited) {
    if (!validate()) {
      myDirList->setCurrentItem(myLastSelected);
      myDirList->setSelected(myLastSelected, true);
      return;
    }
    delete myEdit;
    delete myBtn;
    myEdit = 0;
    myBtn  = 0;
    myEdited = false;
    myDirList->setFocus();
  }
  if (item) {
    myDirList->setCurrentItem(item);
    myDirList->setSelected(item, true);
    myDirList->ensureCurrentVisible();
    qApp->processEvents();
    if (button == LeftButton && myLastSelected == item) {
      QRect ir = myDirList->itemRect(myLastSelected);
      
      myEdit = new QLineEdit(myDirList->viewport());
      myBtn  = new QToolButton(myDirList->viewport());
      myBtn->setText(" ... ");
      connect(myBtn, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
      myEdit->setGeometry(0, 
                          ir.top()-(myEdit->sizeHint().height()-ir.height())/2, 
                          myDirList->viewport()->width()-myBtn->sizeHint().width(), 
                          myEdit->sizeHint().height());
      myBtn->setGeometry (myEdit->width(), 
                          ir.top()-(myEdit->sizeHint().height()-ir.height())/2, 
                          myBtn->sizeHint().width(),
                          myEdit->sizeHint().height());
      connect(myEdit, SIGNAL(returnPressed()), this, SLOT(onEditFinished()));
      myEdited = true;
      myEdit->show();
      myBtn->show();
      if (myDirList->index(myLastSelected) != (int)myDirList->count()-1)
        myEdit->setText(myLastSelected->text());
      myEdit->selectAll();
      myEdit->setCursorPosition(myEdit->text().length());
      myEdit->installEventFilter(this);
      myEdit->setFocus();
    }
  }
  else {
    myDirList->clearSelection();
  }
  myLastSelected = item;
}

/*!
  Called when user double-clicks on any item
*/
void QtxDirListEditor::onDblClicked(QListBoxItem* item) {
  onMouseButtonClicked(LeftButton, item, QPoint(0,0));
}

/*!
  <...> (Browse dir) button slot
*/
void QtxDirListEditor::onBtnClicked() {
  QString dir = myEdit->text().stripWhiteSpace().isEmpty() ? 
                QString::null : 
                myEdit->text().stripWhiteSpace();

  dir = QFileDialog::getExistingDirectory(dir, this, 0, tr("Select directory"), true);

  if (!dir.isEmpty()) {
    myEdit->setText(dir);
    myEdit->selectAll();
    myEdit->setCursorPosition(myEdit->text().length());
  }
}

/*!
  Called when user finises editing of path by pressing <Enter>
*/
void QtxDirListEditor::onEditFinished() {
  if (myEdit) {
    if (!validate()) {
      myDirList->setCurrentItem(myLastSelected);
      myDirList->setSelected(myLastSelected, true);
      return;
    }
    myEdit->deleteLater();
    myBtn->deleteLater();
    myEdit = 0;
    myBtn  = 0;
    myEdited = false;
    myDirList->setFocus();
  }
}

/*!
  Event filter
*/
bool QtxDirListEditor::eventFilter(QObject* object, QEvent* event) {
  if ( myEdited ) {
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
      if (object == myDirList->horizontalScrollBar() || object == myDirList->verticalScrollBar()) {
	if (!validate()) {
	  myDirList->setCurrentItem(myLastSelected);
	  myDirList->setSelected(myLastSelected, true);
	  return true;
	}
	delete myEdit;
	delete myBtn;
	myEdit = 0;
	myBtn  = 0;
	myEdited = false;
	myDirList->setFocus();
      }
    }
    else if (event->type() == QEvent::KeyPress) {
      QKeyEvent* ke = (QKeyEvent*)event;
      if (ke->key() == Key_Tab)
	return true;
      if (object == myDirList) {
	return true;
      }
      else if (object == myEdit) {
        if ( ke->key() == Key_Up || ke->key() == Key_Down || ke->key() == Key_PageUp || ke->key() == Key_PageDown ||
	     ( ke->key() == Key_Home  || ke->key() == Key_End  || ke->key() == Key_Prior || ke->key() == Key_Next ) && 
                 (ke->state() & ControlButton) ) {
          return true;
	}
	else if ( ke->key() == Key_Escape ) {
	  delete myEdit;
	  delete myBtn;
	  myEdit = 0;
	  myBtn  = 0;
	  myEdited = false;
	  myDirList->setFocus();
	  return true;
	}
      }
    }
  }
  return QWidget::eventFilter(object, event);
}

/*!
  <Insert> button slot
*/
void QtxDirListEditor::onInsert() {
  if (!myEdited) {
    myLastSelected = 0;
    onMouseButtonClicked(LeftButton, myDirList->item(myDirList->count()-1), QPoint(0,0));
    onMouseButtonClicked(LeftButton, myDirList->item(myDirList->count()-1), QPoint(0,0));
  }
}

/*!
  <Delete> button slot
*/
void QtxDirListEditor::onDelete() {
  if (!myEdited && myDirList->currentItem() >=0) {
    removeDir(myDirList->item(myDirList->currentItem()));
    myDirList->setFocus();
  }
}

/*!
  <Move up> button slot
*/
void QtxDirListEditor::onUp() {
  if (!myEdited && myLastSelected) {
    int index = myDirList->currentItem();
    if (index > 0 && index < (int)myDirList->count()-1 && myDirList->isSelected(index)) {
      QString t = myDirList->text(index-1);
      myDirList->changeItem(myDirList->text(index), index-1);
      myDirList->changeItem(t, index);
      myDirList->setCurrentItem(index-1);
      myLastSelected = myDirList->item(index-1);
      myDirList->setSelected(myLastSelected, true);
      myDirList->setFocus();
    }
  }
}

/*!
  <Move down> button slot
*/
void QtxDirListEditor::onDown() {
  if (!myEdited && myLastSelected) {
    int index = myDirList->currentItem();
    if (index >= 0 && index < (int)myDirList->count()-2 && myDirList->isSelected(index)) {
      QString t = myDirList->text(index+1);
      myDirList->changeItem(myDirList->text(index), index+1);
      myDirList->changeItem(t, index);
      myDirList->setCurrentItem(index+1);
      myLastSelected = myDirList->item(index+1);
      myDirList->setSelected(myLastSelected, true);
      myDirList->setFocus();
    }
  }
}
