**************************************************************************************
McMeshTran: A MC Mesh and data Transformation/ Translation/ Transfer tool

McMeshTran provide post-processing interface for MCNP mesh tally, MCNP6 unstructured mesh output and TRIPOLI mesh tally, and multiphics coupling with Fluent, CFX, and ANSYS Workbench. It provide useful functions such as summing, averaging mesh tally, tranforming mesh and interpolate data. 

For any questions related to the use of this software/library you may contact Ulrich Fischer(ulrich.fischer@kit.edu) or, for technical assistance, Yuefeng Qiu (yuefeng.qiu@kit.edu).
**************************************************************************************
********Something to know before going on********

It is not necessary to compile McMeshTran in order to use it, because we provided also precompiled McMeshTran binaries. You can find them in  repository "McMeshTran-Binaries". Compile it only if you cannot find precompiled McMeshTran binaries, or you are going to change the source code. 

This release is tested under Salome_7.4.0. Using other Salome version is not guarantee to work, and might cause error during this process. 

**************************************************************************************
********Compilation and installation on Linux system********


***Installing Salome platform***
-> Download Salome_7.4.0 platform from http://www.salome-platform.org/downloads/previous-versions/salome-v7.4.0 . 

-----> Under the list "Binaries for officially supported Linux platforms", Choose the version which is closest to your OS. 

----->You need to register for downloading Salome. 

-> Extract the zip file into a folder, then go into this folder and run the script "runInstall". A install wizard window will come out. 

-> Click always "Next", and keep the default value if you don't care.

-----> If you like, you can change the installation folder;

-----> If you want to save disk space, choose only these module : KERNEL, GUI, MED, GEOM, SMESH, PARAVIS.

-----> There might be warnings on "cppunit" libraries and so on, it won't affect the use of Salome. 

-> We abbreviate the Salome install folder as $SALOME


***Compiling McMeshTran***

-> Download this package, unzip and rename it as "MCMESHTRAN_SRC_0.1.0", place it under $SALOME.

-> There are a few code you need to check. Open $SALOME/MCMESHTRAN_SRC_0.1.0/idl/MCMESHTRAN.idl, find line 127 and read and instruction. We need to comment out the line "#ifdef WITH_CGNS" and "#endif" if it has not been done. 

-> We need to add MCMESHTRAN package into the Salome environment for compiling it. In the following changes, back-up those file before changing them. 
	
-----> Open $SALOME/build.sh (or $SALOME/build.sh), find line 32, and add "MCMESHTRAN" into the list. At the end it looks like: def_modules="${def_modules} RANDOMIZER SIERPINSKY ATOMIC ATOMGEN ATOMSOLV DOCUMENTATION MCMESHTRAN" 
-----> Add the following environment variables to the end of $SALOME/env_build.sh (if you are using csh, then  $SALOME/env_build.csh)
	
	#------ MCMESHTRAN ------
	export MCMESHTRAN_ROOT_DIR=${INST_ROOT}/MCMESHTRAN_0.1.0
	if [ -n "${ENV_FOR_LAUNCH}" ] ; then
	  if [ "${ENV_FOR_LAUNCH}" = "1" ] ; then
		exportp PATH ${MCMESHTRAN_ROOT_DIR}/bin/salome
		exportp LD_LIBRARY_PATH ${MCMESHTRAN_ROOT_DIR}/lib/salome
		exportp PYTHONPATH ${MCMESHTRAN_ROOT_DIR}/bin/salome:${MCMESHTRAN_ROOT_DIR}/lib/python${PYTHON_VERSION}/site-packages/salome
	  fi
	fi
	##
	#------ MCMESHTRAN_src ------
	export MCMESHTRAN_SRC_DIR=${INST_ROOT}/MCMESHTRAN_SRC_0.1.0

-> Go to $SALOME, run the following command to compile McMeshTran:

	./build.sh MCMESHTRAN
	
-> After the compilation, You can find the binaries in $SALOME/INSTALL/MCMESHTRAN_0.1.0. Copy this folder "MCMESHTRAN_0.1.0" to $SALOME folder. 


***Installing and running McMeshTran***

-> Be sure that your $SALOME/MCMESHTRAN_0.1.0 folder have following folders:

	-> bin
	
	-> lib
	
	-> share
	
	-> adm_local
	
	-> idl
	
	-> include

