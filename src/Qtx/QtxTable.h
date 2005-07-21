// File:      QtxTable.h
// Author:    Sergey TELKOV

#ifndef QTX_TABLE_H
#define QTX_TABLE_H

#include "Qtx.h"

#include <qtable.h>

#ifndef QT_NO_TABLE

class QHeader;

class QTX_EXPORT QtxTable : public QTable
{
  Q_OBJECT

  class HeaderEditor;

public:
  QtxTable( QWidget* = 0, const char* = 0 );
  QtxTable( int, int, QWidget* = 0, const char* = 0 );
  virtual ~QtxTable();

  bool             headerEditable( Orientation ) const;

  bool             editHeader( Orientation, const int );
  void             endEditHeader( const bool = true );

  virtual bool     eventFilter( QObject*, QEvent* );

signals:
  void             headerEdited( QHeader*, int );
  void             headerEdited( Orientation, int );

public slots:
  virtual void     hide();
  virtual void     setHeaderEditable( Orientation, bool );

private slots:
  void             onScrollBarMoved( int );
  void             onHeaderSizeChange( int, int, int );

protected:
  virtual void     resizeEvent( QResizeEvent* );

  virtual bool     beginHeaderEdit( Orientation, const int );
  virtual void     endHeaderEdit( const bool = true );
  bool             isHeaderEditing() const;
  virtual QWidget* createHeaderEditor( QHeader*, const int, const bool = true );
  virtual void     setHeaderContentFromEditor( QHeader*, const int, QWidget* );

  QHeader*         header( Orientation o ) const;

private:
  void             updateHeaderEditor();
  void             beginHeaderEdit( Orientation, const QPoint& );
  QRect            headerSectionRect( QHeader*, const int ) const;

private:
  QWidget*         myHeaderEditor;
  QHeader*         myEditedHeader;
  int              myEditedSection;
  QMap<int, bool>  myHeaderEditable;
};

#endif

#endif
