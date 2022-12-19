//const FS = require('fs');
//const path = require('path');
import * as FS from 'fs';
import * as path from 'path';

const GZIPPED_PAGE = FS.readFileSync(path.resolve('./dist/index.html.gz'));

function chunkArray(myArray, chunk_size){
  var index = 0;
  var arrayLength = myArray.length;
  var tempArray = [];
  for (index = 0; index < arrayLength; index += chunk_size) {
      let myChunk = myArray.slice(index, index+chunk_size);
      // Do something if you want with the group
      tempArray.push(myChunk);
  }
  return tempArray;
}

function addLineBreaks(buffer){
  let data = '';
  let chunks = chunkArray(buffer, 30);
  chunks.forEach((chunk, index) => {
    data += chunk.join(',');
    if(index+1 !== chunks.length){
      data+=',\n';
    }
  });
  return data;
}

const CPPHEADER = 
`
#ifndef _espconnect_webpage_h
#define _espconnect_webpage_h

const uint32_t ESPCONNECT_HTML_SIZE = ${GZIPPED_PAGE.length};
const uint8_t ESPCONNECT_HTML[] PROGMEM = { 
${ addLineBreaks(GZIPPED_PAGE) }
};

#endif
`;

FS.writeFileSync(path.resolve('./dist/espconnect_webpage.h'), CPPHEADER);


