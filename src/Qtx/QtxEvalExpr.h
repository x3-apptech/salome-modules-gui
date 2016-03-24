// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxEvalExpr.h
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#ifndef QTXEVALEXPR_H
#define QTXEVALEXPR_H

#include "Qtx.h"

#include <QList>
#include <QVariant>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QtxEvalSet;
class QtxEvalParser;

class QTX_EXPORT QtxEvalExpr
{
public:
  //! Parsing error type
  typedef enum
  {
    OK,               //!< No errors found
    OperandsNotMatch, //!< Types of arguments are invalid for this operation
    InvalidResult,    //!< Operation cannot find result (for example, division by zero)
    InvalidOperation, //!< Unknown operation
    OperationsNull,   //!< Internal operations pointer of parser is null
    InvalidToken,     //!< Invalid token (neither operation, nor parameter of value)
    CloseExpected,    //!< Closing bracket is expected
    ExcessClose,      //!< Extra closing bracket is found
    BracketsNotMatch, //!< Opening and closing brackets are of different type, e.g. [)
    StackUnderflow,   //!< There are no arguments in the stack for the operation
    ExcessData        //!< The parsing is finished, but there are more then one value in the stack
  } Error;

public:
  QtxEvalExpr( const QString& = QString() );
  QtxEvalExpr( const bool, const QString& = QString() );
  ~QtxEvalExpr();

  QVariant           calculate( const QString& = QString() );

  QString            expression() const;
  void               setExpression( const QString& );

  Error              error() const;
  QtxEvalParser*     parser() const;

  bool               autoDeleteOperationSets() const;
  void               setAutoDeleteOperationSets( const bool );

  QList<QtxEvalSet*> operationSets() const;
  QtxEvalSet*        operationSet( const QString& ) const;
  void               removeOperationSet( QtxEvalSet* );
  void               insertOperationSet( QtxEvalSet*, const int = -1 );

private:
  void               intialize( const bool, const QString& );

private:
  QString            myExpr;
  QtxEvalParser*     myParser;
};

class QTX_EXPORT QtxEvalParser
{
public:
  QtxEvalParser();
  virtual ~QtxEvalParser();

  QVariant           calculate();
  QVariant           calculate( const QString& );
  bool               setExpression( const QString& );

  QList<QtxEvalSet*> operationSets() const;
  QtxEvalSet*        operationSet( const QString& ) const;
  void               removeOperationSet( QtxEvalSet* );
  void               insertOperationSet( QtxEvalSet*, const int = -1 );

  bool               autoDeleteOperationSets() const;
  void               setAutoDeleteOperationSets( const bool );

  virtual void       clearParameters();
  virtual bool       removeParameter( const QString& name );
  virtual QVariant   parameter( const QString& name ) const;
  virtual bool       hasParameter( const QString& name ) const;
  virtual void       setParameter( const QString& name, const QVariant& value );
  QStringList        parameters() const;

  QtxEvalExpr::Error error() const;

  bool               firstInvalid( QString& ) const;
  void               removeInvalids();
  QString            dump() const;

  static QString     toString( const QList<QVariant>& );

protected:
  //! Types of postfix representation elements
  typedef enum
  {
    Value, //!< Value (number, string, etc.)
    Param, //!< Parameter
    Open,  //!< Open bracket
    Close, //!< Close bracket
    Pre,   //!< Unary prefix operation
    Post,  //!< Unary postfix operation
    Binary //!< Binary operation
  } PostfixItemType;

  //! Postfix representation element
  typedef struct
  {
    QVariant          myValue;
    PostfixItemType   myType;
  } PostfixItem;

  typedef QList<PostfixItem>      Postfix;   //!< postfix representation
  typedef QList<QtxEvalSet*>      SetList;   //!< list of operations
  typedef QMap<QString, QVariant> ParamMap;  //!< parameter-to-value map

protected:
  QString            dump( const Postfix& ) const;
  virtual bool       prepare( const QString&, Postfix& );
  virtual bool       setOperationTypes( Postfix& );
  virtual bool       sort( const Postfix&, Postfix&, const QStringList&,
                           const QStringList&, int f = -1, int l = -1 );

  virtual bool       parse( const QString& );
  virtual void       setError( const QtxEvalExpr::Error );

  bool               calculate( const QString&, QVariant&, QVariant& );

  static int         search( const QStringList&, const QString&,
                             int offset, int& matchLen, int& listind );
  static QString     note( const QString& str, int pos, int len );
  static int         globalBrackets( const Postfix&, int, int );

private:
  void               operationList( QStringList& ) const;
  void               bracketsList( QStringList&, bool ) const;
  bool               createValue( const QString&, QVariant& ) const;
  int                priority( const QString&, bool isBin ) const;
  QtxEvalExpr::Error isValid( const QString&,
                              const QVariant::Type, const QVariant::Type ) const;
  QtxEvalExpr::Error calculation( const QString&, QVariant&, QVariant& ) const;

