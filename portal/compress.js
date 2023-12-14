import { gzipAsync } from '@gfx/zopfli';
import FS from 'fs'
import path from 'path'

const SAVE_PATH = '../src';

const BUNDLE_CSS = FS.readFileSync(path.resolve(path.resolve(), './public/build/bundle.css'));
const BUNDLE_JS = FS.readFileSync(path.resolve(path.resolve(), './public/build/main.js'));
const INDEX_HTML = 
`<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset='utf-8'>
	<meta name='viewport' content='width=device-width,initial-scale=1'>

	<title>ESPConnect</title>
	<style>
    ${BUNDLE_CSS}
	</style>
</head>
<body>
<div id="app"></div>
<script>
${BUNDLE_JS}
</script>
</body>
</html>
`;

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


(async function(){
  try{
    const GZIPPED_INDEX = await gzipAsync(INDEX_HTML, { numiterations: 15 });

    const FILE = 
`
#ifndef _espconnect_webpage_h
#define _espconnect_webpage_h

const uint32_t ESPCONNECT_HTML_SIZE = ${GZIPPED_INDEX.length};
const uint8_t ESPCONNECT_HTML[] PROGMEM = { 
${ addLineBreaks(GZIPPED_INDEX) }
};

#endif
`;

    FS.writeFileSync(path.resolve(path.resolve(), SAVE_PATH+'/espconnect_webpage.h'), FILE);
    console.log(`[COMPRESS.js] Compressed Bundle into webpage.h header file | Total Size: ${(GZIPPED_INDEX.length / 1024).toFixed(2) }KB`)
  }catch(err){
    return console.error(err);
  }
})();
