#!/bin/bash

echo '#!/bin/bash\n\n' > job.pbs

echo '#PBS -l nodes=$2:a:student:ppn=8+$3:b:student:ppn=8\n' >> job.pbs
echo '#PBS -N $1\n' >> job.pbs
echo '#PBS -A $1\n\n' >> job.pbs

echo 'cd ${PBS_O_WORKDIR}\n\n' >> job.pbs

echo 'date\n' >> job.pbs

echo 'HOSTFILE=${PBS_JOBID}.hosts\n' >> job.pbs
echo 'cat ${PBS_NODEFILE} | sort | uniq > ${HOSTFILE}\n' >> job.pbs
echo 'NUM_NODES=`cat ${HOSTFILE} | wc -l`; cat ${HOSTFILE}\n\n' >> job.pbs

echo 'export OMPI_MCA_btl=self,sm,tcp\n' >> job.pbs
echo 'export RSH_COMMAND=/usr/bin/ssh\n\n' >> job.pbs

echo 'mpirun -hostfile ${HOSTFILE} -npernode 8 -n $4 ./main $5\n' >> job.pbs
