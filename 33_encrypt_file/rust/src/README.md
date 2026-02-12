# Encrypt file

## Description

1. Read file to be encrypted from stdin
2. Determine size of file
3. If small file, read entire contents to byte array (whole file processing)
4. If large file, split into chunks, read chunks into set of byte array (stream file processing)
5. Use cipher of choice to transform file contents
6. Write transformed file contents to single file

## Considerations

* Symmetric vs asymmetric: the former is preferrable, with the same key for encrypt/decrypt
* Password-based or raw key
* Whole file or streaming
* No cryptographic primitives available, so either use:
  * Third-party libs
  * Non-cryptographic transformation
  * Combine primitives like XOR, hashing and randomness manually

