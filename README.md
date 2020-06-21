# **Nucleus: For Persistent Memory Native Applications**

[![Build and Test](https://github.com/axomem/nucleus/workflows/Build%20and%20Test/badge.svg)](https://github.com/axomem/nucleus/actions)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=axomem_nucleus&metric=coverage)](https://sonarcloud.io/dashboard?id=axomem_nucleus)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=axomem_nucleus&metric=bugs)](https://sonarcloud.io/dashboard?id=axomem_nucleus)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=axomem_nucleus&metric=vulnerabilities)](https://sonarcloud.io/dashboard?id=axomem_nucleus)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=axomem_nucleus&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=axomem_nucleus)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=axomem_nucleus&metric=alert_status)](https://sonarcloud.io/dashboard?id=axomem_nucleus)

**Nucleus** helps accelerate building applications that run natively on 
persistent memory (pmem) such as Intel&reg; Optane&trade; DC Persistent Memory. 

_Persistent Applications_ have the following characteristics:

* Application data and state is held in persistent memory. 
* Data and state survive application restarts, server restarts, power failures etc.
* No serialisation of data to disk or other storage medium is needed for persistence. 

Persistent Application development is still relatively new and there is much to be learned.
The rewards we are targeting include:

* **Several orders of magnitude increase in speed**, resulting in less wastage of CPU cycles, less hardware, and less energy.
* **Faster application development cycles**, since there are fewer layers in which different expertise is needed and where things can go wrong
* **Overall lower costs** for building multi-terabyte business applications and analytics solutions.

If you are willing to work on the frontier of transformational technology, read on...   

### How Nucleus helps
Nucleus provides an opinionated set of libraries and a framework to help you build a ReST API Application in C++.
Key features include:

* A framework to initialise your application data and state as persistent objects
* Persistent Memory Pool management to open and close pools
* Pre-integrated ReST API server, Logging system and conf-based configuration
* SystemD unit setup to run apps as a server (daemon) - Windows service in backlog
* Tested on actual [Intel&reg; Optane&trade; DC Persistent Memory](https://www.intel.sg/content/www/xa/en/architecture-and-technology/optane-dc-persistent-memory.html).

## Getting started 
Firstly, you do not need actual persistent memory to get started! See further below for how to emulate pmem
for development and testing. By default Nucleus will use regular disk (although its slow). 

### Cross platform support
At present Nucleus favors Linux - specifically Fedora v31+. It's quite likely you can get 
other recent distributions running with not too much pain.   

Nucleus can also be built on Windows via cmake, vcpkg and Visual Studio. Minimum is Microsoft "Visual Studio 15 2017 Win64". 

### Dependencies requiring installing or building

These are the main dependencies:
* A compiler that supports C++17
* [Persistent Memory Development Kit (PMDK)](https://github.com/pmem/pmdk) minimum v1.6
* [PMDK C++ Bindings](https://github.com/pmem/libpmemobj-cpp) minimum v1.8
* Optional - Python3 for test case support
* Optional - Doxygen for generating documentation

On Linux, you may need to build these from sources depending on your distribution age.

There are utility scripts ```install-pmdk.sh``` and ```build-pmdk.sh```  in ```util/``` that can help 
install (from package manager) or build these pre-requisites for Fedora.

For Windows, you can install PMDK via vcpkg. See the instructions at the bottom
of [this page](https://github.com/pmem/libpmemobj-cpp). Make sure to set your compile options to 64 bit too.

### Building the Nucleus "Hello World" app

On Linux this builds Nucleus with the example apps in the ``examples`` directory:

```shell script
mkdir build
cd build
cmake ..
make
```

The default build type is Debug. Specify build type ``cmake -DCMAKE_BUILD_TYPE=Release ..`` for a Release build.

On Windows, there is a build helper called ```build.cmd``` that can help you build the example with cmake.

### Running
The binaries executable will be written to `./bin`. Run `./1-hello_world` - this will use all default configurations. 

You can also use a conf file such as the example `nucleus.conf`, and these settings can also be specified
on the command line, eg:
```shell script
./bin/1-hello_world --config_file=nucleus.conf --log_level=trace
```

### Output
Once running you should see the log output on screen. It is also logged into ``./1-hello_world.log`` by default.
```
[2020-06-21 09:24:53.140] [main] [info] Logging has been initialised with name main and loglevel info and saved to ./1-hello_world.log
[2020-06-21 09:24:53.140] [main] [info] The Nucleus engine is starting
[2020-06-21 09:24:53.140] [main] [info] Built with Nucleus. See https://axomem.io for more info, and follow us on Twitter @axomemio for updates
[2020-06-21 09:24:53.182] [main] [info] Opening existing pool ./1-hello_world.pmem with layout N7nucleus8examples10helloworld10HelloWorldE__v0
[2020-06-21 09:24:53.238] [main] [info] ReST Server starting at http://localhost:8080 with 4 threads across 4 CPUs
[2020-06-21 09:24:53.244] [main] [info] ***
[2020-06-21 09:24:53.244] [main] [info] Server is running. Default site is http://localhost:8080/api/v1/ready
[2020-06-21 09:24:53.244] [main] [info] Press CTRL-C once to shutdown normally. May require up to 3 presses in abnormal termination
[2020-06-21 09:24:53.244] [main] [info] ***

```

You can check Nucleus is running by opening ``http://localhost:8080/api/v1/ping`` in your browser. You should see:
```json
{"data":{"state":"RUNNING"},"response":{"message":"Ping command received"}}
```

Now you can test the application by getting ``http://localhost:8080/api/v1/app/message``: 
```json
{"data":{"value":"Hello World"}}
```

To update the message, you'll need to use an API test tool. We use the free 
[Postman](https://www.getpostman.com/) tool. You'll find a base Postman configuration file in ``app/test``.

PUT the following json to ``http://localhost:8080/api/v1/app/message``:
```json
{"value":"Hello Persistence!"}
```
And you'll receive:
```json
{"response":{"message":"Message value updated 1 time(s) so far"}}
```
Now try restarting the app (CTRL-C will stop it) and checking the value via GET again. If you have real persistent 
memory, you can restart the server and check the value again too!

### Start your own app!

Start by reviewing the code in the examples/1-hello_world directory. You'll see three files:

#### HelloWorld.hpp
This defines the base class that Nucleus manages on Persistent Memory. The data objects in here will survive application restarts!
Specifically, you'll see these variable - these are used in the API above:
```cpp
  private:
    persistent_ptr<string> p_message;
    p<int> p_update_count;
```
Here you can add any types supported by [libpmemobj-c++](https://github.com/pmem/libpmemobj-cpp), 
including plain C++ types, vectors, arrays, strings and even structs and classes.
You'll need to add code in the .cpp file to create and update these though.

#### HelloWorld.cpp
This holds application specific code, including the transactions required to create new objects and update data.
We update the persistent variables in a transaction:

```cpp
    pmem::obj::transaction::run(pop, [&] {
        p_message->assign(message_value);
        p_update_count++;
    });
```
This ensures all the data is written to memory properly. In the event of a power failure halfway through, the
transaction is automatically rolled back on the next restart of the pool.

See the [PMDK C++ Bindings Documentation](https://pmem.io/pmdk/cpp_obj/) page as a starting point for writing persistent
memory code. It's all C++ with some particular patterns to follow.  

This file also includes the callbacks from the ReST API server to handle the requests.
We use [RESTinio](https://github.com/Stiffstream/restinio) as the ReST
server. Here is the [RESTinio documentation](https://stiffstream.com/en/docs.html).

#### main.cpp
This is the starting point for the app. Main() hands control to Nucleus and it does all the rest.

### Other examples 
We've started putting more examples into the /examples dir. After building they will also be in the `./bin directory`

### Your own app
You can use the "Hello World" template as a starting point for your own Persistent Memory native applications! :rocket:

1. Create a directory for your new app. Two options are:
    - Create a directory within the source tree called ``./myapp`` - any files here will be .gitignored and you can even put your own git repository here
    - Create a directory outside the source tree and include nucleus as a subdirectory. See CMakeLists.txt for more information 
2. Copy one of the examples into your new directory
3. Delete the build directory and rerun the cmake process 

## Real and Emulated Persistent Memory support

By default Nucleus will put the pmem pool file in the current directory. This can be a regular filesystem however 
it will be slow compared to memory, even on very fast SSDs.

If you have real or emulated pmem, ensure it is mounted to the filesystem and then specify the main pool file
in the ```nucleus.conf``` file or command line:

```
pool_main_file=/mnt/pmem0/nucleus.pmem 
```
There is also a utility file `env.sh` to help mount pmem on server or development machines. 

For more information on emulating pmem, see 
[Creating Development Environments](https://docs.pmem.io/getting-started-guide/creating-development-environments)

## Installing as a Daemon (server)

### Linux
Nucleus can run as a Daemon under SystemD using the simple service model. A unit file is provided. 

1. From your build directory run `make install`. This will install the most recently compiled binary to the system.
2. Edit the conf file in `/etc/nucleus/nucleus.config` then run `sudo systemctl daemon-reload` to reload configuration.
3. Run `sudo systemctl start nucleus` to start then `sudo systemctl status nucleus` to check status. 
4. To run Nucleus at startup, run `sudo systemctl enable nucleus`  

### Windows
Windows service installation is in backlog. Vote for it [here](https://github.com/axomem/nucleus/issues/41).

## Documentation
Documentation can be compiled by using `make docs` from a build directory. This will put HTML and man page 
documentation into ./build/docs.  

## How to support the Nucleus project
We'd love to hear from you. Some ways you can assist include:
* Contributions via issues, pull requests, feedback, ideas, documentation updates
* GitHub Sponsors (coming soon)
* Get in contact via https://www.axomem.io

## Keep in touch! 
Please follow us via one or more of your preferred channels:
* Watch us on [GitHub](https://github.com/axomem/nucleus) (perhaps :star: us too!)
* Follow us on [LinkedIn](https://www.linkedin.com/company/axomem/) or [Twitter - \@axomemio](https://twitter.com/axomemio)
