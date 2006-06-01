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
// File:      QtxParser.h
// Author:    Alexander SOLOVYOV

#ifndef __QTX_PARSER_HEADER__
#define __QTX_PARSER_HEADER__

#include "Qtx.h"
#include <qvaluestack.h>
#include <qvariant.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif


class QtxOperations;

/*! \var QtxValue
    \brief Alias for QVariant
*/
typedef QVariant QtxValue;


/*!
  \class QtxParser

  This class allows to calculate values of expressions using different set of operations.
  It is provided some of standard set of operations (arithmetics, logic, strings, etc - in QtxStdOperations.h).
  This parser allows to use parameters with help of methods has(), set(), remove(), value(). It uses
  postfix representation of expressions and uses class QtxOperations in order to make certain operation
  Every instance of parser contains only one postfix, so that if expression has been changed, then postfix
  must be rebuilt. In order to increase performance of frequent calculation for many of expressions it is 
  recommended to use different instances of parser for expressions

*/
class QTX_EXPORT QtxParser
{
public:
    /*!
      \enum Error
      \brief Errors during parsing
    */
    typedef enum
    {
        OK,               /*! \var All right */
	OperandsNotMatch, /*! \var Types of arguments are invalid for this operation */
	InvalidResult,    /*! \var Operation cannot find result (for example, division by zero) */
	InvalidOperation, /*! \var Name of operation is unknown */
        OperationsNull,   /*! \var Internal operations pointer of parser is null */
	InvalidToken,     /*! \var It isn't operation, parameter of value  */
	CloseExpected,    /*! \var Close bracket is expected */
	ExcessClose,      /*! \var The one of close bracket is excess */
        BracketsNotMatch, /*! \var Last open and this close bracket are different, for example [) */
	StackUnderflow,   /*! \var There is no arguments in stack for operation */
	ExcessData        /*! \var The parsing is finished, but there is more then one value in stack */

    } Error;

public:
    QtxParser( QtxOperations*, const QString& = QString::null );
    virtual ~QtxParser();

    QtxValue   calculate();
    QtxValue   calculate( const QString& );
    bool       setExpr( const QString& );

    virtual void     clear();
    virtual bool     has   ( const QString& name ) const;
    virtual void     set   ( const QString& name, const QtxValue& value );
    virtual bool     remove( const QString& name );
    virtual QtxValue value ( const QString& name ) const;

    bool       firstInvalid( QString& ) const;
    void       removeInvalids();
    QString    dump() const;
    Error      lastError() const;
    void       paramsList( QStringList& );

    static QString toString( const QValueList< QtxValue >& );

protected:
    /*!
      \enum PostfixItemType
      \brief Types of postfix representation elements
    */  
    typedef enum
    {
      Value, /*! \var Value (number, string, etc.)*/
      Param, /*! \var Parameter */
      Open,  /*! \var Open bracket */
      Close, /*! \var Close bracket */
      Pre,   /*! \var Unary prefix operation */
      Post,  /*! \var Unary postfix operation */
      Binary /*! \var Binary operation */

    } PostfixItemType;

    /*! \var postfix representation element */
    typedef struct
    {
        QtxValue          myValue;
        PostfixItemType   myType;

    } PostfixItem;

    /*! \var postfix representation */
    typedef QValueList< PostfixItem > Postfix;

    /*! \var postfix representation iterator */
    typedef Postfix::const_iterator PostfixIterator;

protected:
            QString  dump( const Postfix& ) const;
    virtual bool     prepare( const QString&, Postfix& );
    virtual bool     setOperationTypes( Postfix& );
    virtual bool     sort( const Postfix&, Postfix&, 
                           const QStringList&, const QStringList&, 
                           int f=-1, int l=-1 );

    virtual bool     parse( const QString& );
    virtual void     setLastError( const Error );

            bool     calculate( const QString&, QtxValue&, QtxValue& );

    static int       search        ( const QStringList&, const QString&, int offset,
                                     int& matchLen, int& listind );
    static QString   note          ( const QString& str, int pos, int len );
    static int       globalBrackets( const Postfix&, int, int );

private:
    /*! \var stack of QtxValues */
    typedef QValueStack < QtxValue >  QtxValueStack;

private:
    QtxOperations*              myOperations;
    QMap< QString, QtxValue >   myParameters;
    Error                       myLastError;
    Postfix                     myPost;
};

#endif
