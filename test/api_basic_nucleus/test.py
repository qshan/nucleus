import requests
import json

server = "http://127.0.0.1:20000"

response = requests.get(f"{server}/api/v1/ready")

assert response.status_code == 200, f"Status code should be 200, got {response.status_code}"

assert "data" in response.json(), "Can't find data object in response:\r\n" + json.dumps(response.json(), indent=4)

exit(0)
