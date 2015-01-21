# Log2esd


**Table of Contents**

- [Introduction](#introduction)
- [Installing](#installing)
- [Restrictive condition](#restrictive-condition)
- [Usage](#usage)
- [Monitor](#monitor)
- [Dependencies](#dependencies)

### Introduction ###

Log2esd is a daemon that writting app's log datas to elasticsearch service;The datas will be read in lines;

### Installing ###

You can install log2esd by sources;

```
$ git clone https://github.com/vpon/log2esd.git
$ cd log2esd
$ ./build_log2esd.sh
$ cd bin
```

The execute file is under directory ./log2esd/bin and copy the file to where you want;

### Restrictive condition ###

Your app's log files must be named in format "YYYYDDMMHHMM.log", and format of datas must be json, If not the daemon will not work correct;

### Usage ###
```
Options:
         -h <192.168.0.1:9200> els service (default: localhost:9200)
         -s <app_logs> directory of app's logs (default: ./log)
         -x <index_prefix> index prefix (default: empty)
         -v <index_divisor> index divisor, must be (2,3,4,6,8,12) (default: 2)
         -n <lines> lines of read once (default: 2)
         -b <line_size> size of line (default: 2048)
         -i <interval> waiting interval (default: 10)
         -w <daemon log> directory of daemon logs (default: /data/logs/log2esd)
         -l <level> level of daemon logs (default: INFO)
         -r <yes/no> remove log file after send all datas (default: yes)
         -d <pid_file> daemonize log2esd
         -H Show this help
```

### Monitor ###
Under the logs directory, have two files(.written_stat.txt & .written_file.txt).The statistic of written datas
saved in file of the first one and the statistic of writting datas saved in file of the second.

### Dependencies ###
* curl-7.39.0

