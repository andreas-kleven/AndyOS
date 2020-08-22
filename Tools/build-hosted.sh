#!/bin/bash
SYSTEM=andyos
SYSROOT=$HOME/$SYSTEM

parent_path=$(cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P)
srcdir=$parent_path/src

export PREFIX="$parent_path/build/hosted"
export TARGET=i686-$SYSTEM
export PATH="$PREFIX/bin:$PATH"

cd $srcdir
mkdir build-automake-1.11.6
cd build-automake-1.11.6
$srcdir/automake-1.11.6/configure --prefix="$PREFIX"
make -j6 && make install

mkdir build-autoconf-2.64
cd build-autoconf-2.64
$srcdir/autoconf-2.64/configure --prefix="$PREFIX"
make -j6 && make install

# binutils
# config.sub

text="  -$SYSTEM* | \\\\"

cd $srcdir/binutils-2.30/
if ! grep -q $SYSTEM "config.sub"; then
    awk "/is not here because it comes later, after sysvr4./ { print; print \"$text\"; next }1" config.sub > config.sub.tmp
    mv config.sub.tmp config.sub
fi

# bfd/config.bfd

text="\
  i[3-7]86-*-$SYSTEM*)\n\
    targ_defvec=i386_elf32_vec\n\
    targ_selvecs=\n\
    targ64_selvecs=x86_64_elf64_vec\n\
    ;;\n\
  #ifdef BFD64\n\
  x86_64-*-$SYSTEM*)\n\
    targ_defvec=x86_64_elf64_vec\n\
    targ_selvecs=i386_elf32_vec\n\
    want64=true\n\
    ;;\n\
  #endif"

cd $srcdir/binutils-2.30/bfd
if ! grep -q $SYSTEM "config.bfd"; then
    awk "/START OF targmatch.h/ { print; print \"$text\"; next }1" config.bfd > config.bfd.tmp
    mv config.bfd.tmp config.bfd
fi

# gas/configure.tgt

text="  i386-*-$SYSTEM*)    fmt=elf ;;"

cd $srcdir/binutils-2.30/gas
if ! grep -q $SYSTEM "configure.tgt"; then
    awk "/case \\\${generic_target} in/ { print; print \"$text\"; next }1" configure.tgt > configure.tgt.tmp
    mv configure.tgt.tmp configure.tgt
fi

# ld/configure.tgt

text="\
i[3-7]86-*-$SYSTEM*)\n\
			targ_emul=elf_i386_$SYSTEM\n\
			targ_extra_emuls=elf_i386\n\
			targ64_extra_emuls=\"elf_x86_64_$SYSTEM elf_x86_64\"\n\
			;;\n\
x86_64-*-$SYSTEM*)\n\
			targ_emul=elf_x86_64_$SYSTEM\n\
			targ_extra_emuls=\"elf_i386_$SYSTEM elf_x86_64 elf_i386\"\n\
			;;"

cd $srcdir/binutils-2.30/ld
if ! grep -q $SYSTEM "configure.tgt"; then
    awk "/break the alpha sorting/,/case/ { if(\$0 ~ /case/) { print; print \"$text\"; next }}1" configure.tgt > configure.tgt.tmp
    mv configure.tgt.tmp configure.tgt
fi

# ld/Makefile.am
cd $srcdir/binutils-2.30/ld

text1="\n\
eelf_i386_$SYSTEM.c: \$(srcdir)/emulparams/elf_i386_$SYSTEM.sh \\\\\n\
  \$(ELF_DEPS) \$(srcdir)/scripttempl/elf.sc \${GEN_DEPENDS}\n\
\\t\${GENSCRIPTS} elf_i386_$SYSTEM \\\"\$(tdir_elf_i386_$SYSTEM)\\\""

text2="\n\
eelf_x86_64_$SYSTEM.c: \$(srcdir)/emulparams/elf_x86_64_$SYSTEM.sh \\\\\n\
  \$(ELF_DEPS) \$(srcdir)/scripttempl/elf.sc \${GEN_DEPENDS}\n\
\\t\${GENSCRIPTS} elf_x86_64_$SYSTEM \\\"\$(tdir_elf_x86_64_$SYSTEM)\\\""

text3="  eelf_i386_$SYSTEM.c \\\\"
text4="  eelf_x86_64_$SYSTEM.c \\\\"

if ! grep -q $SYSTEM "Makefile.am"; then
    awk "/eelf_i386.c:/,/ELF_X86_DEPS/ { if(\$0 ~ /ELF_X86_DEPS/) { print; print \"$text1\"; next }}1" Makefile.am > Makefile.am.tmp
    mv Makefile.am.tmp Makefile.am

    awk "/eelf_x86_64.c:/,/ELF_X86_DEPS/ { if(\$0 ~ /ELF_X86_DEPS/) { print; print \"$text2\"; next }}1" Makefile.am > Makefile.am.tmp
    mv Makefile.am.tmp Makefile.am

    awk "/ALL_EMULATION_SOURCES = \\\\/ { print; print \"$text3\"; next }1" Makefile.am > Makefile.am.tmp
    mv Makefile.am.tmp Makefile.am

    awk "/ALL_64_EMULATION_SOURCES = \\\\/ { print; print \"$text4\"; next }1" Makefile.am > Makefile.am.tmp
    mv Makefile.am.tmp Makefile.am
fi

automake

# emulparams/elf_i386.sh
echo '. ${srcdir}/emulparams/elf_i386.sh
GENERATE_SHLIB_SCRIPT=yes
GENERATE_PIE_SCRIPT=yes
TEXT_START_ADDR=0x40000000' > $srcdir/binutils-2.30/ld/emulparams/elf_i386_$SYSTEM.sh

