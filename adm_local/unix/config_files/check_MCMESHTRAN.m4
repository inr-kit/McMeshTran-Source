#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org

AC_DEFUN([CHECK_MCMESHTRAN],[

AC_CHECKING(for MCMESHTRAN)

MCMESHTRAN_ok=no

AC_ARG_WITH(atomic,
	    [  --with-mcmeshtran=DIR root directory path of MCMESHTRAN module installation ],
	    MCMESHTRAN_DIR="$withval",MCMESHTRAN_DIR="")

if test "x$MCMESHTRAN_DIR" == "x" ; then
   # no --with-atomic-dir option used
   if test "x$MCMESHTRAN_ROOT_DIR" != "x" ; then
      # MCMESHTRAN_ROOT_DIR environment variable defined
      MCMESHTRAN_DIR=$MCMESHTRAN_ROOT_DIR
   fi
fi

if test -f ${MCMESHTRAN_DIR}/lib/salome/libMCMESHTRAN.so ; then
   MCMESHTRAN_ok=yes
   AC_MSG_RESULT(Using MCMESHTRAN module distribution in ${MCMESHTRAN_DIR})

   if test "x$MCMESHTRAN_ROOT_DIR" == "x" ; then
      MCMESHTRAN_ROOT_DIR=${MCMESHTRAN_DIR}
   fi
   AC_SUBST(MCMESHTRAN_ROOT_DIR)
else
   AC_MSG_WARN("Cannot find MCMESHTRAN module sources")
fi
  
AC_MSG_RESULT(for MCMESHTRAN: $MCMESHTRAN_ok)
 
])dnl
 
