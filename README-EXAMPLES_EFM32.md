Contiki Examples EFM32_RF
================

The examples/ directory contains a few examples that will help you get
started with Contiki.

To run the example programs, you need either to be running Linux or FreeBSD (or
any other UNIX-type system), or install Cygwin if you are running Microsoft
Windows [http://cygwin.com](http://cygwin.com). As a minimum you will need to
have the gcc C compiler installed. To run the examples in the 'netsim' target,
you need to have GTK 1.x development libraries installed. These are usually
called 'gtk-devel', 'libgtk1-devel' or similar in your Linux software
installation programs.

compile-platforms/
------------------

A test script that compiles Contiki for a number of platforms and reports any
errors found during the build.



hello-world/
------------

A really simple Contiki program that shows how to write Contiki programs. To
compile and test the program, go into the hello-world directory:

    cd examples/hello-world

Run the 'make' command.

    make

This will compile the hello-world program in the 'native' target.  This causes
the entire Contiki operating system and the hello-world application to be
compiled into a single program that can be run by typing the following command:

    ./hello-world.native

This will print out the following text:

    Contiki initiated, now starting process scheduling
    Hello, world

The program will then appear to hang, and must be stopped by pressing the C key
while holding down the Control key.

border-router/
------------

cd examples/rpl-border-router

ipv6/rpl-udp
------------

cd examples/ipv6/rpl-udp