.. _using_input_widgets_page: 

*******************
Using input widgets
*******************

This section contains some hints that might be useful when working
with SALOME GUI input widgets.

Spin boxes
##########

This type of widgets is used for numeric (integer or floating-point)
data input. Spin boxes are used in standard **SALOME** modules if the
input value has a clearly defined input range (and precision in
case of a floating-point value).

Input precision has a specific semantics in **SALOME** floating-point
spin boxes:

- Positive precision value means that a number in the fixed-point format (corresponding to the "f" format specifier for C printf function) is expected. Positive precision value is the maximum allowed number of digits after the decimal separator.
- Negative precision value means that a number either in the fixed-point or the scientific (exponetial) format is expected. This is similar to the behavior of the "g" format specifier for C printf function. Negative precision value is the maximum allowed number of significant digits in mantissa (note that one digit is always before the decimal separator).

To make the user aware of an input value constraints applied by some
**SALOME** operations, the following basic capabilties are provided by spin boxes:

- The text input manually in a spin box by the user is checked to be a number of valid type (integer or floating-point).
- The text is converted to a number and checked to be within the valid range.
- Additionally, for floating-point data the input text is checked against the precision rules described above.

If the input text in a spin box does not meet the
constraints, the user is shown a message in a
tooltip near the spin box just as he types in it. The tooltip contains
information about the valid data range. For a floating-point input, the
tooltip also contains information about the expected precision. 

In standard **SALOME** modules the precision value can be adjusted through
:ref:`setting_preferences_page` of the correspodning  modules, and the tooltip contains a
reference to the corresponding parameter in the preferences.

On-line documentation for each standard SALOME module contains the
list of user preferences that can be used for tuning floating-point
precision for different types of input quantities.

Spin boxes and SALOME Notebook
##############################

Apart from the numeric input, spin boxes in some **SALOME** modules accept
the names of **Notebook** variables (see :ref:`using_notebook`  
page for more details about **Notebook**).

If a spin box accepts variable names, then it is
additionally checked whether the manually input text represents a variable name. Variable
names should comply with the common naming rules for **Python** variables.
If the input is neither a valid number nor a variable name,
the tooltip informs the user that variable names are also acceptable in this spin box.



