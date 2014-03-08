Python Logic Analyzer

Streaming data analyzer for Saleae logic[1] based on Saleae SDK[2][3].
Currently supports the 8-port version.
Support for other aquisition modules is possible. (Send me some hardware!)

Basic ideas:

- Python's generators are a nice abstraction for working with data streams.

- Most protocols allow for a large data rate reduction in the first
  processing step.  This can be written in C++ while the rest can be
  done easily in Python.

- SWIG makes it easy to write C++ modules that can be used in Python

- CMake allows straightforward cross-platform development

- The Saleae Logic is a neat little device!


This is a work in progress.  Code is carefully designed to make it
modular with minimal dependencies.  An idea is to be able to reuse the
C++ frontend code on a bare-bones microcontroller.


Currently supports:

- C++: Synchronous serial, Asynchronous serial, De-duplication

- Python: all C++ code wrapped as Python generators for ad-hoc
  processing.  Some examples include console dump ASCII, HEX.

- Python: composition of C++ modules

    - Simple Serial chaining

    - Parallel/Serial chaining (RPN dataflow programs)




Planned:

- Cross-platform Qt GUI tools in Python PySide





Links:

[1] http://www.saleae.com/logic

[2] http://downloads.saleae.com/SDK/Saleae%20Device%20SDK.pdf

[3] http://downloads.saleae.com/SDK/SaleaeDeviceSdk-1.1.14.zip

[4] http://www.swig.org/

[5] http://qt-project.org/wiki/PySide