  bool               checkOperations() const;

private:
  SetList            mySets;
  QtxEvalExpr::Error myError;
  ParamMap           myParams;
  Postfix            myPostfix;
  bool               myAutoDel;
};

class QTX_EXPORT QtxEvalSet
{
public:
  QtxEvalSet();
  virtual ~QtxEvalSet();

  virtual QString            name() const = 0;

  virtual void               operationList( QStringList& ) const = 0;

  virtual void               bracketsList( QStringList&, bool open ) const = 0;

  virtual bool               createValue( const QString&, QVariant& ) const;

  virtual int                priority( const QString&, bool isBin ) const = 0;

  virtual QtxEvalExpr::Error isValid( const QString&, const QVariant::Type,
                                      const QVariant::Type ) const = 0;

  virtual QtxEvalExpr::Error calculate( const QString&, QVariant&, QVariant& ) const = 0;
};

class QTX_EXPORT QtxEvalSetBase : public QtxEvalSet
{
public:
  QtxEvalSetBase();
  virtual ~QtxEvalSetBase();

  virtual void               operationList( QStringList& ) const;
  virtual void               bracketsList( QStringList&, bool open ) const;

  virtual QtxEvalExpr::Error isValid( const QString&, const QVariant::Type,
                                      const QVariant::Type ) const;
protected: 
  typedef QList<QVariant::Type> ListOfTypes;

  void                       addTypes( const ListOfTypes& );
  void                       addOperations( const QStringList& );

private:
  QStringList                myOpers;
  ListOfTypes                myTypes;
};

class QTX_EXPORT QtxEvalSetArithmetic : public QtxEvalSetBase
{
public:
  QtxEvalSetArithmetic();
  virtual ~QtxEvalSetArithmetic();

  virtual bool               createValue( const QString&, QVariant& ) const;
  virtual int                priority( const QString&, bool isBin ) const;
  virtual QtxEvalExpr::Error calculate( const QString&, QVariant&, QVariant& ) const;

  static QString             Name();
  virtual QString            name() const;
};

class QTX_EXPORT QtxEvalSetLogic : public QtxEvalSetBase
{
public:
  QtxEvalSetLogic();
  virtual ~QtxEvalSetLogic();

  virtual bool               createValue( const QString&, QVariant& ) const;
  virtual int                priority( const QString&, bool isBin ) const;
  virtual QtxEvalExpr::Error calculate( const QString&, QVariant&, QVariant& ) const;

  static QString             Name();
  virtual QString            name() const;

private:
  int                        intValue( const QVariant& v ) const;
};

class QTX_EXPORT QtxEvalSetMath : public QtxEvalSetBase
{
public:
  QtxEvalSetMath();
  virtual ~QtxEvalSetMath();

  virtual bool               createValue( const QString&, QVariant& ) const;
  virtual int                priority( const QString&, bool isBin ) const;
  virtual QtxEvalExpr::Error calculate( const QString&, QVariant&, QVariant& ) const;

  static QString             Name();
  virtual QString            name() const;
};

class QTX_EXPORT QtxEvalSetString : public QtxEvalSetBase
{
public:
  QtxEvalSetString();
  virtual ~QtxEvalSetString();

  virtual bool               createValue( const QString&, QVariant& ) const;
  virtual int                priority( const QString&, bool isBin ) const;
  virtual QtxEvalExpr::Error calculate( const QString&, QVariant&, QVariant& ) const;

  static QString             Name();
  virtual QString            name() const;
};

class QTX_EXPORT QtxEvalSetSets : public QtxEvalSetBase
{
public:
  typedef QList<QVariant> ValueSet;

public:
  QtxEvalSetSets();
  virtual ~QtxEvalSetSets();

  virtual void               bracketsList( QStringList&, bool open ) const;
  virtual int                priority( const QString&, bool isBin ) const;
  virtual QtxEvalExpr::Error isValid( const QString&, const QVariant::Type,
                                      const QVariant::Type ) const;
  virtual QtxEvalExpr::Error calculate( const QString&, QVariant&, QVariant& ) const;

  static void add( ValueSet&, const QVariant& );
  static void add( ValueSet&, const ValueSet& );
  static void remove( ValueSet&, const QVariant& );
  static void remove( ValueSet&, const ValueSet& );

  static QString             Name();
  virtual QString            name() const;
};

class QTX_EXPORT QtxEvalSetConst : public QtxEvalSet
{
public:
  QtxEvalSetConst();
  virtual ~QtxEvalSetConst();

  static QString             Name();
  virtual QString            name() const;

  virtual bool               createValue( const QString&, QVariant& ) const;

  virtual void               operationList( QStringList& ) const;
  virtual void               bracketsList( QStringList&, bool open ) const;
  virtual int                priority( const QString&, bool isBin ) const;
  virtual QtxEvalExpr::Error isValid( const QString&, const QVariant::Type,
                                      const QVariant::Type ) const;
  virtual QtxEvalExpr::Error calculate( const QString&, QVariant&, QVariant& ) const;
};

#endif // QTXEVALEXPR_H
