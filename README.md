<p>
  <br>
</p>
<p align="center"><img src="/docs/logo.png?sanitize=true&raw=true" width="500"></p>
<br/>
<br/>

<p align="center">
<img src="https://img.shields.io/github/last-commit/ayushsharma82/ESPConnect.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/travis/com/ayushsharma82/ESPConnect/master?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/ayushsharma82/ESPConnect.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/6QGVpSj" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%245-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
 
<br>
<br>

<p align="center">
  <b>ESPConnect</b> is a simplistic and easy approch to having a WiFiManager on your ESP MCUs. It is a lightweight library and hosts a easy-to-use captive portal which does it's job without any hassle. ESPConnect works with both <b>ESP8266</b> & <b>ESP32</b>.
</p>
 
<br>

<p align="center">
  <h4>Portal Preview:</h4>
  <img src="/docs/preview.png" width="520">
</p>

<br>

<h2>How to Install</h2>

###### Directly Through Arduino IDE ( Currently Submitted for Approval. Use Manual Install till it gets Approved.)
Go to Sketch > Include Library > Library Manager > Search for "ESPConnect" > Install

###### Manual Install

For Windows: Download the [Repository](https://github.com/ayushsharma82/ESPConnect/archive/master.zip) and extract the .zip in Documents>Arduino>Libraries>{Place "ESPConnect" folder Here}

For Linux: Download the [Repository](https://github.com/ayushsharma82/ESPConnect/archive/master.zip) and extract the .zip in Sketchbook>Libraries>{Place "ESPConnect" folder Here}

###### Manually through IDE

Download the [Repository](https://github.com/ayushsharma82/ESPConnect/archive/master.zip), Go to Sketch>Include Library>Add .zip Library> Select the Downloaded .zip File.

<br>

<h2>Documentation</h2>
<p>ESPConnect is a dead simple library which does your work in just 2 lines.</p>

<br>

 Include ESPConnect library `#include <ESPConnect.h>` at top of your sketch.
 
 1. Add - `ESPConnect.begin();` in your setup block.
 2. Add - `ESPConnect.loop();` in your loop block.
 
 That's all, now you have a WiFi Manager running on your ESP Module!

<br>

By default, ESPConnect uses your MCU's chip id as a unique SSID for your captive portal AP. If you want to set custom SSID and Password for autoConnect, then you can set it via `ESPConnect.autoConnect("ssid", "password");`. ( Place it above `begin` function ).

 
<br>
<h2>Examples</h2>
 
 Checkout example for ESP8266 and ESP32 in `examples` directory. [Click Here](https://github.com/ayushsharma82/ESPConnect/tree/master/examples)
 
<br>

<h2>Contributions</h2>
<p>Every contribution to this repository is highly appreciated! Don't fear to create issues / pull requests which enhance or fix the library, our maintainers are always ready to keep this library up & compiling.</p>
<p>
  ESPConnect consists of 2 parts: 
  <ol>
    <li>C++ library ( in `src` folder )</li>
    <li>Svelte webpage ( in `ui` folder ) ( provided source code for transparency )</li>
  </ol>
  
  You can contribute to the part you have most skill in.
<p>

<br>
If you like this amazing library, You can buy me a coffee:
<br/><br/>
<a href="https://www.buymeacoffee.com/6QGVpSj" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%245-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<br/>
<br/>


<h2>License</h2>
ESPConnect is licensed under General Public License v3 ( GPLv3 ).
<br>
<br>


<h3>What about Commerical Usage?</h3>
<p>
If your company needs a embedded web interface like ESPConnect/ESP-DASH for 'commercial applications', then you can send me a mail at: <a href="mailto:asrocks5@gmail.com">asrocks5@gmail.com</a>. 

I'm are open to a licensing deal or provide software development services which are along the lines of merging low-power microcontrollers with famous web technologies. For example: ESPConnect which consists of a C++ library and a lightweight Svelte SPA to fit inside a microcontroller.
</p>
<br/>
