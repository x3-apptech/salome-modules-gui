// File:      QtxListOfOperations.h
// Author:    Alexander SOLOVYOV

#ifndef __QTX_LIST_OF_OPERATIONS_HEADER__
#define __QTX_LIST_OF_OPERATIONS_HEADER__

#include "Qtx.h"
#include "QtxOperations.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif


//================================================================
// Class    : 
// Purpose  : 
//================================================================
class QTX_EXPORT QtxListOfOperations : public QtxOperations
{
public:
    QtxListOfOperations();
    virtual ~QtxListOfOperations();

    virtual void  opersList( QStringList& ) const;
    virtual void  bracketsList( QStringList&, bool open ) const;

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior    ( const QString&, bool isBin ) const;
    virtual QtxParser::Error isValid( const QString&,
                                      const QVariant::Type,
                                      const QVariant::Type ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;

    void clear  ();
    bool has    ( const QString& ) const;
    void append ( const QString&, QtxOperations*, int prior );
    void prepend( const QString&, QtxOperations*, int prior );
    void insert ( const QString&, QtxOperations*, int prior, int pos = -1 );
    void remove ( const QString& );
    int  count  () const;
    QtxOperations* operations( const QString& ) const;

private:
    typedef struct
    {
        QString        myName;
        QtxOperations* myOperations;
        int            myAddPrior;

    } OperationSet;

    typedef QValueList< OperationSet >     OperationSets;
    typedef OperationSets::const_iterator  OperationSetsIterator;

    OperationSets mySets;
};

#endif
