
.. _log-formats:

Log Formats
===========

Log files are parsed based on formats defined in configuration files.  Many
formats are already built in to the **lnav** binary and you can define your own
using a JSON file.  When loading files, each format is checked to see if it can
parse the first few lines in the file.  Once a match is found, that format will
be considered that files format and used to parse the remaining lines in the
file.  If no match is found, the file is considered to be plain text and can
be viewed in the "text" view that is accessed with the **t** key.

The following log formats are built into **lnav**:

.. csv-table::
   :header: "Name", "Table Name", "Description"
   :widths: 8 5 20
   :file: format-table.csv

Defining a New Format
---------------------

New log formats can be defined by placing JSON configuration files in the
:file:`~/.lnav/formats/` directory.  The files can be named anything you like,
but they must have the '.json' suffix.  A sample file containing the builtin
configuration will be written to this directory when **lnav** starts up.  You
can consult that file when writing your own formats or if you need to modify
existing ones.

The contents of the format configuration should be a JSON object with a field
for each format defined by the file.

The symbolic name of the format.  This value will also be
  used as the SQL table name for the log.

  :title: The short and human-readable name for the format.
  :description: A longer description of the format.
  :url: A URL to the definition of the format.

  :regex:

  :level-field: The name of the regex capture group that contains the log
    message level.

  :level: A mapping of error levels to regular expressions.  During scanning
    the contents of the capture group specified by *level-field* will be
    checked against each of these regexes.  Once a match is found, the log
    message level will set to the corresponding level.  The available levels,
    in order of severity, are: **fatal**, **critical**, **error**,
    **warning**, **info**, **debug**, **trace**.

  :value:

    :kind:  **string**, **integer**, **float**.
    :collate:

Example format::

    {
        "example_log" : {
            "title" : "Example Log Format",
            "description" : "Log format used in the documentation example.",
            "url" : "http://example.com/log-format.html",
            "regex" : {
                "basic" : {
                    "value" : "^"
                }
            },
            "level-field" : "level",
            "level" : {
                "error" : "ERROR",
                "warning" : "WARNING"
            },
            "value" : {
                "component" : {
                    "kind" : "string",
                    "identifier" : true
                }
            },
            "sample" : [
                {
                    "line" : ""
                }
            ]
        }
    }

Modifying an Existing Format
----------------------------
