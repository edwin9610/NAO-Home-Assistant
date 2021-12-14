import paramiko
import pyowm
import time
import paho.mqtt.client as mqtt
import pywhatkit as pwk
import os
import sys
from scp import SCPClient

mqtt.Client.connected_flag=False #create flag in class
broker="localhost"
topic_1 = "bedroom"
topic_2 = "kitchen"
topic_3 = "christmas"

def on_connect (client,userdata,flags,rc):
    if rc==0:
        client.connected_flag=True #set flag
        print("client is connected")
        connected=True
    else:
        print("connection failed Returned code=",rc)

def on_publish(client,userdata,result):
    print("data published ")
    pass
        
client = mqtt.Client("mqtt")             #create new instance
client.on_connect=on_connect  #bind call back function
print("Connecting to broker ",broker)
client.on_publish = on_publish
client.connect(broker)      #connect to broker

client.loop_start()

def createSSHClient(server, port, user, password):
    client_1 = paramiko.SSHClient()
    client_1.load_system_host_keys()
    client_1.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    client_1.connect(server, port, user, password)
    return client_1

def light():
    scp.get(remote_path = remotepath + "light.txt")
    save_file = open("light.txt","r")
    save = save_file.read()
    print(save)
    if save == "bedroom_1":
        client.publish(topic_1,"1_1")
        save_file.close()
    elif save == "bedroom_0":
        client.publish(topic_1,"1_0")
        save_file.close()
    elif save == "kitchen_1":
        client.publish(topic_2,"2_1")
        save_file.close()
    elif save == "kitchen_0":
        client.publish(topic_2,"2_0")
        save_file.close()
    elif save == "all_1":
        client.publish(topic_1,"1_1")
        client.publish(topic_2,"2_1")
        save_file.close()
    elif save == "all_0":
        client.publish(topic_1,"1_0")
        client.publish(topic_2,"2_0")
        save_file.close()
    else:
        pass
    
    scp.get(remote_path = remotepath + "lightflag.txt")
    save_file_3 = open("lightflag.txt","w+")
    save_file_3.write("False")
    save_file_3.close()
    scp.put(remote_path = remotepath, files = "lightflag.txt")

def weather():
    scp.get(remote_path = remotepath + "city.txt")
    city_file = open("city.txt","r")
    city = city_file.read()

    APIKEY = '78ca4bba6422cc0c92bf1bdac0ebbc3a'
    OpenWMap = pyowm.OWM(APIKEY)  # Use API key to get data
    Weather=OpenWMap.weather_at_place(city)  # give where you need to see the weather
    Data=Weather.get_weather()
    
    temp = Data.get_temperature(unit='celsius')      # get current temparature in celsius 
    print ("Average Temp. Currently ", temp['temp']) # get avg. tmp

    humidity = Data.get_humidity() # get current humidity 
    print ("Humidity : ",humidity) # print humidity

    wind = Data.get_wind() # get current wind 
    print ("Wind Speed : ",wind['speed']) # print wind speed

    cloud = Data.get_clouds() # get current cloud 
    print ("Cloud Coverage Percentage : ",cloud) # print cloud coverage percentage

    status = Data.get_status() # get status of current weather 
    print ("Status of current weather : ",status)

    forecast = "The temperature in {} is {} degrees celsius. The humidity is {}. The wind speed is{}. The cloud coverage is {}percent. The status of current weather: {}".format(city,round(temp['temp']),humidity,wind['speed'], cloud, status)
    scp.get(remote_path = remotepath + "weather.txt")
    save_file_1 = open("weather.txt","w+")
    save_file_1.write(forecast)
    save_file_1.close()
    scp.put(remote_path = remotepath, files = "weather.txt")

    scp.get(remote_path = remotepath + "weatherflag.txt")
    save_file_2 = open("weatherflag.txt","w+")
    save_file_2.write("False")
    save_file_2.close()
    scp.put(remote_path = remotepath, files = "weatherflag.txt")

def security():
    t = time.localtime()
    time_second = t.tm_sec
    time_minute = t.tm_min
    time_hour = t.tm_hour
    client.publish(topic_1,"1_3")
    client.publish(topic_2,"2_3")
    
    if 60 - time_second < 20:
        time_minute = time_minute + 2

    else:
        time_minute = time_minute + 1
        
    pwk.sendwhatmsg("","Alert! Someone else is in your house!", time_hour, time_minute, tab_close = True)

def christmas():
    scp.get(remote_path = remotepath + "christmas.txt")
    save_file = open("christmas.txt","r")
    save = save_file.read()
    
    if save == "0":
        ret=client.publish(topic_3,"0")
        save_file.close()
        
    elif save == "1":
        ret=client.publish(topic_3,"1")
        save_file.close()

    elif save == "2":
        ret=client.publish(topic_3,"2")
        save_file.close()

    elif save == "3":
        ret=client.publish(topic_3,"3")
        save_file.close()

    elif save == "4":
        ret=client.publish(topic_3,"4")
        save_file.close()

    elif save == "5":
        ret=client.publish(topic_3,"5")
        save_file.close()

    scp.get(remote_path = remotepath + "christmasflag.txt")
    save_file_4 = open("christmasflag.txt","w+")
    save_file_4.write("False")
    save_file_4.close()
    
if __name__ == "__main__":
    try:
        server = "192.168.137.57"
        user = "nao"
        password = "nao"
        port = "22"
        remotepath= "/home/nao/"
        
        ssh = createSSHClient(server, port, user, password)
        scp = SCPClient(ssh.get_transport())
           
        while True:
            client.loop_start()
            if client.connected_flag:
                
                scp.get(remote_path = remotepath + "lightflag.txt")
                light_file = open("lightflag.txt","r")
                light_file = light_file.read()
                
                scp.get(remote_path = remotepath + "weatherflag.txt")
                weather_file = open("weatherflag.txt","r")
                weather_file = weather_file.read()

                scp.get(remote_path = remotepath + "securityflag.txt")
                security_file = open("securityflag.txt","r")
                security_file = security_file.read()

                scp.get(remote_path = remotepath + "christmasflag.txt")
                christmas_file = open("christmasflag.txt","r")
                christmas_file = christmas_file.read()
                
                if light_file == "True":
                    light()
                else:
                    pass

                if weather_file == "True":
                    weather()
                else:
                    pass

                if security_file == "True":
                    security()
                else:
                    print("security off")
                    pass

                if christmas_file == "True":
                    christmas()
                else:
                    pass

    except KeyboardInterrupt:
        print("connection failed")
        client.loop_stop()    #Stop loop 
        client.disconnect() # disconnect
        exit(1) #Should quit or raise flag to quit or retry
