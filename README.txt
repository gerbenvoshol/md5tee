md5tee - read from standard input and write to standard output.
         calculate md5sum and size and write to standard error (or to file).


INSTALL INSTRUCTIONS:

  apt-get install libssl-dev make gcc
  
  make

  # optional tests:
  make test
  
  # will install to /usr/local/bin
  make install

DESCRIPTION:

 This program pipes content from stdin to stdout and calculates its
 size and md5sum.
 
 Usage: command1 | md5tee | command2
 Will pipe from command1 to command2, and write size and md5sum to stderr

 Usage: command1 | md5tee filename | command2
 Will pipe from command1 to command2. writes md5sum to filename
 Note: md5tee will create the output file, it must not exists previously

 Chunksize can be changed with environment variable CHUNKSIZE (in bytes)
 see --help for default CHUNKSIZE. Use 0 to disable it.
 
 Based on Pedro Zorzenon Neto https://github.com/pzn1977/md5tee

 (c) 2023 https://github.com/gerbenvoshol/md5tee 

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see https://www.gnu.org/licenses/
