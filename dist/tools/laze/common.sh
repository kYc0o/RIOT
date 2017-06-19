[ -f "build.yml" ] && {
    echo "build.yml exists, exiting."
    exit 1
}

MODULE_NAME=${1:-$(basename $(pwd))}

arglist() {
    for i in $2; do
        echo $1=$i
    done
}

USES="$(grep -ho '\bMODULE_\w*' *.c | sort -u | sed 's/^MODULE_//' | tr [:upper:] [:lower:])"
