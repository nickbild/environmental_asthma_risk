import csv


cnt = 0

with open('beijing_weather_health.csv', newline='') as csvfile:
	reader = csv.DictReader(csvfile)
	for row in reader:
		f = open("ei_data/{0}.{1}.csv".format(row['Asthma'], cnt), "w")
		f.write("temperature,humidity\n")
		f.write("{0},{1}".format(row['temp_lag0'], row['humid_lag0']))
		f.close()

		cnt += 1
