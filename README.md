# The-Smokey-Powell
The Particle Photon Wifi Blynk controlled BBQ Smoker controller

This controller reads temperature through a thermistor and controls the heat of your smoker through a fan or a
Solid State relay. It has three extra probes to be used as meat probes.
Wifi is used to connect to the cloud. 

The blynk app is used as the main interface and is worth noting that all the info runs though the internet. So 
one could be shopping for supplies while also monitoring ones bbq.

A 4x20 I2c LCD is used as a local display and four buttons as an interface
This controller uses replacment thermistors for a Maverick ET732/ET733 grilling thermometer



I will include links below or look above in the files.

The shared particle app https://go.particle.io/shared_apps/5b414976d21fee30e00001c7

The Blynk App (Look Above)

The circuit board gerber file	(Look Above)
I may also decide to sell them

where I have my curcuit boards made http://www.pcbunlimited.com/

schematic of circuit board	(Look Above)

thingiverse link to make 3d printed case and DIN mounts

youtube link to video


Smokey Powell shopping list

digikey (some items are optional) you can use screw terminals or barrel jacks for power, outputs and inputs, screen signal. Really you can use anything you like that matches the pitch. You could even solder wires directly.
I have not tested any of the parts on this list yet.


amazon
meat probes, any Maverick ET-732/733 replacement probe should work. I use the meat probes for both tasks
https://www.amazon.com/gp/product/B01FVQHTUQ/ref=oh_aui_detailpage_o09_s00?ie=UTF8&psc=1

2n2222a or 2n2222 these are cheap and appear to be everywhere except Digikey. Looks like they are becoming obsolete	
https://www.amazon.com/2N2222-Plastic-Encapsulate-Power-Transistors-600mA/dp/B06XPWS52G/ref=sr_1_2?ie=UTF8&qid=1530481975&sr=8-2&keywords=2n2222a



Optional items 




Amazon
5vdc buzzer 12mm use this if you want a microwave like beep for the alarm. Otherwise attach whatever you want to the relay, noise maker, light, spinning flag motor.
	https://www.amazon.com/gp/product/B00B0Q4KKO/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1

SS Relay	these work good if you are going to do some high power electric smoking
	https://www.amazon.com/gp/product/B0087ZTN08/ref=oh_aui_search_detailpage?ie=UTF8&psc=1

Antenna
Add an Antenna for extra range. Be sure to switch the text to external.
	https://www.amazon.com/gp/product/B01GMBUS8O/ref=oh_aui_detailpage_o01_s00?ie=UTF8&psc=1

bbq guru
Fan	If you want to use a fan instead, this is an option. although it does not like pwm so you need to set the pulse range threshold to 100. Otherwise many 12vdc pc fans work well at about 30%
https://www.bbqguru.com/storenav?CategoryId=2&ProductId=44





