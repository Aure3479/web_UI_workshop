# web_UI_workshop

Link to Figma : https://www.figma.com/design/F5hvNst0VXWD3h541jlrZJ/Figma_web_ui_workshop?node-id=0-1&t=kpybbAmQYPcCgfEp-1
(Has 3 pages named : details , site map , what's it's going to be)
Paper sketch on page :"what's it's going to be"

Demo video is: Demo_Aurelien.mp4

Arduino code is : collect_colrs_react.ino
(don't forget to install libraries and Adafruit QT PY ESP32-C3 board to test)

Index.html has Html , css and js implementation in it , needs a internet connection for the websocket and icons


What's it supposed to do : 
-send color codes live to a websocket (don't forget to change Local IP and change wifi and password ) and change the two neopixels RGB leds to the given color (don't forget to change the pins and the number of neopixels)
-send the positive or negative reaction to the connected device by choosing either "Oui" or "Non" by going "up" or "down" and validating your choice with "A" button (buttons where part of a keyboard matrix so change if necessary)
