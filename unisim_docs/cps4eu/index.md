# General informations

# Demo

The first demo attempts to show usage of instrumented simulation in full linux environments.
This is especially usefull when dealing with complex frameworks such as tensorflow.

## Yocto

Detailed information for YOCTO is [here](yocto.md)

Deploy the images (rootfs and linux)

    bitbake core-minimal-image

## QEMU

Detailed information about QEMU is [here](qemu.md)):

Launch qemu on the right image and log into root (no password required)

edit `/etc/network/interfaces`

    auto eth0
    iface eth0 inet dhcp

bring up networking
    
    ifup eth0

Create a demo user and log into

    useradd demo
    su - demo

Update pip and install numpy and tensorflow-lite

    python3 -m pip install -U pip
    python3 -m pip install numpy
    python3 -m pip install --index-url https://google-coral.github.io/py-repo/ tflite_runtime

To retrieve demo files start a server on host

    python -m http.server 8000


Then get files from guest

    wget http://10.0.2.2:8000/resnet50_tflite.py


## Tensorflow lite implementation

Since tensorflow 2.5 ARMv8/OpenCL/YOCTO friendliness has been drastically improved.

Full tensorflow installation is still a very long and hard process so we stick with the simplified tensorflow lite Python package.  

Available Python "wheel"s are especially well suited to python 3.8 environments which YOCTO dunfell and gatesgarth provide.

YOCTO dunfell gaves us troubles (see [bug #14374](https://bugzilla.yoctoproject.org/show_bug.cgi?id=14374)) so we now use gatesgarth (EOL should be end of 2021, so will need to move).

Installation is thouroughly [documented](https://www.tensorflow.org/lite/guide/python) with many variants. We choose one that leave the hard work to the YOCTO image creation side.


# Good to know

- sphinx-build (Documentation generator)
- sparse / cgcc (Static Analyser)
- mpathpersist (SCSI related)
- ncursesw (new curses, w stands for wide chars => UTF8)
- capstone: A lightweight multi-platform, multi-architecture disassembly framework
- fdt: Device tree library

## Terminal

With controlling terminal, process is able to tell kernel which
process group is foreground process group (in same session). If there
is a foreground process group for a terminal, we can control the
foreground process group via the terminal, e.g., Ctrl-C/Ctrl-\ to
terminate the foreground process group. (A terminal may have only one
foreground process group, or may not have any. To put it precisely, a
terminal can only be associated with one process session.)

With controlling terminal, even if you already redirect stdin to other
places/files, you still be able to read/write from/to controlling
terminal, the /dev/tty. This special file is a synonym within the
kernel for the controlling terminal of current process. If your
process has no controlling terminal associated, then open this file
will fail. What can you do regarding this file? E.g., some programs
need user to input password before doing something, such as programs
for login or encryption. These programs may prohibit user from
inputting password from stdin, which means even if you redirect their
stdin to a random file, they'll still wait for your type. The reason
is they all open /dev/tty to read.

To sum up, with controlling terminal, kernel is aware of where to
deliver terminal-generated signal and terminal input if they are
expected by someone. That's all.

So it is unnecessary for a process to associate with a controlling
terminal, if it doesn't want to be controlled by any terminal, and
doesn't want to read/write from/to "/dev/tty" (like most daemon
programs). However, a common process launched from within a shell
always have controlling terminal associated, because it is member of
shell session, which has already established a controlling terminal
when shell starts up. (Actually, a random process cannot attach a
terminal as controlling terminal, only session leader process can.)

`cfmakeraw()`

           termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                           | INLCR | IGNCR | ICRNL | IXON);
           termios_p->c_oflag &= ~OPOST;
           termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
           termios_p->c_cflag &= ~(CSIZE | PARENB);
           termios_p->c_cflag |= CS8;


