# Building

```sh
build test # to build src/text/**
```

# External Libraries

install [MSYS2](https://www.msys2.org/)

```sh
pacman -Syu # update package manager
pacman -S openssl
pacman -S mingw-w64-x86_64-zlib
pacman -S mingw-w64-x86_64-tesseract-ocr
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
