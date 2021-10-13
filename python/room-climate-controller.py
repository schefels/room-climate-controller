# Importing Libraries
import serial
import csv
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation


ARDUINO = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=31)
FILE_TIMESTAMP = int(time.time())
timestamp = []
temp = []
humidity = []
co2 = []
fig, ax = plt.subplots()
fig.subplots_adjust(right=0.75)
twin1 = ax.twinx()
twin2 = ax.twinx()


def write_to_csv():
    with open(f"{FILE_TIMESTAMP}_room-climate-controller.csv", "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerows(zip(timestamp, co2, temp, humidity))


# ref: http://www.mikeburdis.com/wp/notes/plotting-serial-port-data-using-python-and-matplotlib/
# This function is called periodically from FuncAnimation
def animate(i):
    data_bytes = ARDUINO.readline()
    if len(data_bytes) > 4:
        data_str = data_bytes.decode("utf-8").split("|")
        timestamp.append(int(data_str[0].replace(" ", "")))
        temp.append(float(data_str[1].replace(" ", "").split(':')[1].split('°')[0]))
        humidity.append(float(data_str[2].replace(" ", "").split(':')[1].split('%')[0]))
        co2.append(int(data_str[3].replace(" ", "").split(':')[1].split('ppm')[0]))
        write_to_csv()

        ax.clear()
        twin1.clear()
        twin2.clear()
        p1, = ax.plot(timestamp, co2, "b.-", label="CO2 Level")
        p2, = twin1.plot(timestamp, temp, "r.-", label="Temperature")
        p3, = twin2.plot(timestamp, humidity, "g.-", label="Humidity")
        twin2.spines.right.set_position(("axes", 1.05))

        ax.set_ylim(min(co2)-min(co2)*0.1, max(co2)+max(co2)*0.1)
        twin1.set_ylim(min(temp)-min(temp)*0.2, max(temp)+max(temp)*0.3)
        twin2.set_ylim(min(humidity)-min(humidity)*0.4, max(humidity)+max(humidity)*.8)

        ax.set_xlabel("time")
        ax.set_ylabel("CO2 Level (ppm)")
        twin1.set_ylabel("Temperature (°C)")
        twin2.set_ylabel("Humidity (%)")

        ax.yaxis.label.set_color(p1.get_color())
        twin1.yaxis.label.set_color(p2.get_color())
        twin2.yaxis.label.set_color(p3.get_color())

        tkw = dict(size=4, width=1.5)
        ax.tick_params(axis='y', colors=p1.get_color(), **tkw)
        twin1.tick_params(axis='y', colors=p2.get_color(), **tkw)
        twin2.tick_params(axis='y', colors=p3.get_color(), **tkw)
        ax.tick_params(axis='x', **tkw)
        # ax.tick_params(rotation=45)

        ax.legend(handles=[p1, p2, p3])

        fig.text(0.435, 0.95, u"\u2B24\u2B24\u2B24", ha="center", va="bottom", size="large", color="black")
        if (b'31m' in data_bytes):
            fig.text(0.435, 0.95, u"\u2B24\u2001\u2001", ha="center", va="bottom", size="large", color="red")
        if (b'33m' in data_bytes):
            fig.text(0.435, 0.95, u"\u2001\u2B24\u2001", ha="center", va="bottom", size="large", color="yellow")
        if (b'32m' in data_bytes):
            fig.text(0.435, 0.95, u"\u2001\u2001\u2B24", ha="center", va="bottom", size="large", color="green")
        plt.title('Room Climate Controller', color='black')
        # print(data_bytes)
        # print(str(timestamp[len(timestamp)-1]) + " | Temperature: " + str(temp[len(temp)-1]) + " | Humidity: " +
        #       str(humidity[len(humidity)-1]) + " | CO2-Concentration: " + str(co2[len(co2)-1]))


while ARDUINO.is_open:
    fig.canvas.manager.set_window_title('Room Climate Controller')
    animation = animation.FuncAnimation(fig, animate, fargs=None, interval=10000)
    plt.show()
