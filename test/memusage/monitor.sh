echo $$ > mpid
echo "" > log
while [ 1 == 1 ]
do 
	ps aux | grep minoaserver | grep -v grep >> log
	sleep 1
done
