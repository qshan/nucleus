# TODO - Add more strings including utf-8 etc
# TODO - add deliberate error

import requests
import json

print("Testing basic request")

server = "http://127.0.0.1:20000"
api_url = f"{server}/api/v1/app/message"
response = requests.get(api_url)

assert response.status_code == 200, f"Actual:{response.status_code}"

print(json.dumps(response.json(), indent=4))

assert "value" in response.json()["data"]
assert response.json()["data"]["value"] == "Hello World"

print("Testing basic post")

j = {"value": "Hello Back"}
response = requests.put(api_url, json=j)
assert response.status_code == 200, f"Actual:{response.status_code}"

print(json.dumps(response.json(), indent=4))

print("Testing post was successful")

response = requests.get(api_url)
assert response.status_code == 200, f"Actual:{response.status_code}"
print(json.dumps(response.json(), indent=4))

assert response.json()["data"]["value"] == "Hello Back"

exit(0)
