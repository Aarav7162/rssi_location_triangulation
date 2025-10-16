import requests
import time
import json
import math
import statistics

# --- CONFIG ---
BLYNK_AUTH = "ZoFyLt0hTo429oaxvlmIcqsTp9Fxd1sI"
NODES = ["V0", "V1", "V2", "V3"]
CALIBRATION_FILE = "fingerprint_data.json"
SAMPLES = 20
INTERVAL = 0.5

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

def load_fingerprints():
    with open(CALIBRATION_FILE, "r") as f:
        data = json.load(f)
    return {tuple(map(float, k.split("_"))): v for k, v in data.items()}

def find_closest_point(rssi_avg, fingerprints):
    best_point = None
    min_dist = float('inf')
    for point, fp_rssi in fingerprints.items():
        dist = math.sqrt(sum((rssi_avg[n]-fp_rssi[n])**2 for n in NODES))
        if dist < min_dist:
            min_dist = dist
            best_point = point
    return best_point

def calculate_angle(x, y):
    return math.degrees(math.atan2(y, x))

def main():
    fingerprints = load_fingerprints()
    print("🔧 Live RSSI Tracking Started...\n(Press Ctrl+C to stop)\n")

    try:
        while True:
            collected = []
            for _ in range(SAMPLES):
                collected.append(get_rssi())
                time.sleep(INTERVAL)
            avg_rssi = average_samples(collected)

            x, y = find_closest_point(avg_rssi, fingerprints)
            angle = calculate_angle(x, y)

            print(f"RSSI: {avg_rssi}")
            print(f"→ Estimated Position: ({x:.2f}, {y:.2f}) meters")
            print(f"→ Angle from origin: {angle:.2f}°")
            print(f"[SERVO PLACEHOLDER] Would rotate to {angle:.2f}°\n")

    except KeyboardInterrupt:
        print("\nTracking stopped.")

if __name__ == "__main__":
    main()
