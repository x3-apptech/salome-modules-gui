.. _drag_and_drop:

*********************************************************
Implementing Drag and Drop functionality in SALOME module
*********************************************************

.. contents:: Table of Contents

**Drag and Drop** provides a simple visual mechanism to transfer
information between and within applications. 

In some aspects Drag and drop operates similarly to the clipboard copy/cut/paste
mechanism.

Since SALOME GUI is implemented on Qt, the drag and drop functionality support
is provided by means of the corresponding Qt mechanisms.

Currently dragging and dropping of the items can be done within Object browser only,
however this functionality can be extended to other GUI elements as well.

.. _enable_drag_and_drop: 

Enabling drag and drop in SALOME module
=======================================

The Drag and drop functionality is enabled by default in the Object browser. However,
to allow dragging of a data object or dropping data on it, it is necessary to redefine
**isDraggable()** and **isDropAccepted()** methods of the corresponding class, a successor
of the **SUIT_DataObject**. These methods are defined in the base class **SUIT_DataObject**
and default implementation of both functions returns **false**, which prevents dragging and
dropping:

::

	bool SUIT_DataObject::isDraggable() const
	{
	  return false;
	}

	bool SUIT_DataObject::isDropAccepted() const
	{
	  return false;
	}

If your data model is based on the **SUIT_DataObject** and **SUIT_TreeModel**, just
re-implement these functions in your successor data object class and return **true**
when it is needed (for example, depending on the data object type, state, etc).

Another alternative is available if your module is directly inherited from
**LightApp_Module** or **SalomeApp_Module** class (as the majority of existing SALOME modules).
The class **LightApp_Module** (and thus **SalomeApp_Module** also) already provides
high-level API that can be used for enabling drag and drop functionality.

To enable dragging, redefine **isDraggable()** method of your module class. In this method
you can analyze the data object subject to the drag operation and decide if
it is necessary to enable or prevent its dragging:

::

	bool MyModuleGUI::isDraggable( const SUIT_DataObject* what ) const
	{
	  bool result = false;
	  const MyDataObject* obj = dynamic_cast<const MyDataObject*>( what );
	  if ( obj ) {
	    // check if object can be dragged
	    result = <some condition>;
	  }
	  return result;
	}

Note, that you should not invoke here method **isDragEnabled()** of your data object class
(in case if it inherits **LightApp_DataObject** or **SalomeApp_DataObject**), unless you
redefine methods **isDraggable()** and **isDropAccepted()** in your data object class. 
The reason is that the implementation of these methods in **LightApp_DataObject** class
redirects calls to the **LightApp_Module** - be careful to avoid entering endless
recursion loop.

To allow data dropping to an object (the object under the mouse cursor in the
Object browser during the drag operation) redefine **isDropAccepted()** method of your
module class:

::

	bool MyModuleGUI::isDropAccepted( const SUIT_DataObject* where ) const
	{
	  bool result = false;
	  const MyDataObject* obj = dynamic_cast<const MyDataObject*>( where );
	  if ( obj ) {
	    // check if object supports dropping
	    result = <some condition>;
	  }
	  return result;
	}

The caution about avoiding recursive loop mentioned above is also valid for
**isDropAccepted()** method.

.. _handle_data_dropping: 

Handling data dropping
======================

When dragging operation is completed (the data is dropped to an object) the module owning
the item on which the data is dropped is notified by invoking its **dropObjects()** method:

::

	void LightApp_Module::dropObjects( const DataObjectList& what,
	                                   SUIT_DataObject* where,
	                                   const int row,
	                                   Qt::DropAction action )
	{
	}

The default implementation does nothing. However, this method can be redifined in the
successor class and handle the operation properly. The list of dropped
data objects is passed via **what** parameter. The data object on which
the data is dropped is passed via **where** parameter. The parameter **row** specifies in the children list
the position of object where data is dropped; if this parameter is equal to -1, the
data is dropped to the end of the children list. Performed drop action is passed
via **action** parameter; possible values are **Qt::CopyAction** and **Qt::MoveAction**
(other actions are currently unsupported).

The method **dropObjects()** should analyze the parameters and apply
the corresponding actions for rearrangement of the data tree, copying or moving the data items depending on the
operation performed. For example:

::

	void MyModuleGUI::dropObjects( const DataObjectList& what, SUIT_DataObject* where,
	                               const int row, Qt::DropAction action )
	{
	  if ( action != Qt::CopyAction && action != Qt::MoveAction )
	    return; // unsupported action

	  // get parent object
	  MyDataObject* whereObj = dynamic_cast<MyDataObject*>( where );
	  if ( !dataObj ) return; // wrong parent

	  // iterate through all objects being dropped
	  for ( int i = 0; i < what.count(); i++ ) {
	    MyDataObject* whatObj = dynamic_cast<MyDataObject*>( what[i] );
	    if ( !whatObj ) continue;                // skip wrong objects
	    // perform copying or moving
	    copyOrMove( whatObj,                     // data object being copied/moved
	                whereObj,                    // target data object
	                row,                         // index in the target data object
	                action == Qt::CopyAction );  // true if copying is done
	  }
	  // update Object browser
	  getApp()->updateObjectBrowser( false );
	}


In the above code the function **copyOrMove()** performs actual data tree rearrangement.

