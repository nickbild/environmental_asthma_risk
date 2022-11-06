# Environmental Asthma Risk

K-Way smart jackets are instrumented with an Arduino Nicla Sense ME.  The Arduino is housed inside a tiny case and comes wired to a rechargeable LiPo battery.  This hardware platform was designed with tinyML in mind, with an Arm Cortex M4 CPU operating at 64 MHz to run local inferences, and a slew of sensors, including a number of motion and environmental sensors.
  
One of the sensors on this board collects temperature and humidity measurements, then passes them into a machine learning model that I built with Edge Impulse Studio that predicts the number of emergency department visits for asthma that would be expected under those conditions.  If that number exceeds a certain threshold, that is considered a high-risk day for asthma flare ups, and a message is sent to the jacket wearer’s smartphone to give them a heads up so they can take appropriate action.

![](https://raw.githubusercontent.com/nickbild/environmental_asthma_risk/main/media/wearing2_sm.jpg)

Public Edge Impulse Project: https://studio.edgeimpulse.com/public/148301/latest
