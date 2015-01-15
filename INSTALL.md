# SIR Installation Manual

If you want to get up and running quickly and easily.

## Build Steps

### Change working directory

```
$ cd sir
```

### Run `cmake` command

#### Basic usage

```
$ cmake ../sir -DCMAKE_INSTALL_PREFIX=./usr
```

#### To disable metadata support (enabled by default)

```
$ cmake ../sir -DCMAKE_INSTALL_PREFIX=./usr -Dmetadata=OFF
```

#### To force use Qt 4 instead Qt 5

```
$ cmake ../sir -DCMAKE_INSTALL_PREFIX=./usr -Dqt5=OFF
```

#### To force use Qt 4 instead Qt 5 without exiv2 metadata library

```
$ cmake ../sir -DCMAKE_INSTALL_PREFIX=./usr -Dmetadata=OFF -Dqt5=OFF
```

### Make it!

#### Simply

```
$ make
```

#### For multicore CPU

You can build faster using all core of your processor. Call `-j` parameter within `number_of_cores + 1` value.

For example for quad core call:

```
$ make -j5
```

### Finally install

For this manual install target is `./usr`.

```
$ make install
```
