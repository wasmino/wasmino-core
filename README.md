[Wasmino = WASM + Arduino (Running Arduino code in browser)](https://blog.yifangu.com/2020/12/30/wasmino-wasm-arduino-running-arduino-code-in-browser/)
-------------------------------------------------------------------------------------------------------------------------------------------------------

![](https://yifangucom.files.wordpress.com/2020/12/wasmino.gif?w=320)

Wasmino Running the “Fade” example

Wasmino is an Arduino core fully compatible with the Arduino IDE. It allows you to compile your Arduino sketch to WebAssembly and run it in a browser, all using the Arduino IDE.

Showcase
--------

This is the off-the-shelf “Fade” example in the Arduino IDE.

```C++
/*
  Fade
 
  This example shows how to fade an LED on pin 9 using the analogWrite()
  function.
 
  The analogWrite() function uses PWM, so if you want to change the pin you're
  using, be sure to use another PWM capable pin. On most Arduino, the PWM pins
  are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.
 
  This example code is in the public domain.
 
  http://www.arduino.cc/en/Tutorial/Fade
*/
 
int led = 9;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
 
// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
}
 
// the loop routine runs over and over again forever:
void loop() {
  // set the brightness of pin 9:
  analogWrite(led, brightness);
 
  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;
 
  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}
```


You can see it compiled to WASM and running here:

(**Click the “Run” button** and the LED should start to animate)

[https://demo.wasmino.org/#src=gist%3A%2F%2F01734051e7ab5b82173ea947b882ad80&pins=%7B%229%22%3A%7B%22type%22%3A%22led%22%7D%7D](https://demo.wasmino.org/#src=gist%3A%2F%2F01734051e7ab5b82173ea947b882ad80&pins=%7B%229%22%3A%7B%22type%22%3A%22led%22%7D%7D)

Setting it up
-------------

Disclaimer: The toolchain is only tested on Windows (x86-64) and Linux (x86-64), and probably will work for MacOS. It 100% won’t work on 32bit systems since some dependencies (WASI and Binaryen) are 64bit only.

You’ll need your Arduino IDE installed first.

Go to File -> Preferences, and add [https://github.com/wasmino/wasmino-core/releases/download/v0.0.1/package\_wasmino\_index.json](https://github.com/wasmino/wasmino-core/releases/download/v0.0.1/package_wasmino_index.json) to “Additional Boards Manager URLs”.

![](https://yifangucom.files.wordpress.com/2020/12/image.png?w=969)

Install “Wasmino Virtual Boards” in Tools -> Board -> Board Manager. Select Wasmino as as target board.

Write some Arduino code, click upload, and an URL will be generated for you. (If you are uploading for the first time, it will ask you to login to your GitHub account. It uses GitHub Gist for saving the compiled WASM binary. The login times out in 60 seconds.)

![](https://yifangucom.files.wordpress.com/2020/12/image-1.png?w=969)

Paste that URL into your browser and use the UI to add LEDs or toggle switches. The URL will automatically update as you make changes. You can then copy the resulting URL in the browser and share it with other people.

Caveats
-------

The Wasmino should be viewed as having an infinitely fast CPU, and operations other than delay and delayMicroseconds do not take any CPU time. Calling time functions micros and millis should reflect this. IO can only happen during sleep, therefore you cannot do

```C++
while (!digitalRead(2)) {} // this will hang the browser tab
```

and should do this instead
```C++
while (!digitalRead(2)) {
  delay(50);
}
```

Having a infinitely fast CPU also means that interrupts will only happen during delay and delayMicroseconds. It is still good practice to use noInterrupts and interrupts to protect critical sections if you plan to use your code on real hardware.

Other caveats exist such as the lack of BUILTIN_LED macro since Wasmino, being virtual, doesn’t have a built-in LED.

What works and what doesn’t work
--------------------------------


    From: https://www.arduino.cc/reference/en/
    
    y: working
    n: not working
    
    Digital I/O
    y   digitalRead()
    y   digitalWrite()
    y   pinMode()
    Analog I/O
    y*  analogRead()
    n   analogReference()
    y   analogWrite()
    Zero, Due & MKR Family
    n   analogReadResolution()
    n   analogWriteResolution()
    Advanced I/O
    n   noTone()
    n   pulseIn()
    n   pulseInLong()
    n   shiftIn()
    n   shiftOut()
    n   tone()
    Time
    y   delay()
    y   delayMicroseconds()
    y   micros()
    y   millis()
    Math
    y   abs()
    y   constrain()
    y   map()
    y   max()
    y   min()
    y   pow()
    y   sq()
    y   sqrt()
    Trigonometry
    y   cos()
    y   sin()
    y   tan()
    Characters
    n   isAlpha()
    n   isAlphaNumeric()
    n   isAscii()
    n   isControl()
    n   isDigit()
    n   isGraph()
    n   isHexadecimalDigit()
    n   isLowerCase()
    n   isPrintable()
    n   isPunct()
    n   isSpace()
    n   isUpperCase()
    n   isWhitespace()
    Random Numbers
    n   random()
    n   randomSeed()
    Bits and Bytes
    y   bit()
    y   bitClear()
    y   bitRead()
    y   bitSet()
    y   bitWrite()
    y   highByte()
    y   lowByte()
    External Interrupts
    y   attachInterrupt()
    y   detachInterrupt()
    Interrupts
    y   interrupts()
    y   noInterrupts()
    Communication
    y*  Serial
    y   Stream
    USB
    n   Keyboard
    n   Mouse
    
    *   Not exposed in the demo web app.
    *   Not exposed in the demo web app.


The Wasmino Arduino core is available at [https://github.com/wasmino/wasmino-core](https://github.com/wasmino/wasmino-core).

The runtime is also available at [https://github.com/wasmino/wasmino-runtime](https://github.com/wasmino/wasmino-runtime).
