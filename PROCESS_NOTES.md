# Process Notes

This is where I log my progress, and is not formal documentation. Potential dead ends ahead, but feel free to learn along with me.

## Parts

Got this [SparkFun Pulse Oximeter and Heart Rate Sensor](https://www.sparkfun.com/products/15219).

The sensor has a "[Qwiic](https://www.sparkfun.com/qwiic)" connection, which is Sparkfun's system of making it easy to wire up sensors – without soldering – to each other and to one of the company's [Arduino](https://www.arduino.cc/)-clones, the [RedBoard](https://www.sparkfun.com/products/15123). 

New makers should probably go ahead and nab a RedBoard, but I have several [Arduino Uno](https://www.sparkfun.com/products/11021)s lying around, so I got a cheap [Qwiic adapter for Arduino](https://www.sparkfun.com/products/14352) (which the Arduino world calls a "shield.") Ironically, I had to solder the pins onto the shield, but I'm okay with that.

I also nabbed a little box of [Qwiic wires](https://www.sparkfun.com/products/15081).

## Coding the sensor

Followed along with the [SparkFun Pulse Oximeter and Heart Rate Monitor Hookup Guide](https://learn.sparkfun.com/tutorials/sparkfun-pulse-oximeter-and-heart-rate-monitor-hookup-guide).

Big red note on that page: "NOTE: This device is not intended to diagnose or treat any conditions."

Instructions say to search the Arduino Libraries for "SparkFun Bio Sensor Arduino Library" – but that's not exactly what it's called any more. Just search for "Bio Sensor" and you'll find it.

Used the `File > Examples > SparkFun Bio Sensor Hub Library > Example1_config_BPM_Mode1.ino` example.

Worked right away!

Took screen shots.

## Displaying with Neopixel Matrix

Details about the Neopixel matrix library are [here](https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library)

But what I really need are the [GFX library details](https://learn.adafruit.com/adafruit-gfx-graphics-library/overview), which describe how to write text, etc.

Except that the digits in that library are tooo big for what I want. (On my matrix, the digit "1" fills the whole "screen" vertically.)

Hmmmm. I've been here before. Oh yeah, with my "[Weather Matrix](https://johnkeefe.net/make-every-week-entryway-weatherbot)" project, when I wanted to display temperature on the same screen. So [stealing from my own code](https://github.com/jkeefe/weather-matrix/blob/master/weather_matrix.ino). 

