
Introduction
============

The Log File Navigator, lnav, is an enhanced log file viewer that
takes advantage of any semantic information that can be gleaned from
the files being viewed, such as timestamps and log levels.  Using this
extra semantic information, lnav can do things like interleaving
messages from different files, generate histograms of messages over
time, and providing hotkeys for navigating through the file.  It is
hoped that these features will allow the user to quickly and
efficiently zero in on problems.

Installation
------------

Check the `downloads page <http://lnav.org/downloads>`_ to see if there are
packages for your operating system.  Compiling from source is just a matter of
doing::

   $ ./configure
   $ make
   $ sudo make install

Viewing Logs
------------

The arguments to **lnav** are simply the log files or directories to be viewed.
For example, to view all of the CUPS logs on your system::

   $ lnav /var/log/cups

The formats of the logs are determined automatically and indexed on-the-fly.
See :ref:`log-formats` for a listing of the predefined formats and how to
define your own.

If no arguments are given, **lnav** will try to open the default syslog file on
your system::

   $ lnav
