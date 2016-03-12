# Overview

This simple node listens on the /rfid/rfid_detect topic for infomation about the incoming tags and detects their movement.
It builds a simple frequency/phase model of the individual tags and compares the incoming data with these models to find out if a given tag was moved.

Model building requires to collect some information about the tags first, which should take approximatelly 15 seconds.
During this time, the tags should not be moved. The node will report that the model building was finished.

Upon termination, the model prints all tags that were detected in it's range including information about if these tags were moved.

The node publishes IDs of the TAGs on three topics:
 
1. <i>/rfid/moved</i> - which reports ID of a TAG at the start of its detected movement. 
1. <i>/rfid/detected</i> - which reports first detection of a TAG with a given ID. 
1. <i>/rfid/model_ready</i> - which reports that enough measurements of a TAG were taken to determine if it was moved.
