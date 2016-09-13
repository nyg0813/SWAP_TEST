#!/bin/bash
#Make by Kwang Min Choi / http://blog.helperchoi.com / helperchoi@gmail.com
#TEST OS : CentOS 5.x, 6.x, 7.x

 

export LANG=C
export LC_ALL=C


KERNEL_MAJOR_CHECK=`uname -r | cut -d "-" -f 1 | cut -d "." -f 1`
KERNEL_MINOR_CHECK=`uname -r | cut -d "-" -f 1 | cut -d "." -f 3`

LOG_DIR=/root/collect
LOG_NAME=${LOG_DIR}/`date +%Y-%m-%d`_collect.log

declare -a ARRAY_TIME
declare -a ARRAY_MEMLIST
ARRAY_TIME=("${ARRAY_TIME[@]}" "`date`")
 
for MEM_LIST in `cat /proc/meminfo | awk '{print $2}'`
do
 ARRAY_MEMLIST=("${ARRAY_MEMLIST[@]}" "`echo ${MEM_LIST}`")
done

for Free_Page in `cat /proc/zoneinfo | grep "pages free" | awk '{print $3}'`
do
 FREE_ARRAY=("${FREE_ARRAY[@]}" "`echo ${Free_Page}`")
done
 
for MANAGE_DRAM in `cat /proc/zoneinfo | grep "managed" | awk '{print $2}'`
do
 MANAGED_ARRAY=("${MANAGED_ARRAY[@]}" "`echo ${MANAGE_DRAM}`")
done
 
DIRTY_FLUSH_RATIO=`cat /proc/sys/vm/dirty_background_ratio`
DIRTY_CRITICAL_RATIO=`cat /proc/sys/vm/dirty_ratio`
DIRTY_CRITICAL_LIMIT=`echo "$DIRTY_CRITICAL_RATIO - 5" | bc`
CPU_CORE=`cat /proc/cpuinfo | grep -i process | wc -l`
LOAD_AVERAGE=`cat /proc/loadavg | awk '{print $1}'`
LOAD_AVERAGE_LIMIT=`echo "scale=3; ${CPU_CORE} * 2.5" | bc`
export LOAD_DIFF=`echo "${LOAD_AVERAGE} >= ${LOAD_AVERAGE_LIMIT}" | bc`
CPU_IDLE=`sar -p 1 1 | grep Aver | awk '{print $8}'`
CPU_USAGE=`echo "100 - ${CPU_IDLE}" | bc`
RF_USAGE=`df -hP | awk '$6 ~ /\/$/ {print $5}'`
MEM_TSIZE=${ARRAY_MEMLIST[0]}
MEM_FSIZE=${ARRAY_MEMLIST[1]}

MEM_NVRAM=${FREE_ARRAY[2]} 
MEM_FREE_NVRAM=`echo "${MEM_NVRAM} * 4"  | bc`

MEM_DRAM=`echo "${FREE_ARRAY[0]}+${FREE_ARRAY[1]}" |bc` 
MEM_FREE_DRAM=`echo "${MEM_DRAM} * 4"  | bc`

MANNVRAM=${MANAGED_ARRAY[2]} 
MAN_NVRAM=`echo "${MANNVRAM} * 4"  | bc`

MANDRAM=`echo "${MANAGED_ARRAY[0]}+${MANAGED_ARRAY[1]}" |bc` 
MAN_DRAM=`echo "${MANDRAM} * 4" | bc`

CACHE_SIZE=${ARRAY_MEMLIST[4]}

MEM_USIZE=`echo "((${MEM_TSIZE} - ${MEM_FSIZE} - ${CACHE_SIZE}) * 100) / ${MEM_TSIZE}" | bc`

 

if [ ${KERNEL_MAJOR_CHECK} -eq 2 -a ${KERNEL_MINOR_CHECK} -lt 32 ]
then
 PDFLUSH_THREAD=`ps -ef | grep -v grep | grep flush | wc -l`
 SWAP_SIZE=${ARRAY_MEMLIST[11]} 
 SWAP_FREE=${ARRAY_MEMLIST[12]}
 DIRTY_SIZE=${ARRAY_MEMLIST[13]} 
 CONTEXT_SWITCH=`sar -w 1 1 | awk '$1 ~ /^[0-9]/ {print $2}' 2>&1 | sed -n '2,2p'`
 
elif [ ${KERNEL_MAJOR_CHECK} -eq 2 -a ${KERNEL_MINOR_CHECK} -ge 32 ]
then
 PDFLUSH_THREAD=`ps -ef | grep -v grep | grep flush- | wc -l`
 SWAP_SIZE=${ARRAY_MEMLIST[13]} 
 SWAP_FREE=${ARRAY_MEMLIST[14]}
 DIRTY_SIZE=${ARRAY_MEMLIST[15]} 
 CONTEXT_SWITCH=`sar -w 1 1 | awk '$1 ~ /^[0-9]/ {print $3}' 2>&1 | sed -n '2,2p'`
elif [ ${KERNEL_MAJOR_CHECK} -eq 3 ]
then
 PDFLUSH_THREAD=`ps -ef | grep -v grep | grep kworker | wc -l`
 SWAP_SIZE=${ARRAY_MEMLIST[18]} 
 SWAP_FREE=${ARRAY_MEMLIST[19]}
 DIRTY_SIZE=${ARRAY_MEMLIST[20]} 
 CONTEXT_SWITCH=`sar -w 1 1 | awk '$1 ~ /^[0-9]/ {print $3}' 2>&1 | sed -n '2,2p'`
fi

SWAP_USE=`echo "${SWAP_SIZE} - ${SWAP_FREE}" | bc`
EXPR1=`echo "${DIRTY_SIZE} * 100" | bc`
EXPR2=`echo "scale=2; ${EXPR1} / ${CACHE_SIZE}" | bc`
export EXPR_DIFF1=`echo "scale=3; ${EXPR2} >= ${DIRTY_CRITICAL_RATIO}" | bc`
export EXPR_DIFF2=`echo "scale=3; ${EXPR2} >= ${DIRTY_CRITICAL_LIMIT}" | bc`
OPEN_FILE=`sudo /usr/bin/lsof | wc -l`
RUN_PROC=`ps -ef | wc -l`
RUN_THREAD=`ps -eL | wc -l`

echo "[   Total Memory        ] : ${MEM_TSIZE} KB"
echo "    - Total DRAM Memory   : ${MAN_DRAM} KB"
echo "    - Total NARAM Memory  : ${MAN_NVRAM} KB"
echo "[   Free DRAM Memory    ] : ${MEM_FREE_DRAM} KB"
echo "[   Free NVRAM Memory   ] : ${MEM_FREE_NVRAM} KB"
echo "[   Swap Status         ] : ${SWAP_USE} KB / ${SWAP_SIZE} KB"

echo
