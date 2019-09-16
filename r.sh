#make clean && make

rm -rf *.out
rm -rf mem.prof*
rm -rf core

killall dotest

ulimit -c unlimited

# 查看内存使用情况
#env HEAP_PROFILE_ALLOCATION_INTERVAL=10485760 HEAP_PROFILE_MMAP=true HEAPPROFILE=./mem.prof ./linux-build/dotest 1>std.out 2>err.out &

# 检查内存泄漏
env HEAPCHECK=normal HEAPPROFILE=./mem.heap  ./linux-build/dotest 1>std.out 2>err.out &

