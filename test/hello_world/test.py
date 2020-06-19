# TODO - Add more strings including utf-8 etc
# TODO - add deliberate error
from test_case_helpers import ctest, test_case_start, test_case_end, test_case_utils
import requests
import json

test_case_start()

server = test_case_utils.get_server_url()

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

test_case_end()
