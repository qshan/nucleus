from test_case_helpers import ctest, test_case_start, test_case_end, test_case_check, test_case_utils as utils
import requests
import json

test_case_start()

server = utils.get_server_url()

api_url = f"{server}/api/v1/app/customers"

response = requests.get(api_url)
assert utils.status_codes_equal(response.status_code, requests.codes.ok)
print(json.dumps(response.json(), indent=4))
assert len(response.json()["data"]["customers"]) == 3
test_case_check("Get customers")

j = {"name": "", "city": "", "order_count": 100}
response = requests.post(api_url, json=j)
assert utils.status_codes_equal(response.status_code, requests.codes.bad_request)
test_case_check("Post invalid values")

j = {"name": "TestCustomer", "city": "TestCity", "order_count": 100}
response = requests.post(api_url, json=j)
assert utils.status_codes_equal(response.status_code, requests.codes.created)
test_case_check("Post valid values")

api_url = f"{server}/api/v1/app/customers"
response = requests.get(api_url)
assert utils.status_codes_equal(response.status_code, requests.codes.ok)

print(json.dumps(response.json(), indent=4))
for customer in response.json()["data"]["customers"]:
    if customer["name"] == j["name"] and customer["city"] == j["city"] and customer["order_count"] == j["order_count"]:
        break
else:
    assert False, f"Unable to find added customer after post: " + str(j)
test_case_check("Check for added customer")

test_case_end()
