merger
======

Merges sorted text files using common field.

## Usage ##
```
Usage:
  merger [OPTIONS] LEFT-FILE RIGHT-FILE [OUTPUT]

Allowed options:
  -h [ --help ]               print this help message
  -v [ --version ]            print program version
  -q [ --ignore-quotes ]      no special treatment for quoted text
  -d [ --date ]               compare by date/time
  -D [ --drop-empty ]         discard lines without match in all sources
  -s [ --separator ] arg (=,) set fields separator
  -k [ --key ] arg (=1)       field used for comparison (count starts at 1)
  -H [ --header ] arg (=0)    size of the header (left header will also be copied to the output)
```

## Planned features ##

* merge multiple files at once
* discard specified columns