-> Open $SALOME/KERNEL_7.4.0/salome.sh, add the following environment variables into this file.

	#------ MCMESHTRAN ------
	export MCMESHTRAN_ROOT_DIR=${INST_ROOT}/MCMESHTRAN_0.1.0
	if [ -n "${ENV_FOR_LAUNCH}" ] ; then
	  if [ "${ENV_FOR_LAUNCH}" = "1" ] ; then
		exportp PATH ${MCMESHTRAN_ROOT_DIR}/bin/salome
		exportp LD_LIBRARY_PATH ${MCMESHTRAN_ROOT_DIR}/lib/salome
		exportp PYTHONPATH ${MCMESHTRAN_ROOT_DIR}/bin/salome:${MCMESHTRAN_ROOT_DIR}/lib/python${PYTHON_VERSION}/site-packages/salome
	  fi
	fi
	##
	#------ MCMESHTRAN_src ------
	export MCMESHTRAN_SRC_DIR=${INST_ROOT}/MCMESHTRAN_SRC_0.1.0

-> In your desktop, create a new file "runSalome.sh" and put following text into this file(replacing $SALOME with actual path!!): 

	#!/bin/bash
	source $SALOME/KERNEL_7.4.0/salome.sh
	$SALOME/salome_appli_7.4.0/salome --module=GEOM,SMESH,PARAVIS,MCMESHTRAN

-> Under Desktop, make this file as executable script using this command:

	chmod +x ./runSalome.sh

->You can run McMeshTran with running this script now. 

**************************************************************************************
********Compilation and installation on Windows system********

ATTENTION: In this following step you need to have Visual Studio 2010. Without it or with other version of Visual Studio will failed in following steps. 

***Installing Salome platform***

-> Download Salome_7.4.0 platform from http://www.salome-platform.org/downloads/previous-versions/salome-v7.4.0 . 
	->The version you need is "SALOME SDK self-extracting archive for 64bits Windows". 
	->You need to register for downloading Salome. 

-> Extract the Package into a folder, here brief as $SALOME (be sure to replace it with actual path during installation). 

-> Open cmd.exe from Windows Start menu, change folder to $SALOME\WORK, and execute "compile.bat". Please take a coffee, because it takes hours. When the compilation finished, Salome platform will be ready. 

***Compiling McMeshTran***

-> Download the McMeshTran package, unzip it. 

-> In $SALOME\MODULES, Create a folder "MCMESHTRAN", put the source code inside, and rename the source code folder as "MCMESHTRAN_SRC"

	
-> We need to add McMeshTran into the Salome environment. First make a backup of file $SALOME\WORK\set_env.bat

----->in line 55, add "MCMESHTRAN" into the "list" (add it anywhere inside the parenthesis, separate with at least one whitespace)
	
----->in line 81, add "GUI GEOM MED SMESH PARAVIS MCMESHTRAN" into "env_m_list"(add them inside the parenthesis, separate with at least one whitespace).

-> In file $SALOME\WORK\compile.py, add the following line to line 35 

	all_modules.append('MCMESHTRAN')

-> Open cmd.exe from Windows Start menu, change folder to $SALOME\WORK, and execute command:
 
	compile.bat MCMESHTRAN
	
-> The compilation will finished in a few minutes. 

***Installing and using McMeshTran***

-> To run McMeshTran, start cmd.exe in the Windows Start menu, and run the following command:

----->$SALOME\WORK\run_salome.bat --module=MCMESHTRAN
	
----->If you want to start also geometry, meshing and visualization module, using command: $SALOME\WORK\run_salome.bat --module=GEOM,SMESH,PARAVIS,MCMESHTRAN
	
-> One more easy way to run the program is:

----->right-click $SALOME\WORK\run_salome.bat and "Send to -> Desktop (short-cut)";
	
----->right-click the short-cut link in the Desktop, choose "properties";
	
----->Behind the value of "Target", add " --module=MCMESHTRAN" or "--module=GEOM,SMESH,PARAVIS,MCMESHTRAN"(with a whitespace in the front). Click "OK". Next time you can start McMeshTran with this short-cut link. 



**************************************************************************************
For more information, you can find in our publications.

->Yuefeng Qiu, Lei Lu, Ulrich Fischer, Integrated approach for fusion multi-physics coupled analyses based on hybrid CAD and mesh geometries, Fusion Engineering and Design, Available online 4 July 2015, ISSN 0920-3796, http://dx.doi.org/10.1016/j.fusengdes.2015.06.118.

->Yuefeng Qiu, Peng Lu, Ulrich Fischer, Pavel Pereslavtsev, Szabolcs Kecskes, A generic data translation scheme for the coupling of high-fidelity fusion neutronics and CFD calculations, Fusion Engineering and Design, Volume 89, Issues 7–8, October 2014, Pages 1330-1335, ISSN 0920-3796, http://dx.doi.org/10.1016/j.fusengdes.2014.02.044.


Have fun!







