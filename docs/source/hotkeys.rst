
Hotkey Reference
================

.. |ks| raw:: html

   <kbd>

.. |ke| raw:: html

   </kbd>

.. raw:: html

   <style>
   kbd {
       padding: 0.1em 0.6em;
       border: 1px solid #ccc;
       font-size: 11px;
       font-family: Arial,Helvetica,sans-serif;
       background-color: #f7f7f7;
       color: #333;
       -moz-box-shadow: 0 1px 0px rgba(0, 0, 0, 0.2),0 0 0 2px #ffffff inset;
       -webkit-box-shadow: 0 1px 0px rgba(0, 0, 0, 0.2),0 0 0 2px #ffffff inset;
       box-shadow: 0 1px 0px rgba(0, 0, 0, 0.2),0 0 0 2px #ffffff inset;
       -moz-border-radius: 3px;
       -webkit-border-radius: 3px;
       border-radius: 3px;
       display: inline-block;
       margin: 0 0.1em 0.1em 0.1em;
       text-shadow: 0 1px 0 #fff;
       line-height: 1.4;
       white-space: nowrap;
   }
   </style>

Spatial Navigation
------------------

.. list-table::
   :header-rows: 1
   :widths: 5 5 5 20

   * - Keypress
     -
     -
     - Command
   * - |ks| Space |ke|
     - |ks| PgDn |ke|
     -
     - Down a page
   * - |ks| b |ke|
     - |ks| Backspace |ke|
     - |ks| PgUp |ke|
     - Up a page
   * - |ks| j |ke|
     - |ks| Return |ke|
     - |ks| ↓ |ke|
     - Down a line
   * - |ks| k |ke|
     - |ks| ↑ |ke|
     -
     - Up a line
   * - |ks| h |ke|
     - |ks| ← |ke|
     -
     - Left half a page
   * - |ks| l |ke|
     - |ks| → |ke|
     -
     - Right half a page
   * - |ks| Home |ke|
     -
     -
     - Top of the view
   * - |ks| End |ke|
     -
     -
     - Bottom of the view
   * - |ks| e |ke|
     - |ks| Shift |ke| + |ks| e |ke|
     -
     - Next/previous error
   * - |ks| w |ke|
     - |ks| Shift |ke| + |ks| w |ke|
     -
     - Next/previous warning
   * - |ks| n |ke|
     - |ks| Shift |ke| + |ks| n |ke|
     -
     - Next/previous search hit
   * - |ks| > |ke|
     - |ks| < |ke|
     -
     - Next/previous search hit (horizontal)
   * - |ks| f |ke|
     - |ks| Shift |ke| + |ks| f |ke|
     -
     - Next/previous file
   * - |ks| u |ke|
     - |ks| Shift |ke| + |ks| u |ke|
     - 
     - Next/previous bookmark
   * - |ks| y |ke|
     - |ks| Shift |ke| + |ks| y |ke|
     -
     - Next/prevous SQL result

Chronological Navigation
------------------------

.. list-table::
   :header-rows: 1
   :widths: 5 5 20

   * - Keypress
     -
     - Command
   * - |ks| o |ke|
     - |ks| Shift |ke| + |ks| o |ke|
     - Forward/backward 60 minutes
   * - |ks| d |ke|
     - |ks| Shift |ke| + |ks| d |ke|
     - Forward/backward 24 hours
   * - |ks| 1 |ke| - |ks| 6 |ke|
     - |ks| Shift |ke| + |ks| 1 |ke| - |ks| 6 |ke|
     - Next/previous n'th ten minute of the hour
   * - |ks| 0 |ke|
     - |ks| Shift |ke| + |ks| 0 |ke|
     - Next/previous day

Bookmarks
---------

.. list-table::
   :header-rows: 1
   :widths: 5 20

   * - Keypress
     - Command
   * - |ks| m |ke|
     - Mark/unmark the top line
   * - |ks| Shift |ke| + |ks| m |ke|
     - Mark/unmark the range of lines from the last marked to the top
   * - |ks| Shift |ke| + |ks| j |ke|
     - Mark/unmark the next line after the previously marked
   * - |ks| Shift |ke| + |ks| k |ke|
     - Mark/unmark the previous line
   * - |ks| c |ke|
     - Copy marked lines to the clipboard
   * - |ks| Shift |ke| + |ks| c |ke|
     - Clear marked lines

Display
-------

.. list-table::
   :header-rows: 1
   :widths: 5 20

   * - Keypress
     - Command
   * - |ks| ? |ke|
     - View/leave builtin help
   * - |ks| q |ke|
     - Return to the previous view/quit
   * - |ks| Shift |ke| + |ks| t |ke|
     - Display elapsed time between lines
   * - |ks| t |ke|
     - Switch to/from the text file view
   * - |ks| i |ke|
     - Switch to/from the histogram view
   * - |ks| Shift |ke| + |ks| i |ke|-
     - Switch to/from the histogram view 
   * - |ks| v |ke|
     - Switch to/from the SQL result view
   * - |ks| Shift |ke| + |ks| v |ke|
     - Switch to/from the SQL result view and move to the corresponding in the
       log_line column
   * - |ks| p |ke|
     - Toggle the display of the log parser results
   * - |ks| Tab |ke|
     - Cycle through colums to graph in the SQL result view

Query
-----

.. list-table::
   :header-rows: 1
   :widths: 5 20

   * - Keypress
     - Command
   * - |ks| / |ke|
     - Search for a regular expression
   * - |ks| ; |ke|
     - Execute an SQL query
   * - |ks| : |ke|
     - Execute an internal command
