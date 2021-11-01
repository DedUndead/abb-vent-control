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

## Functionalities
 
The web application consists of several functionalities: 
1. Authentication
2. Communication with the database
3. Communication with the ABB-vent via MQTT
4. Communication via web sockets
5. Visualizing data

#### Authentication
The web application will prompt the user with a login web page. The user will need to have a username and password, as the user database is pre-configured. 
Authentication is a vital part of the web application since, without authentication, the user can not access any resources.

The authentication uses the Passport module with a passport-local strategy. The concept validates that the username exists in the database, if it does not the user will be prompted with an error message. If the username exists, it will proceed and decrypt the password in the database and compare it with the input in the website. In case the password does not match, the user will be prompted with an error message.

In addition, if a user tries to access the resources by entering a URL. The user is checked if the user is authenticated. If the user is not authenticated, the user will be redirected to the login page and prompted with an error message.

#### Communication with the database
The database of choice is MongoDB,  where we created four different clusters to handle the incoming and outgoing data. Mongoose facilitates the data handling in the following aspects:
Creates models that will require the incoming data to match the requirements set by the schema model.
Ease the interaction with the database.
Works in an asynchronous environment
As previously mentioned, the database stores four different kinds of data:
 User's information for authentication
 Sensor data received by the ABB-vent
User activity on the website
User session, to increase the longevity of the user on the website without the need to log in again

#### Communication with the ABB-vent via MQTT
On the server-side of the web application exists an MQTT communication functionality. By having this functionality, the server can send data and receive data via the MQTT communication protocol.  The data sent and received is in JSON format. 
The data received by the MQTT is stored in the database and affects different elements on the webpage. For example, the sliders, the toggle button, and more. In addition, the data sent from the server will affect the ABB-vent according to the settings set by the web application.

#### Communication via web sockets
Web sockets enable the real-time transfer of data between the client-side to the server-side of the web application. In this web application, we use a web socket to pass sensor data from the server-side to the client-side to adjust the elements inside of the webpage. In addition, the client-slide can set the configuration of the ABB-vent with the elements present on the Dashboard page.  The web socket will react to change and send the data to the server-side.

The use of web sockets comes in handy to assist in visualizing data. On the stats page, the web socket delivers the data from the database into the webpage and visualizes it. 

How does it work? 
The web socket prompts the server-side that the webpage is loaded or on user input. When the server-side receives that notification, it retrieves the data from the database and prompts the client-side that the data is ready. The client-side uses the data to visualize or adjust elements on the webpage. A similar chain of actions happens on the dashboard page.

#### Visualizing data
The data visualized is divided into four different sections:
Bar chart - displays the count of sessions that a user had on the web application
Pie chart - displays the event impact on the web application by each user
Line chart 1 - displays the fan speed according to the date range placed by the user.
Line chart 2 - displays the fan pressure according to the date range placed by the user.