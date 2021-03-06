.. _python_interpreter_page: 

******************
Python Interpreter
******************

**SALOME** Platform can be launched in the batch mode, without Graphical
User Interface, and operated with the use of Python scripts, which can
fulfill most of the necessary tasks, however, the process of scripting
is quite time consuming and rather error-prone.

This problem has been resolved by the possibility to automatically
generate a set of Python scripts from data created with SALOME GUI,
which greatly increases the productivity of using SALOME platform in
the batch mode. This mechanism can convert a SALOME Study in one or
several Python scripts, which can be stored and imported later to
re-create the content of the original study. The first script is a
SALOME document, which re-creates the SALOME Study, adds the
stored Salome components to the SALOME and automatically calls
Python scripts of the second type containing component specific Python
function calls. This architecture gives great flexibility in manual
modification of the generated scripts because you can modify only one
of the component specific Python scripts without touching others, thus
avoiding expert knowledge of Python API of unused components.

To Dump a  SALOME Study in one or several scripts, in the main menu
select **File --> Dump study**.
Reversibly, it is possible to load a saved Python Script selecting in
the main menu **File -> Load Script**.


