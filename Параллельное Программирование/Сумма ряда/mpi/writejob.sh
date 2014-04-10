#!/bin/bash

echo '#!/bin/bash\n\n' > job.pbs

echo '#PBS -l nodes=$2:ppn=8:student\n' >> job.pbs
echo '#PBS -N $1\n' >> job.pbs
echo '#PBS -A $1\n\n' >> job.pbs

echo 'cd ${PBS_O_WORKDIR}\n\n' >> job.pbs

echo 'date\n' >> job.pbs

echo 'HOSTFILE=${PBS_JOBID}.hosts\n' >> job.pbs
echo 'cat ${PBS_NODEFILE} | sort | uniq > ${HOSTFILE}\n' >> job.pbs
echo 'NUM_NODES=`cat ${HOSTFILE} | wc -l`; cat ${HOSTFILE}\n\n' >> job.pbs

echo 'mpirun -r ssh -f ${HOSTFILE} -ppn 8 -n $3  ./main $4\n' >> job.pbs
