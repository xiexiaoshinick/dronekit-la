===========================================
DroneKit Log Analyzer documentation!
===========================================

DroneKit Log Analyzer (DroneKit-LA) is a powerful open source *static analyzer* for ArduPilot logs.

The tool can read and analyze several log formats, including telemetry logs (tlogs), dataflash binary logs (.BIN)
and dataflash text dumps (.log), and output detailed error and warning information in several formats (json, text, summary). 
The output includes detailed information about the tests, including what tests were run, the results (pass, fail, warn), severity, 
and supporting evidence.

DroneKit-LA is also fast, memory efficient and extensible. It supports numerous analyzers 
and has already proved itself capable of quickly analyzing very large logs. 

This documentation contains a lot of useful information including an 
:doc:`introduction <about/about>` to the tool, an overview of each of 
the :doc:`analyzers <reference/analyzers>`, examples and guidance on how you can contribute. 

If you just want to install the tool and start working, jump to the :doc:`guide/getting_started`.


.. toctree::
    :hidden:
    
    About <about/about>
    guide/getting_started
    guide/other_features
    reference/analyzers
    reference/command_line_reference
    contributing/index
    License <about/license>