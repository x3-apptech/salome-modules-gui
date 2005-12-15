#ifndef QDS_VALIDATOR_H
#define QDS_VALIDATOR_H

#include "QDS.h"

#include <qvalidator.h>

/*!
    Class: QDS_IntegerValidator
*/

class QDS_EXPORT QDS_IntegerValidator : public QIntValidator
{
public:
  QDS_IntegerValidator( QObject* p = 0 );
  QDS_IntegerValidator( const QString& f, QObject* p = 0 );
  virtual ~QDS_IntegerValidator();

  virtual State validate( QString&, int& ) const;

private:
  QString myFilter;
};

/*!
    Class: QDS_DoubleValidator
*/

class QDS_DoubleValidator : public QDoubleValidator
{
public:
  QDS_DoubleValidator( QObject* p = 0 );
  QDS_DoubleValidator( const QString& f, QObject* p = 0 );
  virtual ~QDS_DoubleValidator();

  virtual State validate( QString&, int& ) const;

private:
  QString myFilter;
};

/*!
    Class: QDS_StringValidator
*/

class QDS_EXPORT QDS_StringValidator : public QValidator
{
public:

  QDS_StringValidator( QObject* p = 0 );
  QDS_StringValidator( const QString& f, QObject* p = 0 );
  QDS_StringValidator( const QString& ft, const QString& fg, QObject* p = 0 );
  virtual ~QDS_StringValidator();

  virtual State validate( QString&, int& ) const;

  int           length() const;
  void          setLength( const int );

private:
  int           myLen;
  QString       myFlags;
  QString       myFilter;
};

#endif
