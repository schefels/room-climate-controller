# Importing Libraries
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style

arduino = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=31)
timestamp = []
temp = []
humidity = []
co2 = []
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)

# ref: http://www.mikeburdis.com/wp/notes/plotting-serial-port-data-using-python-and-matplotlib/
# This function is called periodically from FuncAnimation
def animate(i, xs, ys):
    data_bytes = arduino.readline()
    data_str = data_bytes.decode("utf-8").split("|")
    if len(data_bytes) > 4:
        timestamp.append(int(data_str[0].replace(" ", "")))
        i = timestamp[-1]
        temp.append(float(data_str[1].replace(" ", "").split(':')[1].split('°')[0]))
        humidity.append(float(data_str[2].replace(" ", "").split(':')[1].split('%')[0]))
        co2.append(float(data_str[3].replace(" ", "").split(':')[1].split('ppm')[0]))

        # Limit x and y lists to 1500 items
        #xs = xs[-1500:]
        #ys = ys[-1500:]
    
        # Draw x and y lists
        ax.clear()
        ax.plot(timestamp, co2, label="CO2 Level (ppm)")
        # ax.plot(timestamp, temp, label="Temperature (°C)")
    
        # Format plot
        plt.xticks(rotation=45, ha='right')
        plt.subplots_adjust(bottom=0.30)
        plt.title('Room Climate COntroler')
        plt.ylabel('ppm')
        plt.legend()
        plt.axis([None, None, 0, 3000]) 
        print(str(timestamp[len(timestamp)-1]) + " | Temperature: " + str(temp[len(temp)-1]) + " | Humidity: " + str(humidity[len(humidity)-1]) + " | CO2-Concentration: " + str(co2[len(co2)-1]))

while arduino.is_open:
    ani = animation.FuncAnimation(fig, animate, fargs=(timestamp, co2), interval=1000)
    plt.pause(0.05)
    plt.show()
