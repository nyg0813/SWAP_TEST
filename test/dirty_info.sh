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
CACHE_SIZE=${ARRAY_MEMLIST[3]}

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
 SWAP_SIZE=${ARRAY_MEMLIST[15]} 
 SWAP_FREE=${ARRAY_MEMLIST[16]}
 DIRTY_SIZE=${ARRAY_MEMLIST[17]} 
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
 
echo
echo "$(date) / [ CPU Usage ] : ${CPU_USAGE} %"
echo "$(date) / [ Real Memory Usage ] : ${MEM_USIZE} %"
echo "$(date) / [ Root Filesystem Usage ] : ${RF_USAGE}"
echo "$(date) / [ Total Memory ] : ${MEM_TSIZE} KB"
echo "$(date) / [ Free Memory ] : ${MEM_FSIZE} KB"
echo "$(date) / [ Page Cache ] : ${CACHE_SIZE} KB"
echo "$(date) / [ Dirty Page ] : ${DIRTY_SIZE} KB"
echo "$(date) / [ Disk Flush Thread ] : ${PDFLUSH_THREAD}"
echo "$(date) / [ Dirty Ratio LIMIT ] : ${DIRTY_CRITICAL_RATIO}%"
echo "$(date) / [ Dirty Ratio Status ] : ${EXPR2} / 100%"
echo "$(date) / [ Swap Memory Status ] : ${SWAP_USE} KB / ${SWAP_SIZE} KB"
echo "$(date) / [ Open File Count ] : ${OPEN_FILE}"
echo "$(date) / [ Process Total Count ] : ${RUN_PROC}"
echo "$(date) / [ Thread Total Count ] : ${RUN_THREAD}"
echo "$(date) / [ CPU Context Switch ] : ${CONTEXT_SWITCH}"
echo "$(date) / [ System Loadaverage ] : ${LOAD_AVERAGE} (${CPU_CORE} Core System)"
 
if [ ${EXPR_DIFF1} -eq 1 ]
then
 echo "$(date) / [ Dirty Page Status ] : CRITICAL"
 echo
 echo "$(date) / [ Operations Notice ] : Kernel will block all write operations !!!"
elif [ ${EXPR_DIFF2} -eq 1 ]
then
 echo "$(date) / [ Dirty Page Status ] : WARNING"
 echo
else
 echo "$(date) / [ Dirty Page Status ] : NORMAL"
 echo
fi
 
if [ ${LOAD_DIFF} -eq 1 ]
then
 echo "[ Loadaverage Notice ] : Latency time of all process increased !!!"
 echo
fi

echo
