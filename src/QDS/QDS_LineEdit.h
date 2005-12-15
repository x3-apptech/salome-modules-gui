#ifndef QDS_LINEEDIT_H
#define QDS_LINEEDIT_H

#include "QDS_Datum.h"

class QLineEdit;

class QDS_EXPORT QDS_LineEdit : public QDS_Datum
{
  Q_OBJECT

protected:
  class Editor;

public:
  QDS_LineEdit( const QString&, QWidget* = 0, const int = All, const QString& = QString::null );
  virtual ~QDS_LineEdit();

  virtual void         setAlignment( const int, const int = Label );

signals:
  void                 returnPressed();

protected slots:
  virtual void         onParamChanged();

private slots:
  void                 onTextChanged( const QString& );

protected:
  QLineEdit*           lineEdit() const;
  virtual QWidget*     createControl( QWidget* );

  virtual QString      getString() const;
  virtual void         setString( const QString& );

  virtual void         unitSystemChanged( const QString& );
};

#endif
