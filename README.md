<!-- file README.md -->

# Overview and philosophy #

`bismon` (this is a *temporary* name) is a **persistent monitor** which (in the future) would interact with the [GCC](http://gcc.gnu.org/) compiler thru plugins.
It is (or will become) somehow a successor to my old [GCC MELT](http://starynkevitch.net/Basile/gcc-melt/) project; the medium-term goal is also [static program source code analysis](https://en.wikipedia.org/wiki/Static_program_analysis) with the help of [GCC](http://gcc.gnu.org/).

My (Basile Starynkevitch, employed at [CEA,
LIST](http://www-list.cea.fr/) in France) work on `bismon` is partly
funded by the European Union, Horizon H2020 programme,
[CHARIOT](http://chariotproject.eu/) project, under Grant Agreement No
780075. Within CHARIOT I will focus on analysis of some kind of
[IoT](https://en.wikipedia.org/wiki/Internet_of_things) software coded
in C or C++ and ([cross-]()) compiled by [GCC](http://gcc.gnu.org/) on
a Linux desktop.

Currently (mid-2018) `bismon` still in **pre alpha**-stage, and it is *free software* under [GPLv3+](https://www.gnu.org/licenses/gpl-3.0.en.html) license. It is intended for a Linux x86-64 desktop developer's workstation (won't work on Windows or MacOSX or Android).

## Approach ##

*Develop some kind of* high-level (more expressive, more declarative)
[**domain-specific
language**](https://en.wikipedia.org/wiki/Domain-specific_language) to
facilitate [static program source code
analysis](https://en.wikipedia.org/wiki/Static_program_analysis) above
[GCC](http://gcc.gnu.org/), so same motivations as [GCC
MELT](http://starynkevitch.net/Basile/gcc-melt/) (and more broadly
related to [Coccinnelle](http://coccinelle.lip6.fr),
[Frama-C](http://frama-c.com), [Rascal](https://www.rascal-mpl.org/),
[CrossMiner](https://www.crossminer.org/), [Clang
analyser](http://clang-analyzer.llvm.org/) etc...).


Important features and goals of that domain-specific language and its implementation include:

* [orthogonal persistence](https://en.wikipedia.org/wiki/Persistence_%28computer_science%29);
this is a major feature which was not realistically addable to [GCC
MELT](http://starynkevitch.net/Basile/gcc-melt/) so requires a
complete redesign. The `bismon` process is expected to be relatively
long-lived and retains most of its data from one run to the next (in `store*.bmon` textual files).

* [dynamic typing](https://en.wikipedia.org/wiki/Type_system#DYNAMIC) like in Python, Scheme, Ruby, JavaScript, ... (and somehow MELT).

* [multi-threaded](https://en.wikipedia.org/wiki/Thread_%28computing%29) but without any [GIL](https://en.wikipedia.org/wiki/Global_interpreter_lock)

* [reflexive](https://en.wikipedia.org/wiki/Reflection_%28computer_programming%29)

* [homoiconic](https://en.wikipedia.org/wiki/Homoiconicity)

* [garbage collected](https://en.wikipedia.org/wiki/Garbage_collection_%28computer_science%29)

* [declarative](https://en.wikipedia.org/wiki/Declarative_programming), with good pattern matching and rules formalism

* [bootstrapped](https://en.wikipedia.org/wiki/Bootstrapping_%28compilers%29),
  and [compiled to C](https://softwareengineering.stackexchange.com/a/257873/40065),
  perhaps some parts using [GCCJIT](https://gcc.gnu.org/onlinedocs/jit/)

* generating GCC plugins (in C++) which would interact with `bismon` by some RPC mechanism

Notice that the syntax and semantics of `bismon` is *work-in-progress* and subject to change incompatibly.

# build instructions #

On Linux/x86-64 (e.g. Debian/Unstable or recent Ubuntu) only:

## prerequisites ##

* recent [GCC](http://gcc.gnu.org/) (version 7 or 8), with C and C++ and JIT support and plugins enabled (check with `gcc -v`).

* [GTK](http://gtk.org/) 3.22 or better

* [GNU make](https://www.gnu.org/software/make/) 4 or better

* [libbacktrace](https://github.com/ianlancetaylor/libbacktrace) - it is sometimes part of your system *GCC*, but you'll better install it separately

* [markdown](https://www.markdownguide.org/)

## building ##

Edit the `Makefile` if needed (perhaps the `PREPROFLAGS=` and `LIBES=` lines, e.g. to use your system's GCC `libbacktrace`).

Run `make` or `make -j3`

The persistency mechanism is tested by `make redump`

The `./bismon` program accepts a `--help` and `--version`. It has a crude graphic user interface (perhaps to be replaced later by a Web one).

## using `bismon` ##

It is **not really usable yet** except by me (Basile) in april 2018 (no
static source code analysis yet, no generation of GCC plugins yet). If
you want to see something, run `./bismon` then type `the_system` in the command window
labelled *new-bismon*, then both *Ctrl* and *Return* keys pressed
together. Ask me (Basile Starynkevitch, [`basile@starynkevitch.net`](mailto:basile@starynkevitch.net) or [`basile.starynkevitch@cea.fr`](mailto:basile.starynkevitch@cea.fr)...) for details.