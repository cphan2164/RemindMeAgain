# Importing all of the required info
from flask import Flask
from flask import request
# Current data and time
from datetime import datetime
# API for google sheets
import gspread
# Authorization for google sheets
from oauth2client.service_account import ServiceAccountCredentials
import requests

app = Flask(__name__)

#The current date
currentDate = datetime.now().date()

# Function used to send sms texts to your phone
def sendTexts(texts):
	#Loops through texts and sends them except for frist row in google sheets which is the header
	for text in texts[1:]:
		# First column is message
		message=text[0]
		# Second column is time message was originally sent to google sheets
		time = text[1]
		# Prints the message as it shows on phone in console
		print(message + " at " + time)
		#Webhook and send text to IFTTT
		requests.post('https://maker.ifttt.com/trigger/Send_Message/with/key/bvwJ82DPCRiq0mFFrLcrRj', json={"value1" : message, "value2": time})

# Using News API and Open Weather API we get the current news and weather and send it to the phone
# Returns instance
def getWeather():
    # Construct the API URL
    # Position of SDSU
    lat = 32.7774
    lon = -117.0714
    #API key given by getWeather, but I took out the last value so it can't be used by others
    api_key = 'f0ac63922378fabc0563d6e9a83f'
    # URL for open weather
    url = f"http://api.openweathermap.org/data/2.5/forecast?lat={lat}&lon={lon}&appid={api_key}"

    # Send the API request
    response = requests.get(url)

    # Check if the request was successful
    if response.status_code != 200:
        print("Error: Could not retrieve weather forecast.")
        return None

    # Parse the response JSON data
    data = response.json()

    # Extract the forecast data and put it into one large string, this way it is easy to send as text
    today = datetime.now().strftime("%Y-%m-%d")
    forecast_str = "Weather:\n"
    for item in data['list']:
        date = item['dt_txt'].split()[0]
        if date == today:
            time = item['dt_txt'].split()[1]
            kelvin_temp = item['main']['temp']
            fahrenheit_temp = round((kelvin_temp - 273.15) * 9/5 + 32, 2)
            desc = item['weather'][0]['description']
            forecast_str += f"{time}: {desc}, {fahrenheit_temp}°F\n"
    #Country code for the US
    country_code = 'us'
    #API key for News API took out last digit so it can't be used
    api_key = '41879b30d2284f80a578750cc29d'
    # Construct the API URL
    url = f"https://newsapi.org/v2/top-headlines?country={country_code}&apiKey={api_key}"

    # Send the API request
    response = requests.get(url)

    # Check if the request was successful
    if response.status_code != 200:
        print("Error: Could not retrieve news headlines.")
        return None

    # Parse the response JSON data
    data = response.json()

    # Extract the top headlines (up to 2)
    headlines = [article['title'] for article in data['articles'][:2]]

    # Format the headlines into a string
    headlines_str = "News:\n"
    for i, headline in enumerate(headlines):
        headlines_str += f"{i+1}. {headline}\n"
        if i >= 2:
            break

    #Prints what it will look like on phone to console
    print(forecast_str)
    print()
    print(headlines_str)
    # Sends string to IFTTT to send to phone first weather and then headlines
    requests.post('https://maker.ifttt.com/trigger/weather/with/key/bvwJ82DPCRiq0mFFrLcrRj', json={"value1" : forecast_str})
    requests.post('https://maker.ifttt.com/trigger/weather/with/key/bvwJ82DPCRiq0mFFrLcrRj', json={"value1" : headlines_str})

# This functions checks the google sheets to see if there is a reminder to send to phone
def checkGSheets():
    # Use credentials to create a client to interact with the Google Drive API
    scope = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']
    creds = ServiceAccountCredentials.from_json_keyfile_name('creds.json', scope)
    client = gspread.authorize(creds)

    # Open the workbook and sheet
    #APIdataSheet is unique name for my Google Sheet
    sheet = client.open('APIdataSheet').sheet1

    # Get all the values from the sheet
    values = sheet.get_all_values()

    # Print the values
    num_rows = sheet.row_count

    #Prints the number of rows to console
    print("Number of rows: ", num_rows)
    #If number of rows is more then one, meaning more then header it sends all the values to sendTexts() to send the text to the phone
    if(num_rows > 1):
        sendTexts(values)
	#Returns instance
        return 3
    return 1


@app.route('/humTemp')
# Main route used from TTGO ESP32
def humTemp():
	# Default the instance or variation to 1
	variation = "1"
	#Gets temperature and humidity from TTGO ESP32
	temp = request.args.get("temp")
	humi = request.args.get("humi")
	# Saves a string for temperature and humidity in room
	roomTemp = ("--\nThe Temperature is " + temp + " degrees C\nThe Humidity is " + humi + "% rH\n--")
	# Prints the string
	print(roomTemp)
	# Checks if it is a new day and if it is it changes variation to 2
	if(datetime.now().date() > currentDate):
		variation = "2"
	# If variation is 2 it sends the news and weather from APIS to phone
	if(variation == "2"):
		#Calls getWeather() function
		getWeather()
	#Calls checkGSheets() to see if there is messages to be sent,  if there is it returns 3
	if(checkGSheets() == 3):
		# If variation is 2 and 3 then it changes variation to 4
       		if(variation == "2"):
        		variation = "4"
		# Otherwise variation turns from 1 to 3
        	else:
            		variation = "3"
	# If variation is still 1 then it sends the roomTemp string to phone
	if(variation == "1"):
		#Sends string to IFTTT to send to phone
		requests.post('https://maker.ifttt.com/trigger/weather/with/key/bvwJ82DPCRiq0mFFrLcrRj', json={"value1" : roomTemp})
	return variation

@app.route('/button')
# This is for when the reminders need to be deleted called by button on Raspberry Pi
def stop():
    # Use credentials to create a client to interact with the Google Drive API
    scope = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']
    creds = ServiceAccountCredentials.from_json_keyfile_name('creds.json', scope)
    client = gspread.authorize(creds)

    # Open the workbook and sheet
    sheet = client.open('APIdataSheet').sheet1

    #get all rows except the first one
    rows = sheet.row_count

    #delete all rows except the first one
    sheet.delete_rows(2, rows)

    #Send text saying reminders deleted
    print("Reminders Deleted")
    return "Deleted Rows"
