# libsbf
Simple Bind Files library
This library is handling very simple file format for binding some files to one file.
It is implemented pure C for enabling running on an embedded micro controller.

It can be run in a Linux environment.

## How to build
Just run "make" on top directory. Then, it generates lib/libsbf.a, bin/sbfbind and bin/sbfunbind. "sbfbind" and "sbfunbind" is samples to use the libsbf.

## How to use samples

### sbfbind
Bind files

$ sbfbind \<result file name\> \<binding file path\> (\<binding file path\> ...)

### sbfunbind
Unbind files

$ sbfunbind \<sbf file path\> (\<prefix for each file\>)

## Testing

Run "make" under the test directory.

Then generates 100 files of random binary data of random size, and combin all files into test_result.sbf.

To split bound files, "make unbind" is executed, which will split and generate each of the files that were combined in test_result.sbf under the result directory.

## SBF file format

SBF is just bind some files. The format is very simple.

There are 2 header types: one is a total file header (SBF File Header), and the other is each file content header(File Content Header).

### Overview of file format

<table>
  <tbody>
    <tr>
      <td> SBF File Header </td>
      <td> 1st File Content Header </td>
      <td> 1st File Data </td>
      <td> 2nd File Content Header </td>
      <td> 2nd File Data </td>
      <td> ..... </td>
    </tr>
  </tbody>
</table>

### SBF File header format

<table>
  <thead>
    <tr>
      <th></th>
      <th align="center" bgcolor="#444444"> Byte +0 </th>
      <th align="center" bgcolor="#444444"> Byte +1 </th>
      <th align="center" bgcolor="#444444"> Byte +2 </th>
      <th align="center" bgcolor="#444444"> Byte +3 </th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="center" bgcolor="#444444"> 0x00 </td>
      <td align="center" colspan="4"> TAG = 'S', 'B', 'F', ' ' </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x04 </td>
      <td align="center"> Reserve </td>
      <td align="center"> SBF Version </td>
      <td align="center" colspan="2"> Number of files (LE) </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x08 </td>
      <td align="center" colspan="4"> <----- File Content header and file data </td>
    </tr>
  </tbody>
</table>

### File Content Header and File data

<table>
  <thead>
    <tr>
      <th></th>
      <th align="center" bgcolor="#444444"> Byte +0 </th>
      <th align="center" bgcolor="#444444"> Byte +1 </th>
      <th align="center" bgcolor="#444444"> Byte +2 </th>
      <th align="center" bgcolor="#444444"> Byte +3 </th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="center" bgcolor="#444444"> 0x00 </td>
      <td align="center" colspan="4"> File Size (little endian) </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x04 </td>
      <td align="center" colspan="4"> <--- Comment[0:3] (total 32Bytes)  </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x08 </td>
      <td align="center" colspan="4"> ---- Comment[4:7] ---- </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x0C </td>
      <td align="center" colspan="4"> ---- Comment[8:11] ---- </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x10 </td>
      <td align="center" colspan="4"> ---- Comment[12:15] ---- </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x14 </td>
      <td align="center" colspan="4"> ---- Comment[16:19] ---- </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x18 </td>
      <td align="center" colspan="4"> ---- Comment[20:23] ---- </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x1C </td>
      <td align="center" colspan="4"> ---- Comment[24:27] ---- </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x20 </td>
      <td align="center" colspan="4">      Comment[28:31] ----> </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> 0x24 </td>
      <td align="center" colspan="4"> <----- File data (until file size) --- </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> .... </td>
      <td align="center" colspan="4">  ----- File data  --- </td>
    </tr>
    <tr>
      <td align="center" bgcolor="#444444"> .... </td>
      <td align="center">  File data  ---> </td>
      <td align="center" colspan="3"> <--- Padding for alignment 4 bytes ---> </td>
    </tr>
  </tbody>
</table>

:warning: After the tail of file data, padding byte(s) is/are added for 4 byte alignment.

