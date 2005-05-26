#ifndef OCCVIEWER_CREATERESTOREVIEWDLG_H
#define OCCVIEWER_CREATERESTOREVIEWDLG_H

#include "OCCViewer.h"

#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewWindow.h"

#include <QtxDialog.h>
#include <QtxListBox.h> 
#include <SUIT_Application.h>

#include <qmap.h>
#include <qvbox.h>
#include <qdialog.h>
#include <qlistbox.h>
#include <qstringlist.h>

class OCCViewer_ViewPort3d;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_CreateRestoreViewDlg : public QDialog
{
	Q_OBJECT

public:
	OCCViewer_CreateRestoreViewDlg( QWidget*, OCCViewer_Viewer* );
	virtual ~OCCViewer_CreateRestoreViewDlg();

	const viewAspectList&       parameters() const;
	viewAspect                  currentItem() const;
	virtual bool						    eventFilter( QObject*, QEvent* );

public slots:
	void	                      OKpressed();
	void	                      clearList();
	void                        editItemText( QListBoxItem* );
	void	                      changeImage( QListBoxItem* );
	void	                      deleteSelectedItems();

signals:
	void	                      dlgOk();

private:
	int								          myKeyFlag;
	QtxListBox*						      myListBox;
	OCCViewer_ViewPort3d*	      myCurViewPort;
	viewAspect                  myCurrentItem;
	viewAspectList              myParametersMap;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
