import requests
import json
import math
import time
import statistics

BLYNK_AUTH = "YOUR_BLYNK_AUTH"
BLYNK_BASE = "https://blynk.cloud/external/api"

RSSI_VPINS = [0, 1, 2, 3]
OUTPUT_X_VPIN = 6
OUTPUT_Y_VPIN = 7
OUTPUT_DIST_VPIN = 19

def get_rssi(pin):
    try:
        url = f"{BLYNK_BASE}/get?token={BLYNK_AUTH}&v{pin}"
        response = requests.get(url)
        if response.ok:
            return float(response.text)
    except:
        return None

def write_virtual(pin, value):
    try:
        url = f"{BLYNK_BASE}/update?token={BLYNK_AUTH}&v{pin}={value}"
        requests.get(url)
    except:
        pass

def average_rssi_over_time(duration=20, interval=1):
    readings = {f"V{p}": [] for p in RSSI_VPINS}
    start = time.time()
    while time.time() - start < duration:
        for p in RSSI_VPINS:
            val = get_rssi(p)
            if val is not None:
                readings[f"V{p}"].append(val)
        remaining = duration - int(time.time() - start)
        print(f"Collecting RSSI... {remaining}s left", end="\r")
        time.sleep(interval)
    return {pin: round(statistics.mean(vals), 2) if vals else 0 for pin, vals in readings.items()}

def load_fingerprint_data(file_path="fingerprint_data.json"):
    try:
        with open(file_path, "r") as f:
            return json.load(f)
    except:
        return {}

def euclidean_distance(rssi_now, rssi_fp):
    s = 0
    for k in rssi_now.keys():
        s += (rssi_now[k] - rssi_fp.get(k, 0)) ** 2
    return math.sqrt(s)

def find_closest_position(rssi_now, fingerprints):
    best_pos = None
    min_dist = float("inf")
    for pos, rssi_fp in fingerprints.items():
        d = euclidean_distance(rssi_now, rssi_fp)
        if d < min_dist:
            min_dist = d
            best_pos = pos
    return best_pos

def main():
    print("Starting RSSI averaging...")
    averaged = average_rssi_over_time()
    print(f"\nAveraged RSSI: {averaged}")

    fingerprints = load_fingerprint_data()
    if not fingerprints:
        print("No fingerprint data found.")
        return

    best_pos = find_closest_position(averaged, fingerprints)
    if not best_pos:
        print("No matching position found.")
        return

    x, y = map(float, best_pos.split("_"))
    distance = round(math.sqrt(x**2 + y**2), 2)
    angle = round(math.degrees(math.atan2(y, x)), 2)

    print(f"Estimated position: ({x}, {y})")
    print(f"Angle from origin: {angle}°")
    print(f"Distance from origin: {distance} cm")

    write_virtual(OUTPUT_X_VPIN, x)
    write_virtual(OUTPUT_Y_VPIN, y)
    write_virtual(OUTPUT_DIST_VPIN, distance)

if __name__ == "__main__":
    main()
