import tkinter as tk
import threading
import time
import requests
import pywifi
from pywifi import const
import math

BLYNK_AUTH = "ZoFyLt0hTo429oaxvlmIcqsTp9Fxd1sI"
BASE_URL = f"https://blynk.cloud/external/api/get"

RSSI_VPIN_NODE1 = 0
RSSI_VPIN_NODE2 = 1

rssi_node1 = 0
rssi_node2 = 0
rssi_node3 = 0

def fetch_rssi(vpin):
    try:
        response = requests.get(f"{BASE_URL}?token={BLYNK_AUTH}&V{vpin}")
        if response.status_code == 200 and response.text.strip() != "null":
            return int(float(response.text.strip()))
    except:
        pass
    return 0

def get_rssi(target_ssid="Pacific.2.4Ghz"):
    wifi = pywifi.PyWiFi()
    iface = wifi.interfaces()[0]
    iface.scan()
    time.sleep(1)
    scan_results = iface.scan_results()
    for network in scan_results:
        if network.ssid == target_ssid:
            return network.signal
    return 0

class RadarGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("RSSI Radar - Week 1")
        self.geometry("900x750")
        self.configure(bg="black")

        self.canvas = tk.Canvas(self, width=900, height=650, bg="black", highlightthickness=0)
        self.canvas.pack(pady=10)

        self.center_x = 450
        self.center_y = 325
        self.radius = 280

        self.scale_x = self.radius / 78.0
        self.scale_y = self.radius / 40.0

        self.draw_grid_with_distances()

        font_style = ("Courier New", 14, "bold")
        self.rssi1_label = tk.Label(self, text="Node 1 RSSI: 0", fg="lime", bg="black", font=font_style)
        self.rssi1_label.place(x=20, y=30)
        self.rssi2_label = tk.Label(self, text="Node 2 RSSI: 0", fg="lime", bg="black", font=font_style)
        self.rssi2_label.place(x=20, y=60)
        self.rssi3_label = tk.Label(self, text="Node 3 RSSI: 0", fg="lime", bg="black", font=font_style)
        self.rssi3_label.place(x=20, y=90)

        self.update_gui()
        self.update_node3_rssi()
        self.update_from_blynk()

    def draw_grid_with_distances(self):
        rings = 5
        for i in range(1, rings + 1):
            r_x = i * self.radius / rings
            r_y = i * self.radius / rings
            self.canvas.create_oval(
                self.center_x - r_x,
                self.center_y - r_y,
                self.center_x + r_x,
                self.center_y + r_y,
                outline="green"
            )

        self.canvas.create_line(self.center_x, self.center_y - self.radius, self.center_x, self.center_y + self.radius, fill="green")
        self.canvas.create_line(self.center_x - self.radius, self.center_y, self.center_x + self.radius, self.center_y, fill="green")

        font = ("Courier New", 10)
        self.canvas.create_text(self.center_x + self.radius, self.center_y + 15, text="+78 cm", fill="lime", font=font)
        self.canvas.create_text(self.center_x - self.radius, self.center_y + 15, text="-78 cm", fill="lime", font=font)
        self.canvas.create_text(self.center_x - 30, self.center_y - self.radius, text="+40 cm", fill="lime", font=font)
        self.canvas.create_text(self.center_x - 30, self.center_y + self.radius, text="-40 cm", fill="lime", font=font)

    def update_gui(self):
        self.rssi1_label.config(text=f"Node 1 RSSI: {rssi_node1}")
        self.rssi2_label.config(text=f"Node 2 RSSI: {rssi_node2}")
        self.rssi3_label.config(text=f"Node 3 RSSI: {rssi_node3}")
        self.draw_nodes()
        self.after(500, self.update_gui)

    def update_node3_rssi(self):
        global rssi_node3
        rssi_node3 = get_rssi("Pacific.2.4Ghz")
        self.after(2000, self.update_node3_rssi)

    def update_from_blynk(self):
        global rssi_node1, rssi_node2
        rssi_node1 = fetch_rssi(RSSI_VPIN_NODE1)
        rssi_node2 = fetch_rssi(RSSI_VPIN_NODE2)
        self.after(1000, self.update_from_blynk)

    def draw_nodes(self):
        """Draw nodes fixed at 225°, 135°, and 315° on the edge."""
        self.canvas.delete("nodes")

        nodes = [
            (225, "red", "Node 1"),
            (135, "blue", "Node 2"),
            (315, "yellow", "Node 3")
        ]

        node_radius = 8

        for angle_deg, color, label in nodes:
            rad = math.radians(angle_deg)
            x = self.center_x + math.cos(rad) * self.radius
            y = self.center_y - math.sin(rad) * self.radius

            self.canvas.create_oval(x - node_radius, y - node_radius, x + node_radius, y + node_radius,
                                    fill=color, outline="", tags="nodes")

            offset = 20
            lx = self.center_x + math.cos(rad) * (self.radius + offset)
            ly = self.center_y - math.sin(rad) * (self.radius + offset)
            self.canvas.create_text(lx, ly, text=label, fill=color, font=("Courier New", 10), tags="nodes")

if __name__ == "__main__":
    app = RadarGUI()
    app.mainloop()
