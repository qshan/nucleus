from test_case_helpers import ctest, test_case_start, test_case_end, test_case_utils
import requests
import json

test_case_start()

server = test_case_utils.get_server_url()

api_url = f"{server}/api/v1/app/customers"

response = requests.get(api_url)
assert response.status_code == 200, f"Actual:{response.status_code}"
print(json.dumps(response.json(), indent=4))
assert len(response.json()["data"]["customers"]) == 3


j = {"name": "TestCustomer", "city": "TestCity", "order_count": 100}
response = requests.post(api_url, json=j)
assert response.status_code == 201, f"Actual:{response.status_code}"
print(json.dumps(response.json(), indent=4))


api_url = f"{server}/api/v1/app/customers"
response = requests.get(api_url)
assert response.status_code == 200, f"Actual:{response.status_code}"
print(json.dumps(response.json(), indent=4))

for customer in response.json()["data"]["customers"]:
    if customer["name"] == j["name"] and customer["city"] == j["city"] and customer["order_count"] == j["order_count"]:
        break
else:
    assert False, f"Unable to find added customer after post: " + str(j)


test_case_end()
