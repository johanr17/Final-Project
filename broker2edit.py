import paho.mqtt.client as mqtt #import the client1
import time
import json
from collections import OrderedDict
import MySQLdb
import uuid
import datetime

## DATABASE CONNECTION

db = MySQLdb.connect(host="localhost",  # your host 
                     user="root",       # username
                     #passwd="root",     # password
                     db="pointzero")   # name of the database
## QUERYYY
mqtt_sql = (
    "insert into mqtt"
    "(channel, sf, time, gwip, gwid, repeater, systype, rssi, snr, snr_max, snr_min, macAddr, data, frameCnt, fport, devEUI)"
    "values (%(channel)s, %(sf)s, %(time)s, %(gwip)s, %(gwid)s, %(repeater)s, %(systype)s, %(rssi)s, %(snr)s, %(snr_max)s, %(snr_min)s, %(macAddr)s, %(data)s, %(frameCnt)s, %(fport)s, %(devEUI)s)")

insertIntoMQTT = (
    "insert into mqtt"
    "(channel, sf, time, gwip, gwid, repeater, systype, rssi, snr, snr_max, snr_min, macAddr, data, frameCnt, fport, devEUI)"
    "values ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')")
## Query Insert into Parsed Table store formatted data
insertIntoParsed = (
    "insert into parsed"
    "(volume, debit, device_eui)"
    "values (%(volume)s, %(debit)s, %(device_eui)s)")

# Create a Cursor object to execute queries.
cur = db.cursor()

## Parsing raw message payload from MQTT 
def parsingMessage(msg_json):
  ## Load as json object
  #msg_json = json.loads(message.payload, object_pairs_hook=OrderedDict)
  for json_object in msg_json:
    # Prepare data (formatting)
    idData = str(uuid.uuid4())
    #debit = int(str(json_object['data']).decode("hex").encode("ascii"))
    #volume = (debit / 60.0) * 1000
    data = str(json_object['data']).decode("hex").encode("ascii").split(',')
    debit = int(data[0])
    volume = int(data[1])
    #time_stamp = time.strftime("%Y-%m-%dT%H:%M:%S",json_object['time'][:9])
    #print(json_object)

 # Data for MQTT Table
   # rawData = json_object
    # Data for Parsed Table
    parsedDataRaw = '{"volume":%d, "debit":%d, "device_eui":"%s"}' % (volume, debit, json_object['devEUI'])
    print(parsedDataRaw)
    parsedData = json.loads(parsedDataRaw)
    # Insert data into Database
    #cur.execute(insertIntoMQTT % (json_object['channel'], json_object['sf'], json_object['time'], json_object['gwip'], json_object['gwid'], json_object['repeater'], json_object['systype'], json_object['rssi'], json_object['snr'], json_object['snr_max'], json_object['snr_min'], json_object['macAddr'], json_object['data'], json_object['frameCnt'], json_object['fport'], json_object['devEUI']))
    
    
    #insertIntoDatabase(json_object, parsedData)
    cur.execute(mqtt_sql, json_object)
    cur.execute(insertIntoParsed, parsedData)
  # DB Commit
  db.commit()
  
def insertIntoDatabase(rawData, parsedData):
  # Execute query 
  try:
    cur.execute(insertIntoMQTT, rawData)
    cur.execute(insertIntoParsed, parsedData)
    # DB Commit
    db.commit()
    # NB : you won't get an IntegrityError when reading
  except (MySQLdb.Error, MySQLdb.Warning) as e:
    print(e)
    return None

############
def on_log(client, userdata, level, buf):
   print("log: "+buf)

def on_disconnect(client, userdata, flags, rc=0):
   print("Disconnected, return "+str(rc))

def on_connect(client, userdata, flags, rc):
   if rc==0:
      print("\n*** connected OK")
   else:
      print("Bad connection, Return ", rc)

def on_message(client, userdata, message):
    print("message rx *********")
    print(message.payload)
    msg_json = json.loads(message.payload, object_pairs_hook=OrderedDict)
##    msg_json = json.loads(message.payload) ## No Sorting Dictionary

    parsingMessage(msg_json)
##    print("message received " ,str(message.payload.decode("utf-8")))
##    print("message topic=",message.topic)
##    print("message qos=",message.qos)
##    print("message retain flag=",message.retain)
########################################
broker_address="192.168.55.195"
#broker_address="iot.eclipse.org"
print("creating new instance")
client = mqtt.Client("P1") #create new instance
client.on_connect=on_connect
client.on_disconnect=on_disconnect
client.on_log=on_log
client.on_message=on_message #attach function to callback
print("connecting to broker")
client.connect(broker_address) #connect to broker
client.loop_start() #start the loop
print("Subscribing to topic","GIOT-GW/#")
client.subscribe("GIOT-GW/#")
##print("Publishing message to topic","house/bulbs/bulb1")
##client.publish("house/bulbs/bulb1","OFF")
##time.sleep(10) # wait
##client.loop_stop() #stop the loop
##client.disconnect
