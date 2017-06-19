#!/bin/sh

. ${RIOTBASE}/dist/tools/laze/common.sh

eval "$(RIOTBASE=${RIOTBASE}/dist/tools/laze make all)"

laze create --type=app \
    $(arglist --depends "${DEPENDS}") \
    $(arglist --uses "${USES}") \
    $(arglist --sources "$(ls -Imain.c | grep -E '(\.c|\.cpp)$')")

[ -n "$CFLAGS" ] && {
    {
        echo "    global_vars:"
        echo "        CFLAGS:"
        for var in $CFLAGS; do
            a="$(echo $var | cut -d= -f1)"
            b="$(echo $var | cut -s -d= -f2)"
            [ -n "$b" ] && echo "            - $a=\"$b\"" \
                 || echo "            - $a"
        done
    } >> build.yml
}
