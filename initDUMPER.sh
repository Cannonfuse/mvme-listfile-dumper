#!/bin/bash

SOURCE=${BASH_ARGV[0]}
CDIR=$(dirname ${SOURCE})
DUMPERSYS=$(cd ${CDIR}/.. > /dev/null;pwd); export DUMPERSYS
LD_LIBRARY_PATH=$DUMPERSYS/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
PATH=$DUMPERSYS/bin:$PATH
export PATH
