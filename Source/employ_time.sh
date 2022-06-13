#! /bin/bash

mapfile -t ARRAY < <(printf "1\n2\n4\n8\n16\n32\n64\n128\n256\n")
echo "${ARRAY[@]}"

for p in "${ARRAY[@]}"; do
	for n in "${ARRAY[@]}"; do
		echo "p = $p    n = $n"
		if [ "$1" != "" ]; then
			echo "Starting MPI employing"
		else
			echo "Insert arguments: <machinefile> <nProcessors> <executableFile> <rows> <cols>\n"
			exit
		fi
		name=$(echo exec_p"$p"_n"$n".txt)
		{
			rm /home/cpd2021/$2
			scp $2 node01:/home/cpd2021/
			sleep 1
			scp $2 node02:/home/cpd2021/
			sleep 1
			scp $2 node03:/home/cpd2021/
			sleep 1
		} &> /dev/null
		sleep 3 3 3 
		cp $2 /home/cpd2021/
		mpirun --hostfile $1 -np $p /home/cpd2021/$2 $n $n | tee "$name"
	done
done

