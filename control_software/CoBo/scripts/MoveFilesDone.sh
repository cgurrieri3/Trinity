#!/bin/bash

source /home/trinity/.bashrc
FILEDIR="/home/trinity/Programs/Trinity/control_software/CoBo/format"
TBDIR="/home/trinity/Programs/Trinity/control_software/tb/mesaflash"
TIMESTAMP=`date "+%b_%d"`
SAVEDATA="/home/trinity/Documents/Flight"
DESTDIR="${SAVEDATA}/${TIMESTAMP}/RawDataUnmerged"

sudo mv ${FILEDIR}/*AsAd0* ${DESTDIR}
#sudo mv ${FILEDIR}/*AsAd2* ${DESTDIR}
sudo mv ${TBDIR}/TB_data* ${DESTDIR}
