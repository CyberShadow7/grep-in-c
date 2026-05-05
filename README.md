# Grep In C

A `grep` clone written from scratch in C.

***
## Purpose

My purpose for this project is to write some code like the good old days when AI and LLMs did not exist.

Plus, it's so much more fun writing your own code. :)

So, even though I might be reimplementing the wheel on some stuff, just the fact that I am not frustrated and not reaching for an LLM when things get hard makes me very happy.

***
## Testing

Given that I do not think that I can do a better job at rewriting the legendary, battle-tested, `grep`, I will just be making sure my version just runs on different Linux systems and maybe a Windows VM.

This was written in Arch Linux so I am not sure which glibc version someone would need, so enjoy static binaries or build from source!

***
## Compilation

Make sure you have `gcc` installed!

### Dynamic executables

After downloading this repo, `cd` into it, the following command should be more than enough to make a dynamic executable for your system to run.

```sh
$ gcc -o grep-in-c main.c
```

### Static executables

Same instructions as for dynamic, just add some changes to the `gcc` command.

```sh
$ gcc -s -static -o grep-in-c main.c # -s for stripped symbols
```

***
## Usage

So far, this is only for the terminal.

The syntax is: `grep-in-c <FILE> <STS>` where `STS` means String To Search (for).

***
## Notes

As this is a very early, work-in-progress, project, expect to see some debug output and features which aren't functional yet.
***
