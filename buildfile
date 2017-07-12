# file      : buildfile
# copyright : Copyright (c) 2013-2017 Code Synthesis Tools CC
# license   : MIT; see accompanying LICENSE file

./: {*/ -build/} doc{INSTALL LICENSE NEWS README version} file{manifest}

doc{version}: file{manifest} # Generated by the version module.
doc{version}: dist = true

# Don't install examples, tests or the INSTALL file.
#
dir{examples/}:  install = false
dir{tests/}:     install = false
doc{INSTALL}@./: install = false
