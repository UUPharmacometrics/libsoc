# A minimal R package that calls out to C

This repository contains a minimal example of a complete R package that calls
out to C.  I have written this package to demonstrate the minimal amount of
steps needed in order to write *and share* a package which calls C code.  The
document you are now reading grew out of my own experiences in writing and
sharing the [LXB package][lxb] with colleagues at work.

The [Writing R Extensions][manual] manual contains a reference for everything
that is mentioned here (and more).  You may also want to check out [Developing R packages](https://github.com/jtleek/rpackages).


## Packages with C code (more painful than you'd like unless your intended audience is programmers)

My assumption is that you want to build an R package and share it with your
colleagues.  As it turns out, this is not so simple.  When you create a package
it will by default just be an archive containing the source code that you
write.  This means that a C compiler is needed in order to install the
package.  If your colleagues all have a C compiler installed (maybe they are
programmers or all use Linux) then this is no problem.  If not, get ready for a
world of pain.

It is possible to create binary packages that do not require a C compiler for
installation, the catch is that they will only install on a machine with the
same operating system version and R version as was used to build the package.
You can get around this problem by uploading your package to [CRAN][cran],
since it creates binary versions of your package for you.  However,
submitting to CRAN involves quite a bit of work.  You have to make sure your
package compiles on Linux, Mac OS X as well as Windows.  The submission process
is not automated, so expect a delay of a couple of days between submission and
a binary version of your package being available.

Before going any further you should ask yourself if the pain is worth it.  If
you can get away with writing all your intended functionality in R without it
being too slow or using too much memory, then I'd suggest you stick with a pure
R solution.

## How to call C from R

Let me outline how R calls out to C without getting into the details of the R
Foreign Function Interface since this would take too much space.

R calls a C compiler to build a shared library of your C code.  This library
can then be dynamically loaded into R and you can call functions that you have
exported in your C code.  Here are the steps involved:

-   To generate a shared library call

        R CMD SHLIB reverse.c
    
    in the `src/` directory.  This will create the shared library `reverse.so`.
    The file name extension depends on which operating system you are using (I
    am using Mac OS X).
-   To load the library inside R you call `dyn.load('reverse.so')`.  
-   Finally, to call the function exported in your C call `.Call('reverse',
    1:10)`.  Here `reverse` is the name of the function exported in `reverse.c`
    and `1:10` is the (only) parameter this function takes.

Manually generating a shared library is a bit messy (it generates `.o` and
`.so` files in the directory where your source code is) and using `.Call()`
can be slightly dangerous (e.g. what happens if you pass the wrong type of
parameters?).  The solution is to generate a package.

## Preparing a package

A package is a directory with the structure in this repository (there may be
more files and folders, but I tried to make this example minimal).  Here is an
overview of what goes where in the package:

-   Put the C source code in the `src/` subdirectory.
-   Create an R wrapper function which calls out to your C code and put it in
    the `R/` subdirectory.  In this example it is sufficient to have a wrapper
    function like `reverse <- function(x) .Call('reverse', x)` but you may want
    to coerce any variables before passing them to your C function.  Note that
    it is not necessary to load the shared library with `dyn.load()` in the
    wrapper.  R takes care of this for us.
-   A `NAMESPACE` file which tells R to load the shared library and what
    wrapper functions to expose from the `R/` subdirectory.  In this example
    `useDynLib(reverse)` loads the library, and `export(reverse)` exports the
    `reverse` function from the R wrapper code.
-   A `DESCRIPTION` file which contains a summary of the package.  For example
    the package version is entered here.  Please use a sensible scheme for
    versioning your package (e.g. X.Y, where X is incremented when changes to
    the public API exposed inside `R/` breaks backward compatibility, and Y is
    incremented otherwise).  You also need to enter the version of R your
    package depends on; at the moment I am not sure what the best practice is
    when filling out this field.  A safe bet is the version of R that you are
    using, but this may be too restrictive.  Another idea is to pick the lowest
    version that is used by the people you are sharing your package with (if
    this is known to you).  As for the license field -- if you pick a
    non-standard license then you will get a warning when you check your
    package (see below), so it may make sense to pick a standard license.

These four items are all that is needed to create a functioning package, but
you will get a warning about missing documentation when checking the package
(see below).  To avoid warnings you will also need

-   A `man/` folder with documentation for the package itself and for each
    function that is exported in the `NAMESPACE` file.  The format used for
    documentation is described in the [guide on extending R][manual].  Note:
    you should include code examples in your help files.  These examples are
    run when you check the files so make sure your examples are complete.  If
    you want to write examples that can't be run for some reason, you need to
    wrap them in `\dontrun{}`.

## Building and installing

Creating a package which is ready to be shared consists of the following steps:

-   Building the package.  This creates an archive that you can share.  Change
    directory to the parent of the package and type
    
        R CMD build reverse

    This will generate the archive `reverse_1.0.tar.gz`, where `1.0` is the
    version of the package.
-   Checking the package.  This ensures that the package can be installed.
    Type
    
        R CMD check reverse_1.0.tar.gz
        
    If there are any problems you will be notified and a log file is created in
    the directory `reverse.Rcheck/`.  Note that you should pass the name of the
    archive to the `check` command, *not* the name of the directory the package
    resides in (this can be confusing because the latter works but it creates
    temporary files inside your package directory structure).

At this point you can go ahead and install the package by typing

    R CMD INSTALL reverse_1.0.tar.gz
    
Now start R, make sure the library loads by typing `library(reverse)` and that
the exported function works by typing `reverse(1:10)` (you should see the
numbers 1 to 10 in reversed order).

To uninstall the package type

    R CMD REMOVE reverse

If you only want to distribute the source version of your package then you are
done at this point.  Simply send the `reverse_1.0.tar.gz` archive to the people
you want to share with and tell them how to install your package.  The downside
to this is that everybody you share with *must* have a C compiler installed.
If you want to share with people who may not have a C compiler, then you need
to create a binary version of your package.

To create a binary version of your package you use the command

    R CMD INSTALL --build reverse_1.0.tar.gz
    
This will first install your package and then create a binary package archive
called `reverse_1.0.tgz` which you can share (the installation proceedure for
binary packages is the same as for source packages).  The problem with this
method is that the binary package will *only* install on computers with the
exact same operating system version and R version that you used to build the
binary package.  To work around this problem you will have to submit your
package to CRAN.

## Submitting to CRAN

Before submitting to CRAN you should ensure that your package passes all
checks.  This is not really a problem.  What is a problem however is that
packages on CRAN should compile on Linux, Mac OS X and Windows and it is up to
you to make sure that it does.  Getting your C code to compile on all three
platforms can be a big problem.  There is a site called
[win-builder][win-builder] to which you can upload a source package and it will
automatically check it on a Windows machine.  This is useful if you do not have
access to Windows, but it can be very time consuming to fix compilation
problems this way.  I do not know of any similar sites for Linux and Mac OS X
so if you do not have access to one of these operating systems then you are out
of luck.

To actually submit a package you should first read through the [submission
guidelines][submit] (you will be forced to confirm that you've read through
this later anyway).  Next, upload a source package to the CRAN ftp and send an
email to the CRAN mailing list (current addresses to the ftp and mailing list
can be found at [CRAN][cran]).  In your email the subject should include
package name and version (e.g. "reverse 1.0") and in the body simply ask for
the package to be added to CRAN.  The submission process is not automated,
instead the mailing list is read by the maintainers so be polite.  You will get
a reply to your submission, if you need to reply back make sure you CC the
mailing list again as there may be more than one maintainer handling your case.
If problems are found in your package and you need to upload a new version,
make sure you send a new submission email as well since the maintainers expect
a submission email to accompany each archive uploaded to the ftp.

That's it!  If you have any improvements to this document please open a pull
request and I will review and merge it.

[win-builder]: http://win-builder.r-project.org/
[cran]: http://cran.r-project.org/
[manual]: http://cran.r-project.org/doc/manuals/R-exts.html
[submit]: http://CRAN.R-project.org/web/packages/policies.html
[lxb]: http://cran.r-project.org/web/packages/lxb/index.html
