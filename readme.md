This project is my computer architecture assignment in XJTU: to implement a pipeline simulator based on MIPS32.

I did this in macOS 12.2.1, so you'd better running this in UNIX like system or bash/wsl in windows(maybe, i hadn't try). 

 To run this project, do these following:

```shell
$ cd src
$ ./run.sh 
```

After these, you are running with the file sample.s by default. If you would like to run with your own assembly program `assembly.s`(but pay a little attention when doing this. Cause I parse the *.s in a extremely naive way, you'd better imate the format in `sample.s` to write a new one with only `add,sub,addi,beqz,lw,sw` instructions and DON'T make any mistakes otherwise you will get WRONG ANSWER. ), just move the file to `src` and do the following:

```shell
$ ./run.sh assembly.s
```

if you type the `./run.sh` and get `permission denied`, you should change the mod to make `./run.sh` executable:

```shell
$ chmod +x ./run.sh
```

Above is all I'd like to say, cause after you enter the process, you can type `help` to get all you can do. Isn't it easy?



But sadly, as you can see, this simulator is so weak that it can only support six instructions... Wish I could improve it someday. :)
