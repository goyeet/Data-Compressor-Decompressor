# Lossless Data Compressor and Decompressor

NOTE: This program is adapted from a Computer Systems and C Programming assignment.\
(Header files provided by Profesor)

This C program contains code for a Huffman encoder and decoder. The encoder and decoder read from
stdin and output to stdout unless specified to read/output using specific files.

## Formatting

Format the code with:

```
make format
```

## Building

Build the program with:

```
make all
```

## Running

To encode, run the program with:

```
$ ./encode
```

To decode, run the program with:

```
$ ./decode
```

and add any of the following options to specify what you want to run.
```
OPTIONS
  -h             Program usage and help.
  -v             Print compression statistics.
  -i infile      Input file to compress/decompress.
  -o outfile     Output of compressed/decompressed data.
```

## Cleaning

Remove all files that are compiler generated with:

```
make clean
```
