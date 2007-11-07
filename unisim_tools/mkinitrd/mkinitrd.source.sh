if test "x$1" = x || test "x$2" = x || test "x$3" = x; then
	echo "Usage: mkinitrd.sh <out file prefix> <size in KB> <directory to copy into image>"
	echo "example: ./mkinitrd.sh foo 1024 bar"
	echo "         Creates a compressed initial ramdisk 'foo.gz' (1 MB uncompressed)."
	echo "         The content of directory 'bar' is transfered into the image before compression"
	exit
fi
dd if=/dev/zero of=$1 count=$2 bs=1024
sudo losetup /dev/loop0 $1
sudo mke2fs /dev/loop0
mkdir ~/mnt || true
sudo mount /dev/loop0 ~/mnt
sudo cp -a $3/* ~/mnt/
sudo umount ~/mnt
sudo losetup -d /dev/loop0
gzip $1

