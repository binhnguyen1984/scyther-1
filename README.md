The Scyther-abstraction tool repository
===========================

This README describes the organization of the repository of the Scyther-abstraction
tool for security protocol analysis. The tool integrates a protocol abstraction mechanism with the Scyther verifier in order to speedup the verification tasks. In particular, the tool applies different abstraction techniques to aggressively simplify an input protocol specification before feeding it to Scyther. As the abstract models have simpler message structures and smaller message sizes than their original specifications, the tool enables Scyther to achieve unbounded verification for various protocols which without abstraction already render Scyther timeout or memory exhaustion for even small numbers of threads. For installation and usage instructions of the Scyther tool see:
<http://www.cs.ox.ac.uk/people/cas.cremers/scyther/index.html>.

Installing from source
----------------------

In order to run the tool from a repository checkout, it is required to
compile the C sources into a working binary for the backend.  The
simplest way to achieve this is to run the `build.sh` script in the
`./src` directory. This script compiles a binary version of the tool on
the native platform. Thus, in the Linux case, it should produce
`./src/scyther-abst`. This file is automatically copied to the related
directory under `./gui`, and if successful you can attempt to run
`./gui/scyther-gui.py` to use the graphical user interface.

The build process depends on the following
(Debian/Ubuntu) packages:

  * `cmake`
  * `build-essential`
  * `flex`
  * `bison`
  * `gcc-multilib`

If you are using Ubuntu, installing these may be as simple as running

`sudo apt-get install cmake build-essential flex bison gcc-multilib`

In case you also want to be able to compile Windows binaries from Linux,
you also need:

  * `mingw32`

Note that welcome all contributions, e.g., further protocol models. Just send
us a pull request.


Manual for protocol specification in Scyther
------

The current (incomplete) snapshot of the manual can be found in the following location:

  * [./gui/scyther-manual.pdf](gui/scyther-manual.pdf)


Protocol Models
---------------

The protocol models have the extension `.spdl` and can be found in the following directories:

  * [./gui/Protocols](gui/Protocols), containing the officially released models, and
  * [./testing](testing), containing models currently under development.
  * models: containing experimental models used in our publication POST 2015.

Publications related to Scyther-Abstraction
--------------------------------------------
Binh Thanh Nguyen and Christoph Sprenger. Sound Security Protocol Transformations. POST 2013.
Binh Thanh Nguyen and Christoph Sprenger. Abstractions for Security Protocol Verification. POST 2015.
Binh Thanh Nguyen. Sound Abstractions for Security Protocol Verification. Ph.D. Thesis No. 22674, Department of Computer Science, ETH Zurich, May 2015.
Binh Thanh Nguyen, Christoph Sprenger, and Cas Cremers. Abstractions for Security Protocol Verification. Submitted to JCS, 2015.