# emulparams/elf_x86_64.sh
echo '. ${srcdir}/emulparams/elf_x86_64.sh' > $srcdir/binutils-2.30/ld/emulparams/elf_x86_64_$SYSTEM.sh

# GCC
# config.sub

text="    -$SYSTEM* | \\\\"

cd $srcdir/gcc-8.1.0/
if ! grep -q $SYSTEM "config.sub"; then
    awk "/is not here because it comes later, after sysvr4/ { print; print \"$text\"; next }1" config.sub > config.sub.tmp
    mv config.sub.tmp config.sub
fi

# gcc/config.gcc

text1="\
*-*-$SYSTEM*)\n\
  gas=yes\n\
  gnu_ld=yes\n\
  default_use_cxa_atexit=yes\n\
  use_gcc_stdint=wrap\n\
  ;;"

text2="\
i[34567]86-*-$SYSTEM*)\n\
    tm_file=\\\"\${tm_file} i386/unix.h i386/att.h dbxelf.h elfos.h glibc-stdint.h i386/i386elf.h $SYSTEM.h\\\"\n\
    ;;\n\
x86_64-*-$SYSTEM*)\n\
    tm_file=\\\"\${tm_file} i386/unix.h i386/att.h dbxelf.h elfos.h glibc-stdint.h i386/i386elf.h i386/x86-64.h $SYSTEM.h\\\"\n\
    ;;"

cd $srcdir/gcc-8.1.0/gcc
if ! grep -q $SYSTEM "config.gcc"; then
    awk "/Common parts for widely ported systems/,/case/ { if(\$0 ~ /case/) { print; print \"$text1\"; next }}1" config.gcc > config.gcc.tmp
    mv config.gcc.tmp config.gcc

    awk "/Assume that newlib is being used and so __cxa_atexit is provided/,/case/ { if(\$0 ~ /case/) { print; print \"$text2\"; next }}1" config.gcc > config.gcc.tmp
    mv config.gcc.tmp config.gcc
fi

# libstdc++-v3/crossconfig.m4

text="\
  *-$SYSTEM*)\n\
    GLIBCXX_CHECK_COMPILER_FEATURES\n\
    GLIBCXX_CHECK_LINKER_FEATURES\n\
    GLIBCXX_CHECK_MATH_SUPPORT\n\
    GLIBCXX_CHECK_STDLIB_SUPPORT\n\
    ;;\n\
    "

cd $srcdir/gcc-8.1.0/libstdc++-v3
if ! grep -q $SYSTEM "crossconfig.m4"; then
    awk "/Base decisions on target environment/,/case/ { if(\$0 ~ /case/) { print; print \"$text\"; next }}1" crossconfig.m4 > crossconfig.m4.tmp
    mv crossconfig.m4.tmp crossconfig.m4
fi

autoconf

# libgcc/config.host

text="\
i[34567]86-*-$SYSTEM*)\n\
	extra_parts=\\\"\$extra_parts crti.o crtbegin.o crtend.o crtn.o\\\"\n\
	tmake_file=\\\"\$tmake_file i386/t-crtstuff t-crtstuff-pic t-libgcc-pic\\\"\n\
	;;\n\
x86_64-*-$SYSTEM*)\n\
	extra_parts=\\\"\$extra_parts crti.o crtbegin.o crtend.o crtn.o\\\"\n\
	tmake_file=\\\"\$tmake_file i386/t-crtstuff t-crtstuff-pic t-libgcc-pic\\\"\n\
	;;"

cd $srcdir/gcc-8.1.0/libgcc/
if ! grep -q $SYSTEM "config.host"; then
    awk "/enable_execute_stack=enable-execute-stack-empty.c/,/case/ { if(\$0 ~ /case/) { print; print \"$text\"; next }}1" config.host > config.host.tmp
    mv config.host.tmp config.host
fi

# fixincludes/mkfixinc.sh

text="    *-$SYSTEM* | \\\\"

cd $srcdir/gcc-8.1.0/fixincludes
if ! grep -q $SYSTEM "mkfixinc.sh"; then
    awk "/case/ { print; print \"$text\"; next }1" mkfixinc.sh > mkfixinc.sh.tmp
    mv mkfixinc.sh.tmp mkfixinc.sh
fi

# gcc/config/andyos.h

cp $parent_path/andyos.h $srcdir/gcc-8.1.0/gcc/config/

# Build

cd $srcdir
mkdir build-binutils-2.30
cd build-binutils-2.30
../binutils-2.30/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot=$SYSROOT --disable-werror
make -j6
make -j6 install
 
cd $srcdir/gcc-8.1.0
./contrib/download_prerequisites

cd $srcdir
mkdir build-gcc-8.1.0
cd build-gcc-8.1.0
../gcc-8.1.0/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot=$SYSROOT --with-newlib --enable-languages=c,c++
make -j6 all-gcc all-target-libgcc
make -j6 install-gcc install-target-libgcc

cd $parent_path
./download-libc.sh
./build-libc.sh

cd $srcdir/build-gcc-8.1.0
make -j6 all-target-libstdc++-v3
make -j6 install-target-libstdc++-v3

mkdir -p $SYSROOT/usr
cp -R $PREFIX/$TARGET/* $SYSROOT/usr
cp -R $PREFIX/lib/gcc/i686-andyos/8.1.0/* $SYSROOT/usr/lib

mkdir -p $HOME/opt/cross
cp -R $PREFIX/i686-andyos $HOME/opt/cross/
cp -R $PREFIX/include $HOME/opt/cross/
cp -R $PREFIX/lib $HOME/opt/cross/
cp -R $PREFIX/libexec $HOME/opt/cross/
cp -R $PREFIX/share $HOME/opt/cross/
cp -R $PREFIX/bin/i686-andyos-* $HOME/opt/cross/bin
