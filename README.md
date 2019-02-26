# The-Smokey-Powell
The Particle Photon Wifi Blynk controlled BBQ Smoker controller

This controller reads temperature through a thermistor. Control of your smokers heat is made through a fan or
Solid State relay. It has three extra probe inputs to be used as meat probes.
Wifi is used to connect to the cloud. 

The blynk app is used as the main interface from your phone or mobile device. It is worth noting that all the information runs through the cloud. This means one could be shopping for supplies, while also monitoring ones bbq back at home.

A 4x20 I2c LCD is used as a local display and four buttons as the interface controls.
The local interface is a little clunky. The main interface was intended to be the blynk app.

This controller can use replacement thermistors for Weber iGrill2 grilling thermometer. Or any other thermistor that will work 
with a 10k ohm reference resistor.

For those that use the circuit board I designed. The Outputs are sinking.

Watch The Youtube video for more information.
https://youtu.be/kV1lrQCiiBc

Included are links below or look above in the files.

The shared particle app https://go.particle.io/shared_apps/5c6e0d62d71ca3002285fe2e You will need to copy it to your apps so you can modify it.

The Blynk App (Look Above) Download Blynk. In Blynk there is an option take a picture of the QR code.

The circuit board gerber file	(Look Above)  Send this Zip file in to order a board. Board dimensions are 4.75"x2.5"
I may also decide to sell them

where I have my circuit boards made http://www.pcbunlimited.com/ They are great quality and cheap!

schematic of circuit board	(Look Above)

thingiverse link to make 3d printed case and DIN mounts (Link coming soon!)

youtube link to video	(Link coming soon)


Smokey Powell shopping list:

Bill of Material (Look Above)
digikey (some items are optional) you can use screw terminals or barrel jacks for power, outputs and inputs. Really you can use anything you like that matches the pitch. You could even solder wires directly.
(I have not tested all of the parts on this list yet.)


meat probes, https://www.amazon.com/Weber-iGrill-Pro-Meat-Probe/dp/B01M8LG8U5/ref=sr_1_4?ie=UTF8&qid=1549819424&sr=8-4&keywords=igrill+2+probes


2n2222a or 2n2222 these are cheap and appear to be everywhere except Digikey. Looks like they are becoming obsolete. You will need three of them.	
https://www.amazon.com/2N2222-Plastic-Encapsulate-Power-Transistors-600mA/dp/B06XPWS52G/ref=sr_1_2?ie=UTF8&qid=1530481975&sr=8-2&keywords=2n2222a

This is the particle photon. The brains of the operation.
https://www.amazon.com/Particle-Reprogrammable-Development-Prototyping-Electronics/dp/B016YNU1A0/ref=sr_1_2?ie=UTF8&qid=1550113530&sr=8-2&keywords=photon


Optional items:



5vdc buzzer 12mm use this if you want a microwave like beep for the alarm. Otherwise attach whatever you want to the relay, noise maker, light, spinning flag motor.
	https://www.amazon.com/gp/product/B00B0Q4KKO/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1

SS Relay	these work good if you are going to do some high power electric smoking
	https://www.amazon.com/gp/product/B0087ZTN08/ref=oh_aui_search_detailpage?ie=UTF8&psc=1

Antenna
Add an Antenna for extra range. Be sure to switch the text to external in the program.
	https://www.amazon.com/gp/product/B01GMBUS8O/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1

bbq guru Fan	
If you want to use a fan. This is an option. although it does not like pwm so you need to set the pulse range threshold to 100. Otherwise many 12vdc pc fans work well at about 30%
https://www.bbqguru.com/storenav?CategoryId=2&ProductId=44





