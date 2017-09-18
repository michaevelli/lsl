## TITLE

`ls -l` command in c
Done not to actually use but to show I can do this

## Usage

<pre>
$ <b>./lsl</b>
total 28
-rw-rw-r-- 1 username username  1214 Sep 18 12:09 README.md
-rwxrwxr-x 1 username username 14264 Sep 18 12:12 lsl
-rw-r--r-- 1 username username  6262 Sep 18 12:08 lsl.c
</pre>
<pre>
$ <b>./lsl</b>
total 48
drwxr-xr-x  2 username username 4096 Feb 28  2017 Desktop
drwxr-xr-x  8 username username 4096 Aug  1 10:00 Documents
drwxr-xr-x  2 username username 4096 Sep 18 11:26 Downloads
drwxr-xr-x  2 username username 4096 Feb 28  2017 Music
drwxr-xr-x  3 username username 4096 Aug  8 22:00 Pictures
drwxr-xr-x  2 username username 4096 Feb 28  2017 Public
drwxr-xr-x  2 username username 4096 Feb 28  2017 Templates
drwxr-xr-x  2 username username 4096 Feb 28  2017 Videos
-rw-r--r--  1 username username 8980 Feb 28  2017 examples.desktop
drwxrwxr-x 10 username username 4096 May  2  2017 node_modules
</pre>

## Installation

compile with `gcc lsl.c -o lsl`  
execute with `./lsl`  
to execute in any directory, `sudo cp lsl /usr/bin`, though why you would want to do that instead of `ls -l` or `alias lsl='ls -l'` is beyond me

## Contributing

Do as you wish, I suppose

## License

MIT