.. _drag_drop_light_modules: 


Drag and Drop in "light" modules
================================

The data model of the **light** (not having CORBA engine) SALOME module is usually
based on the custom tree of data objects. The general approach is to
inherit a custom data
object class from the **LightApp_DataObject** and a custom data model from the
**LightApp_DataModel** class. The data model class is responsible for building the
appropriate presentation of the data tree in the Object browser.

Thus, the implementation of the drag and drop functionality in a **light** module (more
precisely, the method **dropObjects()** as described above), consists in copying data
entities (by creating new instances of the corresponding data object class) or moving
existing data objects to the new position in a tree. The Object browser will update the
tree representation automatically, as soon as **updateObjectBrowser()** function is called.


.. _drag_drop_full_modules:

Using UseCaseBuilder for Drag and Drop handling in "full" modules
=================================================================

Drag and drop operation requires underlying data model to allow flexible re-arrangement of
the data entities inside the data tree. In a **full** (CORBA engine based) SALOME
module, which data model is usually based on the hierarchy of **SALOMEDS::SObject** entities
provided by the data server functionality, re-arrangement of the data
tree is not a trivial task.

However, SALOME data server (**SALOMEDS**) CORBA module proposes a mechanism that can be used
to customize data tree representation in a simple and flexible way -
:ref:`use_case_builder`.

With use case builder, the **dropObjects()** function can be easily implemented. For example:

::

	// GUI part: process objects dropping
	void MyModuleGUI::dropObjects( const DataObjectList& what, SUIT_DataObject* where,
	                               const int row, Qt::DropAction action )
	{
	  if ( action != Qt::CopyAction && action != Qt::MoveAction )
	    return; // unsupported action

	  // get parent object
	  SalomeApp_DataObject* dataObj = dynamic_cast<SalomeApp_DataObject*>( where );
	  if ( !dataObj ) return; // wrong parent
	  _PTR(SObject) parentObj = dataObj->object();

	  // collect objects being dropped
	  MYMODULE_ORB::object_list_var objects = new MYMODULE_ORB::object_list();
	  objects->length( what.count() );
	  int count = 0;
	  for ( int i = 0; i < what.count(); i++ ) {
	    dataObj = dynamic_cast<SalomeApp_DataObject*>( what[i] );
	    if ( !dataObj ) continue;  // skip wrong objects
	    _PTR(SObject) sobj = dataObj->object();
	    objects[i] = _CAST(SObject, sobj)->GetSObject();
	    count++;
	  }
	  objects->length( count );

	  // call engine function
	  engine()->copyOrMove( objects.in(),                              // what
	                        _CAST(SObject, parentObj)->GetSObject(),   // where
	                        row,                                       // row
	                        action == Qt::CopyAction );                // isCopy

	  // update Object browser
	  getApp()->updateObjectBrowser( false );
	}

	// Engine part: perform actual data copying / moving
	void MyModule::copyOrMove( const MYMODULE_ORB::object_list& what,
	                           SALOMEDS::SObject_ptr where,
	                           CORBA::Long row, CORBA::Boolean isCopy )
	{
	  if ( CORBA::is_nil( where ) ) return; // bad parent

	  SALOMEDS::Study_var study = where->GetStudy();                               // study
	  SALOMEDS::StudyBuilder_var studyBuilder = study->NewBuilder();               // study builder
	  SALOMEDS::UseCaseBuilder_var useCaseBuilder = study->GetUseCaseBuilder();    // use case builder
	  SALOMEDS::SComponent_var father = where->GetFatherComponent();               // father component
	  std::string dataType = father->ComponentDataType();
	  if ( dataType != "MYMODULE" ) return; // not a MYMODULE component
  
	  SALOMEDS::SObject_var objAfter;
	  if ( row >= 0 && useCaseBuilder->HasChildren( where ) ) {
	    // insert at a given row -> find insertion position
	    SALOMEDS::UseCaseIterator_var useCaseIt = useCaseBuilder->GetUseCaseIterator( where );
	    int i;
	    for ( i = 0; i < row && useCaseIt->More(); i++, useCaseIt->Next() );
	    if ( i == row && useCaseIt->More() ) {
	      objAfter = useCaseIt->Value();
	    }
	  }
	  // process all objects in a given list
	  for ( int i = 0; i < what.length(); i++ ) {
	    SALOMEDS::SObject_var sobj = what[i];
	    if ( CORBA::is_nil( sobj ) ) continue; // skip bad object
	    if ( isCopy ) {
	      // copying is performed
	      // get the name of the object
	      CORBA::String_var name = sobj->GetName();
	      // create a new object, as a child of the component object
	      SALOMEDS::SObject_var new_sobj = studyBuilder->NewObject( father );
	      new_sobj->SetAttrString( "AttributeName", name.in() );
	      sobj = new_sobj;
	      // ... perform other necessary data copying like
	      // adding the corresponding attributes or creation
	      // of servant data entities...
	    }
	    // insert the object or its copy to the use case tree
	    if ( !CORBA::is_nil( objAfter ) )
	      useCaseBuilder->InsertBefore( sobj, objAfter ); // insert at a given row
	    else
	      useCaseBuilder->AppendTo( where, sobj );        // append to the
	  end of the list
	  }
	}



