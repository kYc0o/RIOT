#!/bin/sh

[ -f "build.yml" ] && {
    echo "build.yml exists, exiting."
    exit 1
}

MODULE_NAME=${1:-$(basename $(pwd))}

USES="$(grep -ho '\bMODULE_\w*' *.c | sort -u | sed 's/^MODULE_//' | tr [:upper:] [:lower:])"
DEPENDS="$(cd ${RIOTBASE} ; USEMODULE=${MODULE_NAME} make -f getdep.mk)"

arglist() {
    for i in $2; do
        echo $1=$i
    done
}

laze create --type=module --auto-sources --name=${MODULE_NAME} $(arglist --depends "${DEPENDS}") $(arglist --uses "${USES}")

[ -d "include" ] && {
    {
        echo "    global_vars:"
        echo "        CFLAGS:"
        echo "            - -I$(realpath --relative-to=${RIOTBASE} include)"
    } >> build.yml
}
