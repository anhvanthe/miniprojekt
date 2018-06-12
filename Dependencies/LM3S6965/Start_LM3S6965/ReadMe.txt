Readme.txt for LM3S6965 start project.
This project was built for IAR Embedded Workbench for ARM V8.11.1.

Supported hardware:
===================
The sample project for Luminary LM3S6965 is prepared to run on
an Luminary LM3S6965 eval board, but may be used on other target
hardware as well.
Using different target hardware may require modifications.

Configurations
==============
- Debug_Flash:
  This configuration is prepared for download into internal
  Flash using J-Link. An embOS debug and profiling library is
  used.
  To use SEGGER SystemView with this configuration, configure
  SystemViewer for LM3S6965 as target device and SWD at
  2000 kHz as target interface.

- Release_Flash:
  This configuration is prepared for download into internal
  Flash using J-Link. An embOS release library is used.
