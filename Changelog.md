# Changelog

## December 17, 2019 (JST)
* Update README.md
  * Developping environment is based on Visual Studio Community 2019
  * Version of OS and device driver are updated
  * Some typo are fixed
* At startup, issue "Get Log Page" command with LID = 02h to retreive SMART information
  * To check how many temperature sensors the device has
* Bug fix and feature enhancements for "Get Features" command with FID = 04h (Temperature Threshold)
  * Mask TMPTH with 0xFFFF (65535)
  * Retreive "Over Threshold" and "Under Threshold" for implemented temperature sensors (including Composite Temperature)

