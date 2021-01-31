<p>
  <br>
</p>
<p align="center"><img src="/docs/logo.png?sanitize=true&raw=true" width="500"></p>
<br/>
<br/>

<p align="center">
<img src="https://img.shields.io/github/last-commit/ayushsharma82/ESPConnect.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/travis/com/ayushsharma82/ESPConnect/main?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/ayushsharma82/ESPConnect.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/6QGVpSj" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%245-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
 
<br>
<br>

<p align="center">
  <b>ESPConnect</b> is a simplistic and easy approch to having a WiFi manager on your ESP MCUs. It is a lightweight library which let's you configure WiFi networks without any fuss. ESPConnect works with both <b>ESP8266</b> & <b>ESP32</b>.
</p>
 
<br>

<h4 align="center">Portal Preview:</h4>
<p align="center">
  <img src="/docs/preview.png" width="520">
</p>

<br>

<h2>How to Install</h2>

###### Directly Through Arduino IDE ( Currently Submitted for Approval. Use Manual Install till it gets Approved.)
Go to Sketch > Include Library > Library Manager > Search for "ESPConnect" > Install

###### Manual Install

For Windows: Download the [Repository](https://github.com/ayushsharma82/ESPConnect/archive/main.zip) and extract the .zip in Documents>Arduino>Libraries>{Place "ESPConnect" folder Here}

For Linux: Download the [Repository](https://github.com/ayushsharma82/ESPConnect/archive/main.zip) and extract the .zip in Sketchbook>Libraries>{Place "ESPConnect" folder Here}

###### Manually through IDE

Download the [Repository](https://github.com/ayushsharma82/ESPConnect/archive/main.zip), Go to Sketch>Include Library>Add .zip Library> Select the Downloaded .zip File.

<br>

<h2>Dependencies</h2>
<p>
  <h5>For ESP8266:</h5>
  <ul>
      <li>ESP8266 Arduino Core - <b>latest</b></li>
      <li>ESPAsyncTCP - <b>v1.2.2</b></li>
  	  <li>ESPAsyncWebServer - <b>v1.2.3</b></li>
  </ul>
  
  <h5>For ESP32:</h5>
  <ul>
      <li>ESP32 Arduino Core - <b>latest</b></li>
      <li>AsyncTCP - <b>v1.1.1</b></li>
  	  <li>ESPAsyncWebServer - <b>v1.2.3</b></li>
  </ul>
</p>

<br>

<h2>Documentation</h2>
<p>ESPConnect is a dead simple library which does your work in just 2 lines:</p>

<br>

#### `ESPConnect.autoConnect(const char* ssid, const char* password, unsigned long timeout)`
This function sets the SSID and password for your captive portal. The third parameter - `timeout` is optional which tells the library to keep the captive portal running for `x` - milliseconds. (Default - `3 minutes`)

<br>

#### `ESPConnect.begin(AsyncWebServer *server)`
Call this function after `autoConnect` to start the WiFi manager. This function will first try to connect to the previous configured WiFi credentials and if those are not found, it will spin up a captive portal. If the user doesn't sets the wifi credentials, the library will return to execute your rest of the code in the setup block.

**Please note:** WiFi inactivity is managed by your code. If you think you need to reboot the MCU if there is no WiFi then please call the `ESP.restart()` function after checking `ESPConnect.begin()`'s response.

<br>
<br>

<h2>Examples</h2>
 
 Checkout example for ESP8266 and ESP32 in `examples` directory. [Click Here](https://github.com/ayushsharma82/ESPConnect/tree/master/examples)
 
<br>

<h2>Contributions</h2>
<p>Every contribution to this repository is highly appreciated! Don't fear to create issues / pull requests which enhance or fix the library, our maintainers are always ready to keep this library up & compiling.</p>

ESPConnect consists of 2 parts: 
- C++ library ( in `src` folder )
- Svelte webpage ( in `portal` folder ) ( provided source code for transparency )</li>

You can contribute to the part you have most skill in.


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
