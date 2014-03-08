utfacade
========
This is the utfacade Ubitrack submodule.

Usage
-----
In order to use it, you have to clone the buildenvironment, change to the ubitrack directory and add the utfacade by executing:

    git submodule add https://github.com/schwoere/utfacade.git modules/utfacade

Description
----------
The utfacade contains frontend adapters for ubitrack dataflow networks. It also contains the utConsole.

Dependencies
----------
In addition, this component has to following submodule dependencies which have to be added for successful building:

<table>

  <tr>
    <th>Dependency</th><th>Dependent Components</th><th>optional Dependency</th>
  </tr>
  <tr>
    <td>utDataflow</td><td>utFacade, utComponents</td><td>no</td>
  </tr>
   <tr>
    <td>utVision</td><td>utComponents, ApplicationEndpointsVision</td><td>yes</td>
  </tr>
</table>
