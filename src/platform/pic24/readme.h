/** 
\page PIC24_dsPIC33_port The PIC24 and dsPIC33 port
 
\author Bryan A. Jones

\section purpose Purpose

This is a sample project that demonstrates the use of PyMite on the
<a href="http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=2629&param=en533465">PIC24
and dsPIC33 family</a> of microcontrollers.  Any PIC with at least 128 KiB of
program memory and 4 KiB of RAM can run PyMite.


\section details Details

This sample project demonstrates a simple use case of 
python-on-a-chip (p14p) on a microcontroller.  The file 
\ref src/platform/pic24/main.c "main.c" is used to initialize p14p
and indicate the name of the module it should run.  In this case, it
will run the module \ref src/platform/pic24/main.py "main.py".

In the module \ref src/platform/pic24/main.py "main.py", we see that 
the program prints a "Hello world" message, defines and runs an 
initialization function and then executes ipm.


\section building Building the Project

First, install <a href="http://www.cygwin.com">Cygwin</a>, manually
selecting installation of make and python. To build documentation,
install <a href="http://docutils.sourceforge.net">docutils</a> <b>to
the Cygwin Python</b> and
<a href="http://www.doxygen.org">Doxygen</a>. Put the path to the
Cygwin Python before the standard Windows Python, since this project
WILL NOT BUILD with Windows Python. 

Start with a clean PyMite tree.  Edit 
<a href="../../../src/platform/pic24/Makefile">src/platform/pic24/Makefile</a>
near line 14 to set the part number of your PIC.

<a href="../../../src/platform/pic24/Makefile">src/platform/pic24/Makefile</a>
will build the PyMite VM archive as well as the 
sample application

@verbatim
    $ cd src/platform/pic24
    $ make
@endverbatim

As a second alternative, the p14p.mcp project file loaded into the Microchip
MPLAB IDE will build the project. However, you must have the path to Cygwin's
make.exe in your Windows path for this to work.


\section flashing Flashing the Binary

The steps above result in the binary file that need to go on the PIC.
The Makefile is configured for use with the 
<a href="http://www.reesemicro.com/Home/pic24-software-library-collection/pic24-bully-bootloader">Bully
Bootloader</a>. Alternatively, use a PICKit2/3 to program your PIC.

*/