#ifndef SUIT_FILEDIALOG_H
#define SUIT_FILEDIALOG_H

#include "SUIT.h"

#include <qcombobox.h>
#include <qfiledialog.h>

class SUIT_ResourceMgr;
class SUIT_FileValidator;

class SUIT_EXPORT SUIT_FileDlg : public QFileDialog
{
  Q_OBJECT

public:
  SUIT_FileDlg( QWidget*, bool open, bool showQuickDir = true, bool modal = true );
  virtual ~SUIT_FileDlg();

public:    
  bool                isOpenDlg()    const;    
  QString             selectedFile() const;

  void                setValidator( SUIT_FileValidator* );

  static QString      getFileName( QWidget* parent, const QString& initial, const QStringList& filters, 
				                           const QString& caption, bool open, bool showQuickDir = true,
                                   SUIT_FileValidator* validator = 0 );
  static QString      getExistingDirectory( QWidget* parent, const QString& initial,
                                            const QString& caption, bool showQuickDir = true );

private:
  bool                acceptData();
  void                addExtension();
  bool                processPath( const QString& path );

protected slots:
  void                accept();        
  void                reject(); 
  void                quickDir( const QString& );

protected:
  bool                myOpen;             /* open/save selector  */      
  QString             mySelectedFile;     /* selected filename   */    
  SUIT_FileValidator* myValidator;        /* file validator      */
  QComboBox*          myQuickCombo;       /* quick dir combo box */

  static QString      myLastVisitedPath;  /* last visited path   */
};

#endif
