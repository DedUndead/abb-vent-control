# ABB-Vent-Control
Internet of Things: ABB Ventilation Control group Project.

## Description
This part of the ABB vent control project consists of the web user interface. The user can interact with the ABB vent via the website application. 
The web user interface allows to control the following settings:
Mode - The user can toggle between automatic to manual modes
Speed - The user can change the speed of the ABB vent when the mode is on the manual mode
Pressure - The user can change the pressure of the ABB vent when the mode is on the automatic mode
In addition , the user can see the overall statistics of the data stored in the database on the stats page. For example, user sessions, user activity, and data from the sensor in between two different times.

## Technology stack

| Technology | Application |
|------------|-------------|
| anime.js   | Frontend    |
| tailwind   | Frontend    |
| NodeJS     | Backend     |
| MQTT       | Backend     |
| MongoDB    | Database    |

#### Modules
* [animejs](https://www.npmjs.com/package/animejs) - Lightweight JavaScript animation library 
* [async-mqtt](https://www.npmjs.com/package/async-mqtt) - JavaScript client library for MQTT protocol
* [bcrypt](https://www.npmjs.com/package/bcrypt) - JavaScript library to encrypt passwords
* [connect-flash](https://www.npmjs.com/package/connect-flash) - library to handle temporary messages
* [connect-mongo](https://www.npmjs.com/package/connect-mongo) - library to handle user session in the website
* [ejs](https://www.npmjs.com/search?q=ejs) - Template language to allow JavaScript in HTML markup
* [express](https://www.npmjs.com/package/express) - Minimal and flexible Node.js web application framework
* [express-session](https://www.npmjs.com/package/express-session) - Handles user session to store user session ID in the server-side
* [mongodb](https://www.npmjs.com/package/mongodb) - Handles the connection between the server to MongoDB
* [mongoose](https://www.npmjs.com/package/mongoose) - MongoDB object modeling tool which is designed to work in asynchronous environment
* [passport](https://www.npmjs.com/package/passport) - authentication middleware for NodeJS
* [passport-local](https://www.npmjs.com/package/passport-local) - passport strategy for authentication
* [socket.io](https://www.npmjs.com/package/socket.io) - handles real time bidirectional event-based
* [nodemon](https://www.npmjs.com/package/nodemon) - module to assist NodeJS application development 
* [chartjs](https://www.npmjs.com/package/chartjs) - library to handle charts on HTML
#### 

## User Manual
To run the web application, there three possible ways to download the application:

#### GitHub CLI
We can use the following command: 
<pre><code>gh repo clone DedUndead/ABB-Vent-Control</code></pre>
 
#### HTTPS
1. Open Git Bash
2. Change the to the directory that you wish to clone to repository to
3. Enter the following command:
<pre><code>git clone https://github.com/DedUndead/ABB-Vent-Control.git </code></pre>

#### GitHub desktop
GitHub desktop is a GUI version of Git Bash/ GitHub CLI. In there the user has all kind of options in a click of a button.

#### Run the web application
To run the web application, follow the following steps:
1. Have a terminal open in the directory where the application is located
2. run the following command: 
<pre><code>npm run dev</code></pre>
3. Enter http://localhost:3000/ in your web browser
4. Ask the devlopers for the credentials to access the resource of the web application

