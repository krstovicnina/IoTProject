# ComputerVisionProject
Smart House simulation Arduino project created for the Internet of Things course. 

Final exam project
Internet of Things

In this task, a control system should be created in the "Smart Home". The smart home contains heating system, cooling system, remote lights control, motion detection, temperature and illumination sensor.

•Heating system –should contain heater (resistor) which is controlled by relay

•Cooling system-should contain a DC motor that is turned on and off by a relay

•Lights –OneLED controlled with relay

•Temperature sensor –LM35 for measurement ambient temperature

•Illumination –photo resistor  

•Motion sensor -HC-SR501


Heating  and  cooling  system  cannot  work in the  same  time. For  these  two  systems it is  necessary to implement temperature hysteresis control. 
If  measured  temperature  is  higher  than  23°C  cooling  system should be turned on until temperature is 17°C. 
If measured temperature is lower than 17°C heating system should be on until temperature reach 23°C. 
Temperature and illumination are measured every 10 minutes. 
User should be  able to turn the  light on and  off through communication with PC.
System has home light auto mode, when this mode is active and measured illumination is lower than 30% light should be on, otherwise should be off. 
When user send command to turn on or off light, then home light auto mode is deactivated. 
System has home secure mode, when this mode is active light is turned on every time when motion is detectedand user should be notified about it via e-mail.
If there is no motion next 10 seconds after that lightisturned off, only one notification should be sent within these 10 seconds.
User can activate or deactivate both of previously mentioned modesvia e-mail. 
It  is  necessary  to  enable remote control  of  heating,  cooling  system,  as  well  as  lights, control  is performed via e-mail based on the given e-mail Subject. In addition to control, it is necessary to enable the sending of all measurements and detections to theankspeak.com. 
Also, the system should be able to send a daily report every day, but also as the request of the userthorough email. 
If the user requests it, a report should be created from the beginning of that day. 

The report should contain:

1.Minimal daily temperature 

2.Maximal daily temperature

3.Average temperature per hour

4.Daily average temperature 

5.Graph with all temperature measurements

6.Minimal daily illumination 

7.Maximal daily illumination

8.Average illumination per hour

9.Daily average illumination

10.Graph with all illumination measurements

11.Total number of detections

12.Average number of detections per hour

13.Graph with number of detections per hour

14.Duration how long home secure modewas Turned ON

15.Duration how long light auto modewas Turned ON
