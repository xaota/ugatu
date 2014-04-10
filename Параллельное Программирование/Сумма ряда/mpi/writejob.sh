#!/bin/bash

echo '#!/bin/bash' > job.pbs

echo '#PBS -l nodes='$2':ppn=8:student' >> job.pbs
echo '#PBS -N '$1 >> job.pbs
echo '#PBS -A '$1 >> job.pbs

echo 'cd ${PBS_O_WORKDIR}' >> job.pbs

echo 'date' >> job.pbs

echo 'HOSTFILE=${PBS_JOBID}.hosts' >> job.pbs
echo 'cat ${PBS_NODEFILE} | sort | uniq > ${HOSTFILE}' >> job.pbs
echo 'NUM_NODES=`cat ${HOSTFILE} | wc -l`; cat ${HOSTFILE}' >> job.pbs

echo 'mpirun -r ssh -f ${HOSTFILE} -ppn 8 -n '$3' ./main '$4 >> job.pbs
