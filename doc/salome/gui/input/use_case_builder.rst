.. _use_case_builder: 

*************************************************************************************
Customize data tree representation in the Object browser by means of use case builder
*************************************************************************************

.. contents:: Table of Contents

In SALOME, the representation of the data tree in the Object browser for the *full*
(CORBA-based) module is done basing on the study contents as it is supplied by SALOME
data server (SALOMEDS). In contrast to the *light* module which data tree is completely
defined and can be easily attuned by the module specific implementation, *full* module
must publish its data in the CORBA study by means of the corresponding API of SALOME
data server, namely **SALOMEDS::StudyBuilder**.

As soon as data entities are published
in the study, they are shown in the Object browser, in the same order as they appear
in the study tree. Re-arrangement of the data entities with such approach is not a 
trivial task: for example, when copying/moving any data entity at the new position
within the tree, it is necessary to copy all its attributes as well
and to clear (in case of move operation) the data entity at the original position. Also, it is not possible to
make some data items in the tree "invisible" for the user (though it might be useful).

Use case builder provides an alternative and more flexible way for customizing the
data tree representation. It implements another approach to the data tree hierarchy,
based on the tree node attributes. With use case builder it is possible to arrange
and easily re-arrange the data items in the data tree in any appropriate way.

For example, with use case builder it is easy to implement such operations as
:ref:`drag_and_drop` and Copy/Cut/Paste. With use case builder approach
it is not important how data entities are arranged in the study tree, they even may 
lie on the same level - use case builder allows providing custom data tree
representation, completely indepedent on the study data tree itself. It is even possible
to hide some data entities in the tree representation while still keeping them in the
study (to store specific module data).

Object browser automatically checks it the module root data object
contains a tree node attribute and switches to the browsing of the
data tree for such module using the use case
builder. Otherwise, it browses data using an ordinary study tree iterator. Thus, it is
possible to have in the same study some modules based on use case builder approach and
others not using it.

.. _use_case_builder_usage:

Use case builder
================

To obtain a reference to the use case builder, the function **GetUseCaseBuilder()** 
of the **SALOMEDS::Study** interface can be used:

::

	interface Study
	{
 	 // Get reference to the use case builder
	  UseCaseBuilder GetUseCaseBuilder(); 
	};

**SALOMEDS::UseCaseBuilder** interface of the **SALOMEDS CORBA** module provides several
methods that can be used to build a custom data tree. Its API is similar to the API of
**SALOMEDS::StudyBuilder** interface - it operates with terms  *"father object"* and
*"child object"*. In addition, use case builder uses term *"current object"* that is
used as a parent of the children objects added if the parent is not explicitly 
specified.

::

	interface UseCaseBuilder
	{
	  // Set top-level root object of the use case tree as the current one.
	  // This method is usually used to add SComponent items to the top level of the tree
	  boolean SetRootCurrent();
  
 	 // Set the object theObject as the current object of the use case builder
	  boolean SetCurrentObject(in SObject theObject);
  
	  // Append object SObject to the end of children list of the current object
 	 boolean Append(in SObject theObject);
  
	  // Append object SObject to the end of children list of the parent object theFather
	  boolean AppendTo(in SObject theFather, in SObject theObject);
 	 
	  // Insert object theFirst before the object theNext (under the same parent object)
	  boolean InsertBefore(in SObject theFirst, in SObject theNext);
  
	  // Remove object from the use case tree (without removing it from the study)
	  boolean Remove(in SObject theObject);
	  
 	 // Check if the object theObject has children (in the use case tree)
	  boolean HasChildren(in SObject theObject);
	  
	  // Get father object of the given object theObject in the use cases tree
	  SObject GetFather(in SObject theObject);
 	 
	  // Check if given object theObject is added to the use case tree
 	 boolean IsUseCaseNode(in SObject theObject);
 	 
	  // Get the current object of the use case builder
	  SObject GetCurrentObject();
	};


.. _browse_use_case_tree: 

Browsing use case data tree
===========================

Browsing the use case tree can be done by means of the use case iterator, that is
provided by the  **SALOMEDS::UseCaseIterator** interface of the **SALOMEDS CORBA**
module. Access to the use case iterator can be done via **SALOMEDS::UseCaseBuilder**
interface:

::

	interface UseCaseBuilder
	{
	  // Get a reference to the use case iterator and initialize it
	  // by the given object theObject
	  UseCaseIterator GetUseCaseIterator(in SObject theObject);
	};


The API of the **SALOMEDS::UseCaseIterator** interface is similar to the 
**SALOMEDS::ChildIterator**:

::

	interface UseCaseIterator
	{
	  // Activate or reset use case iterator; boolean parameter allLevels
	  // specifies if the iterator should browse recursively on all sub-levels or
	  // on the first sub-level only.
	  void Init(in boolean allLevels);
	  // Check if the iterator can browse to the next item
	  boolean More();
	  // Browse the iterator to the next object
	  void Next();
	  // Get the object currently pointed by the iterator
	  SObject Value();
	};

Typical usage of the **UseCaseIterator** is as follows:

::

	// get use case builder
	SALOMEDS::UseCaseBuilder_var useCaseBuilder = study->GetUseCaseBuilder();

	// get the use case iterator
	SALOMEDS::UseCaseIterator_var iter = useCaseIter->GetUseCaseIterator( sobject.in() );
	// iterate through the sub-items recursively
	for ( useCaseIter->Init( true ); useCaseIter->More(); useCaseIter->Next() ) {
	  SALOMEDS::SObject_var child = useCaseIter->Value();
	  // do something with the child
	  // ...
	  // clean-up
 	 child->UnRegister();
	}
	// clean-up
	useCaseIter->UnRegister();
	useCaseBuilder->UnRegister();

.. _use_case_compatibility:

Remark about compatibility with existing studies
================================================

If you decide to switch your module to the use case builder approach to provide
customization for the data tree representation, you must take care of compatibility
with existing SALOME studies. Basically it means that you have to add
a simple code to  **Load()** (and **LoadASCII()** if necessary) method
of your module, which adds tree node attributes to all data entities
in the data tree of your module. The simplest way to do
this is to iterate through all data items and recursively add them to
the use case builder:

::

	// find component
	SALOMEDS::SComponent_var comp = study->FindComponent( "MYMODULE" );
	// add tree node attributes only if component data is present in the study
	if ( !CORBA::is_nil( comp ) ) {
  	  // get the use case builder
  	  SALOMEDS::UseCaseBuilder_var useCaseBuilder = study->GetUseCaseBuilder();
  	  // check if tree nodes are already set
  	  if ( !useCaseBuilder->IsUseCaseNode( comp.in() ) ) {
   	    // set the current node of the use case builder to the root
    	    useCaseBuilder->SetRootCurrent();
    	    // add component item to the top level of the use case tree
    	    useCaseBuilder->Append( comp.in() );
	    // iterate through all child items recursively
	    SALOMEDS::ChildIterator_var iter = study->NewChildIterator( comp.in() );
	    for ( iter->InitEx( true ); iter->More(); iter->Next() ) {
	      SALOMEDS::SObject_var sobj   = iter->Value();
	      SALOMEDS::SObject_var father = sobj->GetFather();
	      // add an object to the corresponding level in the use case tree
	      useCaseBuilder->AppendTo( father.in(), sobj.in() );
	      // clean up (avoid memory leaks)
	      sobj->UnRegister();
	      father->UnRegister();
	    }
	  }
	  useCaseBuilder->UnRegister(); // clean up
	}

