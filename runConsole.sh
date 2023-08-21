# ex: ./runConsole.sh ./chiang-instances/COR10-F6-M3-JF50-B4-0.25-0.25-4 pils

clear
echo -e 'Running application...'
echo
cd output
./app $1 $2 $(hostname)
echo
echo -e '# Done!'
echo