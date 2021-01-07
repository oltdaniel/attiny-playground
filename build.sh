#!/usr/bin/bash

# extract arguments
COMMAND=$1
FILE=$2

# check if mccu is given, else set default to attiny25
if [[ -z $MMCU ]]; then
    MMCU=attiny25
fi
# check if fcpu is give, else set default to 1MHz
# default attiny25's from the factory have 1MHz
if [[ -z $FCPU ]]; then
    FCPU=1000000
fi
# check if baud is give, else set default to 19200
if [[ -z $BAUD ]]; then
    BAUD=19200
fi
# check if port is give, else set default to /dev/ttyUSB0
if [[ -z $PORT ]]; then
    PORT=/dev/ttyUSB0
fi

# log helper function with some coloring
log() {
    case $1 in
        "SUC")
            echo -ne "[\e[32mSUC\e[0m]"
            ;;
        "ERR")
            echo -ne "[\e[31mERR\e[0m]"
            ;;
        *)
            echo -ne "[$1]"
            ;;
    esac
    printf ": %s\n" "$2"
}

# compile function taking no parameters
compile() {
    local CFLAGS="-g3"
    mkdir -p build
    avr-gcc $CFLAGS -Os -DF_CPU=$FCPU -mmcu=$MMCU -c src/$FILE.c -o build/$FILE.o -Iinclude
	avr-gcc $CFLAGS -DF_CPU=$FCPU -mmcu=$MMCU -o build/$FILE.elf build/$FILE.o
	avr-objcopy -O ihex build/$FILE.elf build/$FILE.hex
    log "SUC" "Compiled."
}

# flash function taking no parameters
flash() {
    # check if file exists we want to write
    if [[ ! -f build/$FILE.hex ]]; then
        log "ERR" "build/$FILE.hex does not exist."
        return
    fi
    avrdude -b $BAUD -c avrisp -p $MMCU -P $PORT -U flash:w:build/$FILE.hex -F
    log "SUC" "Flashed."
}

erase() {
    if [[ -d build ]]; then
        rm -r build
    fi
    log "SUC" "Erased build files."
}

# parse commands
case $COMMAND in
    "c" | "compile")
        compile
        ;;
    "f" | "flash")
        flash
        ;;
    "e" | "erase")
        erase
        ;;
    "h" | "help")
        echo "This is a attiny project environment."
        echo
        echo "Command format: './build.sh <COMMAND> <FILE>'"
        echo
        echo "Prepend '<SETTING>=<VALUE>' to the command to overwrite the default settings."
        echo "- MMCU: Overwrite the selected chip. Default: attiny25"
        echo "- PORT: Overwrite the flash port. Default: /dev/ttyUSB0"
        echo "- FCPU: Overwrite the chip frequency. Default: 1000000"
        echo "- BAUD: Overwrite the upload speed. Default: 19200"
        echo
        echo "The following commands exist:"
        echo "- 'c' or 'compile': Compile the src/<FILE>.c into build/*."
        echo "- 'f' or 'flash': Flash the build/<FILE>.hex file onto the chip."
        echo "- 'e' or 'erase': Delete the build directory."
        echo "- 'h' or 'help': Print this message."
        ;;
    *)
        log "ERR" "unknown command. for help type '$ ./build.sh h'"
        ;;
esac