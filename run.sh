# ex: ./run.sh ./chiang-instances/COR10-F6-M3-JF50-B4-0.25-0.25-4 pils

clear
echo -e 'Running application...'
echo
cd output
mkdir -p log
./app $1 $2 $(hostname) &> ./log/$(date +"%Y-%m-%d_%H-%M-%S").log
echo
echo -e '# Done!'
echo
