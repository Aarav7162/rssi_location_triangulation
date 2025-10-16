import requests
import time
import json
import statistics
import numpy as np

# --- CONFIG ---
BLYNK_AUTH = "ZoFyLt0hTo429oaxvlmIcqsTp9Fxd1sI"
NODES = ["V0", "V1", "V2", "V3"]
CALIBRATION_FILE = "fingerprint_data.json"
SAMPLES_PER_POINT = 20
INTERVAL = 1

ROOM_WIDTH = 2.65
ROOM_HEIGHT = 2.4
TOTAL_POINTS = 200

def get_rssi():
    readings = {}
    for node in NODES:
        try:
            url = f"https://blynk.cloud/external/api/get?token={BLYNK_AUTH}&{node}"
            resp = requests.get(url, timeout=3)
            val = int(resp.text) if resp.text.strip() else -100
        except:
            val = -100
        readings[node] = val
    return readings

def average_samples(samples):
    return {node: round(statistics.mean([s[node] for s in samples]), 2) for node in NODES}

def generate_points(total_points, width, height):
    rows = int(np.sqrt(total_points))
    cols = int(np.ceil(total_points / rows))
    x_coords = np.linspace(0, width, cols)
    y_coords = np.linspace(0, height, rows)
    points = []
    for y in y_coords:
        for x in x_coords:
            points.append((round(x, 3), round(y, 3)))
    return points[:total_points]

def main():
    try:
        with open(CALIBRATION_FILE, "r") as f:
            data = json.load(f)
    except FileNotFoundError:
        data = {}

    points = generate_points(TOTAL_POINTS, ROOM_WIDTH, ROOM_HEIGHT)
    print(f"Total calibration points: {len(points)}\n")

    for idx, (x, y) in enumerate(points, 1):
        input(f"Move to point {idx}/{TOTAL_POINTS} ({x}, {y}) and press Enter to start calibration...")
        print(f"Collecting {SAMPLES_PER_POINT} RSSI samples at ({x}, {y})...")
        collected = []
        for i in range(SAMPLES_PER_POINT):
            rssi = get_rssi()
            collected.append(rssi)
            print(f" Sample {i+1}/{SAMPLES_PER_POINT}: {rssi}")
            time.sleep(INTERVAL)
        avg_rssi = average_samples(collected)
        data[f"{x}_{y}"] = avg_rssi
        print(f" → Average RSSI: {avg_rssi}\n")

        # Save progress after each point
        with open(CALIBRATION_FILE, "w") as f:
            json.dump(data, f, indent=4)

    print(f"✅ Calibration complete! All data saved to {CALIBRATION_FILE}")

if __name__ == "__main__":
    main()
