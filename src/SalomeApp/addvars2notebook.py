# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import os
import os.path

def addvars2notebook(filename):
    #
    vars_and_values = []
    contents = []
    #
    directory = os.path.dirname(filename)
    base = os.path.basename(filename)
    prefix = base[:-3] # remove ".py"
    prefix = prefix + '_'
    from os import listdir
    l = listdir(directory)
    for f in l:
        if f.find(prefix) != 0: continue
        if f[-3:] != ".py": continue
        module = f[len(prefix):-3]
        try:
            mod = __import__("addvars2notebook_%s"%(module))
        except ImportError:
            continue
        try:
            func = mod.addvars2notebook
        except AttributeError:
            continue
        fff = os.path.join(directory, f)
        content = func(fff, vars_and_values)
        contents.append([fff, content])
        pass
    #
    if vars_and_values:
        stream = open(filename)
        content = stream.read()
        stream.close()
        fields = content.split("import iparameters")
        if len(fields) == 2:
            begin = fields[0]
            if begin.find("## NoteBook E.A.") >= 0:
                begin = begin.split("## NoteBook E.A.")[0]
                pass
            end = "import iparameters" + fields[1]
            content = begin
            content += "## NoteBook E.A.\n"
            for var, value in vars_and_values:
                content += "notebook.set(%s,%s)\n"%(var.__repr__(), value)
                pass
            content += end
            stream = open(filename, "w")
            stream.write(content)
            stream.close()
            #
            for fff, content in contents:
                stream = open(fff, "w")
                stream.write(content)
                stream.close()
                pass
            pass
        pass
    #
    return

if __name__ == "__main__":
    import sys
    addvars2notebook(sys.argv[1])
    pass
