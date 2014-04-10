#!/bin/bash

echo '#!/bin/bash' > job.pbs

echo '#PBS -l nodes='$2':a:student:ppn=8+'$3':b:student:ppn=8' >> job.pbs
echo '#PBS -N '$1 >> job.pbs
echo '#PBS -A '$1 >> job.pbs

echo 'cd ${PBS_O_WORKDIR}' >> job.pbs

echo 'date' >> job.pbs

echo 'HOSTFILE=${PBS_JOBID}.hosts' >> job.pbs
echo 'cat ${PBS_NODEFILE} | sort | uniq > ${HOSTFILE}' >> job.pbs
echo 'NUM_NODES=`cat ${HOSTFILE} | wc -l`; cat ${HOSTFILE}' >> job.pbs

echo 'export OMPI_MCA_btl=self,sm,tcp' >> job.pbs
echo 'export RSH_COMMAND=/usr/bin/ssh' >> job.pbs

echo 'mpirun -hostfile ${HOSTFILE} -npernode 8 -n '$4' ./main '$5 >> job.pbs
