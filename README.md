# **Nucleus: For Persistent Memory Native Applications**

_Nucleus_ is an experimental library intended to accelerate building applications that run natively on 
persistent memory (pmem). Persistent Applications have the following characteristics:

* Application data and state is held in persistent memory. 
* Data and state survive application restarts, server restarts, power failures etc.
* No serialisation of data to an external data store is required to ensure persistence. 
* They tend to be extremely fast as they are operating on the memory bus and use byte-level addressing.

Persistent Application development is still in its infancy and there is much to be learned.
However, the rewards we are targeting include:

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
* Applications built on the Nucleus framework have been tested on [Intel&reg; Optane&trade; DC Persistent Memory](https://www.intel.sg/content/www/xa/en/architecture-and-technology/optane-dc-persistent-memory.html).

## Getting started 
Firstly, you do not need actual persistent memory to get started! See further below for how to emulate pmem
for development and testing. By default Nucleus will use regular disk (although its slow). 

### Cross platform support
At present Nucleus favors Linux - specifically Fedora v29+. It's quite likely you can get 
other distributions running with not too much pain.  

Nucleus can also be built on Windows via cmake, vcpkg and Visual Studio. Minimum is Microsoft "Visual Studio 15 2017 Win64". 

### Dependencies requiring building or installing

At present there are these main dependencies:
* [Persistent Memory Development Kit (PMDK)](https://github.com/pmem/pmdk) minimum v1.6
* [NDCTL](https://github.com/pmem/ndctl) minimum v60.1 

On Linux, you may need to build PMDK from sources depending on your distribution age.  

There are utility scripts ```setup-pmdk.sh``` and ```build-pmdk.sh```  in ```utils/``` that can help set up or build these pre-requisites for Fedora.

For Windows, you can install PMDK via vcpkg. See the instructions at the bottom
of [this page](https://github.com/pmem/libpmemobj-cpp).  

### Building the Nucleus "Hello World" app

On Linux this builds Nucleus our "hello world" example app in the ``examples/1-hellow_world`` directory:

```bash
mkdir build
cd build
cmake ..
make
```

The default build type is Debug. Specify build type ``cmake -DCMAKE_BUILD_TYPE=Release ..`` for a Release build.

On Windows, there is a build helper called ```build.cmd``` that can help you build the example with cmake.

### Running
The binary executable will be written to ``./build/bin/nucleus``. 
Run `./nucleus.sh` after confirming Nucleus binary file is in build/bin. This uses the ```nucleus.conf``` file in the base directory.
 
All entries in the conf file can also be specified on the command line, eg:
```bash
./build/bin/nucleus --config_file=nucleus.conf --log_level=trace
```

### Output
Once running you should see the log output on screen. It is also logged into ``./nucleus.log`` by default.
```
[2019-08-01 15:05:21.301] [main] [info] Logging has been initialised with name main and loglevel info and saved to ./nucleus.log
[2019-08-01 15:05:21.301] [main] [info] Built with Nucleus. See https://axomem.io for more info, and follow us on Twitter @axomemio for updates
[2019-08-01 15:05:21.301] [main] [info] Nucleus is starting
[2019-08-01 15:05:21.301] [main] [info] Creating new pool ./nucleus.pmem with layout 'myapp_v0.0.1' and size 536870912
[2019-08-01 15:05:21.378] [main] [info] Pool successfully created.
[2019-08-01 15:05:21.378] [main] [info] AppManager: App initializing after first persistence
[2019-08-01 15:05:21.379] [main] [info] ReST Server configured on port 8080 with 4 threads across 8 CPUs
***
Nucleus is running. Default site is http://localhost:8080/api/v1/ready
Press CTRL-C once to shutdown normally. May require up to 3 presses in abnormal termination
***
```

You can check Nucleus is running by opening ``http://localhost:8080/api/v1/ready`` in your browser. You should see:
```json
{"data":{},
 "response":{"message":"Ready command received successfully","status":200}}
```

Now you can test the application by getting ``http://localhost:8080/api/v1/app/message``: 
```json
{"data":{"value":"Hello World"},"response":{"status":200}}
```

To update the message, you'll need to use an API test tool. We use the free 
[Postman](https://www.getpostman.com/) tool. You'll find a base Postman configuration file in ``app/test``.

PUT the following json to ``http://localhost:8080/api/v1/app/message``:
```json
{"value":"Hello Persistence!"}
```
And you'll receive:
```json
{"response":{"message":"Message value updated 1 time(s) so far","status":200}}
```
Now try restarting the app (CTRL-C will stop it) and checking the value via GET again. If you have real persistent 
memory, you can restart the server and check the value again too!

### Start your own app!

Start by reviewing the code in the app directory. You'll see three files:

#### MyApp.hpp
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

#### MyApp.cpp
This holds application specific code, including the transactions required to create new objects and update data.
We update the persistent variables in a transaction:

```cpp
    pmem::obj::transaction::run(PoolManager::getPoolManager()->getPoolForTransaction(), [&] {
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
We've started putting more examples into the /examples dir. To build these you can edit the 
"CMakeLists.txt" to point to the example you want. Then delete the build directory and rerun cmake. 

### Your own app
You can use this the "Hello World" template as a starting point for your own Persistent Memory native applications! :rocket:

1. Create a directory for your new app. Two options are:
  - Create a directory within the source tree called ``./myapp`` - any files here will be .gitignored and you can even put your own git repository here
  - Create a directory outside the source tree 
2. Update the CMakeLists.txt file in Nucleus to point to the new directory (near the top)
3. Copy one of the examples into your new directory
4. Delete the build directory in Nucleus and rerun the cmake process 

## Real and Emulated Persistent Memory support

By default Nucleus will put the pmem pool file in the current directory. This can be a regular filesystem however 
it will be slow compared to memory, even on very fast SSDs.

If you have real or emulated pmem, ensure it is mounted to the filesystem and then specify the main pool file
in the ```nucleus.conf``` file or command line:

```
pool_main_file=/mnt/pmem0/nucleus.pmem 
```
There is also a utility file ```env.sh``` to help mount pmem on server or development machines. 

For more information on emulating pmem, see 
[Creating Development Environments](https://docs.pmem.io/getting-started-guide/creating-development-environments)

## Documentation
The main guidance at present is this README.md file. Further documentation is coming.  

## How to support the Nucleus project
We'd love to hear from you. Some ways you can assist include:
* Contributions via issues, pull requests, feedback, ideas, documentation updates
* GitHub Sponsors (coming soon)
* Get in contact via https://www.axomem.io

## Keep in touch! 
Please follow us via one or more of your preferred channels:
* Watch us on [GitHub](https://github.com/axomem/nucleus) (perhaps :star: us too!)
* Follow us on [LinkedIn](https://www.linkedin.com/company/axomem/) or [Twitter - @axomemio](https://twitter.com/axomemio)
