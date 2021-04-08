
import numpy as np
import serial
import os
import time
import json
from serial.tools import list_ports


class Laser():
    """

    """

#--------------------------------- Initializing --------------------------------
    def __init__(self):
        self.rep_rates_kHz = {0:50, 1:100, 2:200, 3:299.625, 4:400, 5:500,
                                6:597.015, 7:707.965, 8:800, 9:898.876, 10:1000}
        return



#------------------------------- Serial functions ------------------------------
    def start_serial(self, serial_name):
        try:
            self.ser = serial.Serial(serial_name, 38400, timeout=.1)
            print("Connection is established")
        except Exception as e:
            print(e)
            print("Could not open serial")
        return


    def close_serial(self):
        if self.ser.is_open:
            self.ser.close()
        return

    def serial_read(self):
        return

    def send_cmd(self, command):
        serial_cmd = command + "\n"
        try:
            self.ser.write(str.encode(serial_cmd))
        except:
            print("Command %s not sent. Could not open serial" %serial_cmd)
        return

#------------------------------- Commands  .......------------------------------
    def go_to_standby(self):
        self.send_cmd('ly_oxp2_standby')

    def go_to_listen(self):
        self.send_cmd('ly_oxp2_listen')

    def enable_laser(self):
        self.send_cmd('ly_oxp2_enabled')

    def enable_AOM_laser(self):
        self.send_cmd('ly_oxp2_output_enable')

    def disable_AOM_laser(self):
        self.send_cmd('ly_oxp2_output_disable')


    def set_pulse_energy(self, energy, unit):
        if unit == "uJ":
            energy_nJ = energy * 1000
            command = 'ly_oxp2_power=' + str(energy_nJ)
            self.send_cmd(command)
        elif unit == "nJ":
            energy_nJ = energy_nJ
            command = 'ly_oxp2_power=' + str(energy_nJ)
            self.send_cmd(command)

    def set_repetition_rate(self, freq):
        freq_nr = list(self.rep_rates_kHz.keys())[list(self.rep_rates_kHz.values()).index(freq)]
        command = 'e_freq=' + str(freq_nr)
        self.send_cmd(command)

    def get_measured_pulse_energy(self):
        self.send_cmd('e_mlp?')

    def get_repetition_rate(self):
        self.send_cmd('e_freq?')


#--------------------------------- if __main__ ---------------------------------
if __name__ == "__main__":
    laser = Laser()
    ports = (list(list_ports.comports()))
    port_names = list(map(lambda p : p.device, ports))

    laser.start_serial(port_names[0])
    time.sleep(2)

    laser.send_cmd('e_freq_available?')
    while True:
        if laser.ser.in_waiting > 0:
            line = laser.ser.readline()
            data = line.decode("utf-8")
            print(data)
