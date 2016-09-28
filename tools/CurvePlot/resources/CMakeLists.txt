# Copyright (C) 2010-2015  CEA/DEN, EDF R&D
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
INCLUDE(UseQtExt)

SET(_res
    CURVEPLOT_msg_en.ts
    CURVEPLOT_msg_fr.ts
    CURVEPLOT_msg_ja.ts
)

QT_INSTALL_TS_RESOURCES("${_res}" "${SALOME_CURVEPLOT_INSTALL_RES_DATA}")

SET(dist_salomeres_DATA
  dump_view.png
  fit_all.png
  fit_area.png
  zoom_pan.png
  draw_points.png
  draw_lines.png
  legend.png
  hor_linear.png
  ver_linear.png
  hor_logarithmic.png
  ver_logarithmic.png
  settings.png
  )

FOREACH(f ${dist_salomeres_DATA})
  INSTALL(FILES ${f} DESTINATION ${SALOME_CURVEPLOT_INSTALL_RES_DATA})
ENDFOREACH()
