# Building

```sh
build.bat test # to build bin/test.exe
```

# External Libraries

install [MSYS2](https://www.msys2.org/)

```sh
cp /c/TDM-GCC-64/bin/mingw32-make.exe /c/TDM-GCC-64/bin/make.exe
nano ~/.bashrc
# add next two lines
export PATH=/c/TDM-GCC-64/bin:$PATH
export PREFIX=/c/TDM-GCC-64/x86_64-w64-mingw32
```

## OpenSSL

https://github.com/openssl/openssl

```sh
cd /tmp
wget https://github.com/openssl/openssl/releases/download/openssl-3.1.1/openssl-3.1.1.tar.gz
tar -xvf openssl-3.1.1.tar.gz
rm openssl-3.1.1.tar.gz
cd openssl-3.1.1
# 32-bits
./Configure mingw no-shared --prefix=$PREFIX --openssldir=$PREFIX --libdir=$PREFIX/lib32
make
make install
# 64-bits
./Configure mingw64 no-shared --prefix=$PREFIX --openssldir=$PREFIX --libdir=$PREFIX/lib
make
make install
cd ..
rm -R openssl-3.1.1
```

## Net-SNMP

https://github.com/net-snmp/net-snmp

```sh
cd /tmp
wget https://github.com/net-snmp/net-snmp/archive/refs/tags/v5.9.4.pre3.tar.gz
tar -xvf v5.9.4.pre3.tar.gz
rm v5.9.4.pre3.tar.gz
cd net-snmp-5.9.4.pre3
# 32-bits
./configure --host=i686-pc-mingw32 --prefix=$PREFIX --libdir=$PREFIX/lib32 --includedir=$PREFIX/include --with-libs="-lws2_32" --disable-shared --with-mibs=ALL --with-default-snmp-version=1 --with-sys-contact=@@no.where --with-sys-location=Unknown --with-logfile=/var/log/snmpd.log --with-persistent-directory=/var/net-snmp
make
make install
# 64-bits
./configure --prefix=$PREFIX --libdir=$PREFIX/lib --includedir=$PREFIX/include --with-libs="-lws2_32" --disable-shared --with-mibs=ALL --with-default-snmp-version=1 --with-sys-contact=@@no.where --with-sys-location=Unknown --with-logfile=/var/log/snmpd.log --with-persistent-directory=/var/net-snmp
make
make install
cd ..
rm -R net-snmp-5.9.4.pre3
```

## zlib

https://github.com/madler/zlib
```sh
cd /tmp
wget https://github.com/madler/zlib/releases/download/v1.2.13/zlib-1.2.13.tar.gz
tar -xvf zlib-1.2.13.tar.gz
rm zlib-1.2.13.tar.gz
cd zlib-1.2.13
# 32 bits
CFLAGS=-m32 ./configure --prefix=$PREFIX --libdir=$PREFIX/lib32 --static
make
make install
# 64 bits
./configure --prefix=$PREFIX --libdir=$PREFIX/lib --static
make
make install
cd ..
rm -R zlib-1.2.13
```

## ncurses

https://github.com/mirror/ncurses
```sh
cd /tmp
wget https://github.com/mirror/ncurses/archive/refs/tags/v6.4.tar.gz
tar -xvf v6.4.tar.gz
rm v6.4.tar.gz
cd ncurses-6.4
# 64 bits
export PATH_SEPARATOR=";"
./configure \
            --prefix=$PREFIX \
            --libdir=$PREFIX/lib \
            --without-libtool \
            --disable-home-terminfo \
            --enable-database \
            --disable-termcap \
            --enable-term-driver \
            --enable-sp-funcs \
            --enable-interop \
            --with-normal \
            --enable-overwrite 
make
make install
cd ..
rm -R ncurses-6.4
```