## Arduino timetable

# Concept

I started this project with the idea of collect and store all data (id and timestamp) of badges swipes to a remote server.
I made a YouTube video of the overall working concept, you can find it [here](https://youtu.be/9fkGFCoe40E).
The webpage that shows the data is [here](http://guglielmo-turco.com/badge/).
When I have time left, I try to improve this project and fix the issues that I have (see below).

# Technology involved

The hardware components are:
- **Arduino MEGA**
- **LCD 16x2**
- **SD datalogger board + clock**
- **Powerboost 500**
- **NFC Shield**
- **FONA GSM Shield**

The code:
- Timbracartellino_MEGA.ino
- Folder timetable-web contains all the webpages
- feed.php is the file called to post data into the database
- includes/variables.php store the configurations to connect to the database
- index.php show all the data stored into the database
- I used Sass for the "custom" style, have a look at sass/screen.scss
- I used jQuery mobile for more responsiveness and an easy-quick way to construct the page with PHP

# Issues

- After 24 hours the lcd stop working, and I think it's related to the `checkCheckSyncTime` function, because if I remove this piece of code, it works. I would like this feature to work, so I don't have to connect the hardware to the computer to update the time every 6 month.
- It takes from 5 to 8 seconds to process one single swipe.
