
INTERVAL=.5
OUTNAME=filename.txt

update_file() {
  docker stats --no-stream --format "table {{.Name}},{{.CPUPerc}},{{.MemUsage}},{{.NetIO}},{{.BlockIO}},{{.PIDs}}"| grep amf | tee --append amf1.csv &
  docker stats --no-stream --format "table {{.Name}},{{.CPUPerc}},{{.MemUsage}},{{.NetIO}},{{.BlockIO}},{{.PIDs}}"| grep lmf | tee --append lmf1.csv &
#   echo $(date +'%s.%N') | tee --append $OUTNAME;
}

while true;
do
  update_file &
  sleep $INTERVAL;
done