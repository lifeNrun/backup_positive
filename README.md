Positive release 1.0
==
####Note: 
This branch is currently a work-in-progress, in hopes of writing a high performance 
web server like Nginx.
####Authors
Author: Positive Persist. 1018507406@qq.com<br/>
Copy right 2015,Positive Persist.
####Function
1.GET,MIME<br/>
2.High performace almost like nginx
####Technology
1.Memory pool in managing files.<br/>
2.Multiple IO, Epoll<br/>
3.Thread pool in sending large file.<br/>
4.Sort files by access count in order to premote the access performance.

####Performance
Test by ab2 of Apache.</br>
To Little files, the speed of Positive almost can reach the speed of Nginx.
```
linux-76r3:/home/yuan # ab2 -c 20000 -n 20000 http://192.168.27.125/index.html
This is ApacheBench, Version 2.3 <$Revision: 655654 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 192.168.27.125 (be patient)
Completed 2000 requests
Completed 4000 requests
Completed 6000 requests
Completed 8000 requests
Completed 10000 requests
Completed 12000 requests
Completed 14000 requests
Completed 16000 requests
Completed 18000 requests
Completed 20000 requests
Finished 20000 requests


Server Software:        Positive
Server Hostname:        192.168.27.125
Server Port:            80

Document Path:          /index.html
Document Length:        461 bytes

Concurrency Level:      20000
Time taken for tests:   1.891 seconds
Complete requests:      20000
Failed requests:        0
Write errors:           0
Total transferred:      11140000 bytes
HTML transferred:       9220000 bytes
Requests per second:    10578.38 [#/sec] (mean)
Time per request:       1890.649 [ms] (mean)
Time per request:       0.095 [ms] (mean, across all concurrent requests)
Transfer rate:          5754.06 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:      465  560  52.1    567     697
Processing:   499  598  66.6    579     734
Waiting:      356  550 107.1    556     734
Total:       1117 1157  20.3   1152    1215

Percentage of the requests served within a certain time (ms)
  50%   1152
  66%   1160
  75%   1172
  80%   1178
  90%   1192
  95%   1198
  98%   1202
  99%   1206
 100%   1215 (longest request)
```


