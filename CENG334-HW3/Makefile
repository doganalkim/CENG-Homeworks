all: c
c:
	g++ -o recext2fs recext2fs.cpp ext2fs_print.c identifier.cpp
e:
	./recext2fs ./testcases/example-bitmap.img 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
d:
	rm -rf ./recext2fs
s:
	e2fsck -fnv ./testcases/example-bitmap.img
m:
	sudo mount -o rw ./testcases2/example-2048-baseline.img fs-root
um:
	sudo umount fs-root
dump:
	dumpe2fs ./testcases/example-bitmap.img
g:
	./grader/grader    ./testcases/example-baseline.img  ./example-bitmap.img  ./testcases/example-bitmap.img 