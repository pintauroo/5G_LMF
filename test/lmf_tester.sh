#!/bin/sh
# VAR = `sudo docker ps | awk '{if(NR>1) print $NF}'`
# docker inspect --format='{{json .State.Health.Status}}' $VAR

RES_FOLDER=res_final_30threads
BASE_DIR=~/andrea/emulation_results 


stop(){
    echo CLEANING...
    cd ~/andrea/oai-cn5g-fed/docker-compose
    python3 core-network.py --type stop-mini
    docker network rm demo-oai-public-net
}

start(){
    echo START...
    stop
    docker network create --driver=bridge --subnet=192.168.70.128/26 -o "com.docker.network.bridge.name"="demo-oai" demo-oai-public-net
    python3 core-network.py --type start-mini --fqdn no --scenario 2
    sleep 5
}

check_healty(){
    echo HEALT CHECK...
    start
    while [ "$(docker inspect --format='{{json .State.Health}}' oai-amf | jq -r '.Status')" != "healthy" ] \
       || [ "$(docker inspect --format='{{json .State.Health}}' oai-smf | jq -r '.Status')" != "healthy" ] 
    do
        echo NOT HEALTY, Rebooting Docker...
        start
    done
    echo HEALTY...
}

start_jmeter_emulation(){
    echo EMULATING $1 users and $2 requests...
    cd "$BASE_DIR"
    rm *.csv
    docker exec --detach  -it oai-amf bash -c  "/amf-server/build/./amf_server 9080 30 192.168.70.136:80/localize-ue 4 14"
    "$BASE_DIR"/./collect_stats.sh 1>&- 2>&-  &
    # /home/giuseppe/andrea/apache-jmeter-5.5/bin/./jmeter -n -t /home/giuseppe/andrea/localization-LMF/LMF_test-multiple_users.jmx -Jthread-number=$1 -Jloop-number-long-user=300 -Jdelay_between_requests=300 -Jloop-number-medium-user=200 -Jloop-number-short-user=100 -Jramp-up-seconds=10 &
    /home/giuseppe/andrea/apache-jmeter-5.5/bin/./jmeter -n -t /home/giuseppe/andrea/localization-LMF/LMF_test.jmx -Jthread-number=$1 -Jloop-number=$2 -Jport-number=9080 &
    wait $!
    echo JMETER COMPLETE
    ps -ef | grep collect_stats | grep -v grep | awk '{print $2}' | xargs kill
}

start_locust_emulation(){
    echo LOCUST
    cd "$BASE_DIR"
    rm *.csv
    # docker exec --detach  -it oai-amf bash -c  "/amf-server/build/./amf_server_run 9080 100 192.168.70.136:80/localize-ue 3 10"
    "$BASE_DIR"/./collect_stats.sh 1>&- 2>&-  &
    # sleep 5
    # locust -f locustfile.py --csv=example --headless 
    locust -f /home/giuseppe/andrea/locust/locustfile.py --headless --logfile log  
    echo LOCUST COMPLETE
    # /home/giuseppe/andrea/apache-jmeter-5.5/bin/./stoptest.sh    
    # pkill locust

    ps -ef | grep collect_stats | grep -v grep | awk '{print $2}' | xargs kill      
    echo PAUSE...
    sleep 5
}

save_results(){
    echo SAVING RESULTS...
    cd "$BASE_DIR"
    # python3 plot_gen.py
    if [ ! -d "$BASE_DIR"/"$RES_FOLDER" ]
    then 
        mkdir "$BASE_DIR"/"$RES_FOLDER"
    fi
    mkdir  "$BASE_DIR"/"$RES_FOLDER"/"$1"_"$2"-"$3"
    mv *.csv *.log  "$BASE_DIR"/"$RES_FOLDER"/"$1"_"$2"-"$3"
}




while getopts ":cltjs" opt; do
  case $opt in
    c)
      check_healty
      ;;
    l)
      check_healty
      docker exec --detach  -it oai-amf bash -c  "/amf-server/build/./amf_server 9080 200 192.168.70.136:80/localize-ue 4 14"
      cd "$BASE_DIR"
      rm *.csv
      "$BASE_DIR"/./collect_stats.sh 1>&- 2>&-  &
      locust -f /home/giuseppe/andrea/locust/locustfile.py --headless --logfile log &
      wait $!
      ps -ef | grep collect_stats | grep -v grep | awk '{print $2}' | xargs kill      
      mkdir "$BASE_DIR"/"locust_results_final"
      mv *.csv log  "$BASE_DIR"/"locust_results_final"
      ;;
    s)
      stop
      ;;
    t)
      "$BASE_DIR"/./collect_stats.sh 1>&- 2>&-  &
      ;;
    j)
    echo "JMETER EXECUTION"
    for  n_rep in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
        do
            for n_thread in 100 200 300 400 500 600 700 800 900 1000 
            do
                for n_req in  1000
                do
                    echo "$n_thread $n_req $n_rep"    
                    check_healty
                    start_jmeter_emulation $n_thread $n_req
                    save_results $n_thread $n_req $n_rep
                done
            done
        done
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done



echo DONE

exit 0

