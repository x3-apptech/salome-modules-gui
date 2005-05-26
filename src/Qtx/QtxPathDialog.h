// File:      QtxPathDialog.h
// Author:    Sergey TELKOV

#ifndef QTXPATHDIALOG_H
#define QTXPATHDIALOG_H

#include "QtxDialog.h"

#include <qmap.h>

class QFrame;
class QLineEdit;
class QPushButton;
class QFileDialog;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxPathDialog : public QtxDialog
{
  Q_OBJECT

protected:
  QtxPathDialog( QWidget* = 0, const bool = true, const bool = false );

public:
  QtxPathDialog( const bool, QWidget* = 0, const bool = true, const bool = false );
  virtual ~QtxPathDialog();

  QString            fileName() const;
  void               setFileName( const QString&, const bool = false );

  QString            filter() const;
  void               setFilter( const QString& );

  virtual void       show();

signals:
  void               fileNameChanged( QString );

protected slots:
  void               validate();

private slots:
  void               onBrowse();
  void               onReturnPressed();
  void               onTextChanged( const QString& );

protected:
  virtual bool       isValid();
  virtual bool       acceptData() const;
  virtual void       fileNameChanged( int, QString );

  QFrame*            optionsFrame();
  QString            fileName( const int ) const;
  void               setFileName( const int, const QString&, const bool = false );

  QLineEdit*         fileEntry( const int ) const;
  QLineEdit*         fileEntry( const int, int& ) const;
  int                createFileEntry( const QString&, const int, const int = -1 );

  int                defaultEntry() const;
  void               setDefaultEntry( const int );

private:
  void               initialize();
  QStringList        prepareFilters() const;
	bool               hasVisibleChildren( QWidget* ) const;
  QStringList        filterWildCards( const QString& ) const;
  QString            autoExtension( const QString&, const QString& = QString::null ) const;

protected:
  enum { OpenFile, SaveFile, OpenDir, SaveDir, NewDir };

private:
  typedef struct { int mode; QLineEdit* edit;
                   QPushButton* btn; QFileDialog* dlg; } FileEntry;
  typedef QMap<int, FileEntry> FileEntryMap;

private:
  QString            myFilter;
  FileEntryMap       myEntries;
  int                myDefault;
  QFrame*            myEntriesFrame;
  QFrame*            myOptionsFrame;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
