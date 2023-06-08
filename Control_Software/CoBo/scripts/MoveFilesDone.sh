#!/bin/bash

source /home/cherenkov/.bashrc
FILEDIR="/home/cherenkov/Programs/control_software/CoBo/format"
TBDIR="/home/cherenkov/Programs/control_software/tb/mesaflash"
TIMESTAMP=`date "+%b_%d"`
SAVEDATA="/home/cherenkov/Documents/Flight"
DESTDIR="${SAVEDATA}/${TIMESTAMP}/RawDataUnmerged"

sudo mv ${FILEDIR}/*AsAd0* ${DESTDIR}
sudo mv ${FILEDIR}/*AsAd2* ${DESTDIR}
sudo mv ${TBDIR}/TB_data* ${DESTDIR}
