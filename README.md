threadNum:4 dataSize:16384(16kb)

<iframe src=".github/img/line.html" width="100%" height="500px"></iframe>

## asio

```
100
all echo sync:
sum:1655333us
average:16553us
all echo async:
sum:: 37482 us (37.482000 ms)
average:374us

400
all echo sync:
sum:12559570us
average:31398us
all echo async:
sum:: 102964 us (102.964000 ms)
average:257us

700
all echo sync:
sum:44608110us
average:63725us
all echo async:
sum:: 182567 us (182.567000 ms)
average:260us

1000
all echo sync:
sum:81739084us
average:81739us
all echo async:
sum:: 260477 us (260.477000 ms)
average:260us
```

## netpoll-cpp

```
100
all echo sync:
sum:234828us
average:2348us
all echo async:
sum:: 22028 us (22.028000 ms)
average:220us

400
all echo sync:
sum:9595661us
average:23989us
all echo async:
sum:: 92396 us (92.396000 ms)
average:230us

700
all echo sync:
sum:26274292us
average:37534us
all echo async:
sum:: 162593 us (162.593000 ms)
average:232us

1000
all echo sync:
sum:72968403us
average:72968us
all echo async:
sum:: 219975 us (219.975000 ms)
average:219us
```

## netpoll-go

```
100
all echo sync:
sum:367403us
average:3674us
all echo async:
sum:: 22499 us (22.499000 ms)
average:224us

400
all echo sync:
sum:10640202us
average:26600us
all echo async:
sum:: 89207 us (89.207000 ms)
average:223us

700
all echo sync:
sum:27540333us
average:39343us
all echo async:
sum:: 167611 us (167.611000 ms)
average:239us

1000
all echo sync:
sum:71772410us
average:71772us
all echo async:
sum:: 212358 us (212.358000 ms)
average:212us
```

